#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "ofxThread.h"

VideoApp theApp;//global

struct VideoGrabThread: public ofxThread
{
	bool is_new_frame;
	IplImage* frame;

	VideoInput input;
	int _argc;
	char** _argv;

	VideoGrabThread(int argc, char** argv)
	{
		_argc = argc;
		_argv = argv;
		is_new_frame = false;		
	}

	void threadedFunction()
	{
		lock();
		if (!input.init(_argc, _argv))
		{
			unlock();
			return;
		}
		is_new_frame = true;
		unlock();
		while(true)
		{
			
			is_new_frame = false;
			frame = input.get_frame();
			is_new_frame = true;
		}		
	}
};

VideoApp::~VideoApp()
{
	cvReleaseMat(&warp_matrix);
}

VideoApp::VideoApp()
{
	monitorVisible = true; 
	selected = NULL;
	warp_matrix = cvCreateMat(3, 3, CV_32FC1);

	to_reset_back = false;
	using_black_bg = false;
	using_white_bg = false;

	g_Fx = g_Fy = g_prevFx = g_prevFy = 0;
}

bool VideoApp::init(int argc, char** argv)
{
	theConfig.parse_args(argc, argv);

	if (theConfig.minim_window)
	{
#ifdef WIN32
		keybd_event(VK_LWIN, 0, 0, 0);
		keybd_event('D', 0, 0, 0);
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
#endif
	}

	if (argc > 2)
		argc = 2;//hack the _input.init

	grag_thread = new VideoGrabThread(argc,argv);
	grag_thread->startThread();

	sender.setup( theConfig.CLIENT, theConfig.PORT );
	printf("[OSC] setup client as %s : %d\n", theConfig.CLIENT.c_str(), theConfig.PORT);

	size = grag_thread->input._size;

	if (size.width < 400)
	{
		size = size*2;//enlarge the size of camera with small resolution
	}
	total = cvCreateImage(size, 8, 3);
	cvSet(total, CV_RGB(122,122,122));

	float k = 0;
	HalfWidth = size.width/2;
	HalfHeight = size.height/2;
	half = cv::Size(HalfWidth-k, HalfHeight-k);

	roi[0] = cv::Rect(Point(k,k), half);
	roi[1] = cv::Rect(Point(k+HalfWidth,k), half);
	roi[2] = cv::Rect(Point(k,k+HalfHeight), half);
	roi[3] = cv::Rect(Point(k+HalfWidth,k+HalfHeight), half);	

	dstQuad[0] = Point(0,0);
	dstQuad[1] = Point(HalfWidth,0);
	dstQuad[2] = Point(HalfWidth,HalfHeight);
	dstQuad[3] = Point(0,HalfHeight);

	if (!theConfig.load_from("config.xml"))
	{
		theConfig.corners[0] = cv::Point2f(0,0);
		theConfig.corners[1] = cv::Point2f(HalfWidth,0);
		theConfig.corners[3] = cv::Point2f(0,HalfHeight);
		theConfig.corners[2] = cv::Point2f(HalfWidth,HalfHeight);
	}
	vGetPerspectiveMatrix(warp_matrix, theConfig.corners, dstQuad);

	onParamFlip(theConfig.paramFlipX, theConfig.paramFlipY);

	theConfig.auto_explosure = _input.getAutoExplosure();
	/*if (face_track)*/
	haar.init("../../data/haarcascade_frontalface_alt.xml");

	half_raw = cvCreateImage(half, 8, _input._channel);
	half_flip = cvCreateImage(half, 8, _input._channel);
	grayBlob = cvCreateImage(half, 8, 1);
	frame = cvCreateImage(half, 8, _input._channel);
	black_frame = cvCreateImage(half, 8, _input._channel);
	white_frame = cvCreateImage(half, 8, _input._channel);
	grayBuffer = cvCreateImage(half, 8, 1);
	prevBg = cvCreateImage(half, 8, _input._channel);

	cvSet(black_frame, CV_BLACK);
	cvSet(white_frame, CV_WHITE);

	if (theConfig.delay_for_run > 0)
		SLEEP(theConfig.delay_for_run * 1000);

	if (_input._InputType == _input.From_Camera)
		_input.wait(5);

	monitor_gui::show(true);
	param_gui::show(true);
	param_gui::init();

	onRefreshBack();

	return true;
}

void VideoApp::onParamFlip(int fx, int fy)
{ 
	g_Fx = fx;
	g_Fy = fy;

	if (using_black_bg || using_white_bg)
	{

	}
	else
		if (prevBg.empty())//if not initialized
			onRefreshBack();
		else
		{
/*			to_reset_back = false;		*/
			Ptr<IplImage> temp = cvCloneImage(prevBg);
			vFlip(temp, abs(fx-g_prevFx), abs(fy-g_prevFy));
			backModel->init(temp);
		}
}

void VideoApp::onParamAuto(int v)
{	
	paramMoG.win_size = (v+1)*50; //200;
	paramMoG.n_gauss = 3; //5;
	paramMoG.bg_threshold = 0.3; //0.7;
	paramMoG.std_threshold = 2; //2.5;
	paramMoG.minArea = 100.f; //15.f;
	paramMoG.weight_init = 0.01; //0.05;
	paramMoG.variance_init = 30; //30*30;

	backModel.release();	
	backModel = new vBackGaussian();
	//backModel = new vBackFGDStat();
	onRefreshBack();
}


void VideoApp::send_osc_msg()
{
	UINT nBlobs = blobTracker.trackedBlobs.size(); 

	const float _W = HalfWidth;
	const float _H = HalfHeight;

#define addFloatX(num) m.addFloatArg(num/_W);
#define addFloatY(num) m.addFloatArg(num/_H);
	//start
	{
		ofxOscMessage m;
		m.setAddress( "/start" );
		sender.sendMessage( m );
	}

	//face
	if (theConfig.face_track)
	{
		int nFaces = haar.blobs.size();
		for (int i=0;i<nFaces;i++)
		{
			vBlob& obj = haar.blobs[i];
			ofxOscMessage m;
			m.setAddress( "/face");
			m.addIntArg(-1);
			m.addStringArg("");
			addFloatX(obj.center.x);
			addFloatY(obj.center.y);
			addFloatX(obj.box.x);
			addFloatY(obj.box.y);
			addFloatX(obj.box.width);
			addFloatY(obj.box.height);
			sender.sendMessage( m );

			vDrawRect(frame, obj.box, vDefaultColor(i));
		}
	}

	//object
	vector<vTrackedBlob>* pTrackBlobs[] = {&blobTracker.trackedBlobs, &blobTracker.leaveBlobs};
	for (int v=0;v<2;v++)
	{
		vector<vTrackedBlob>* pTb = pTrackBlobs[v];
		int nB = pTb->size();
		for (UINT i=0;i<nB;i++)
		{
			vTrackedBlob& obj = (*pTb)[i];

			//if (obj.status == statusStill)
			//	//do send still blobs out
			//	continue;

			if (obj.isHole)
				vPolyLine(frame, obj.pts, cv::Scalar(0,0,0), 1);
			else
				vPolyLine(frame, obj.pts, vDefaultColor(obj.id), 1);
			//	vDrawRect(frame, obj.box, CV_RGB(0,122,255));
			sprintf(g_buffer, "#%d", obj.id);
			vDrawText(frame, obj.center.x, obj.center.y, g_buffer, vDefaultColor(obj.id));

			int id = obj.id;
			int cx = obj.center.x;
			int cy = obj.center.y;
			int x = obj.box.x;
			int y = obj.box.y;
			int w = obj.box.width;
			int h = obj.box.height;


			{
				ofxOscMessage m;
				if (!theConfig.detailed_mode)
				{
					m.setAddress( "/blob");
				}
				else
				{
					if (obj.isHole)
						m.setAddress( "/hole");
					else
						m.setAddress( "/contour");
				}

				m.addIntArg(id);
				m.addStringArg(obj.getStatusString());
				addFloatX(cx);
				addFloatY(cy);
				addFloatX(x);
				addFloatY(y);
				addFloatX(w);
				addFloatY(h);
				if (theConfig.detailed_mode)
				{
					int nPts = obj.pts.size();
					m.addIntArg(nPts);
					for (int k=0;k<nPts;k++)
					{
						addFloatX(obj.pts[k].x);
						addFloatY(obj.pts[k].y);
					}
				}
				sender.sendMessage( m );
			}
		}
	}
	{
		ofxOscMessage m;
		m.setAddress( "/end" );
		sender.sendMessage( m );
	}	
}

void VideoApp::onRefreshBack()
{
	to_reset_back = true;
}
