#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "MiniTimer.h"

//#define DISABLE_FACE_DETECTION

VideoApp theApp;//global

#ifdef WIN32
int CamServer_WindowCallback(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lparam, int* was_processed)
{
	if (uMsg == WM_DESTROY)
	{
		if ((hwnd == (HWND)monitor_gui::handle && theApp.monitorVisible) || hwnd == (HWND)param_gui::handle)
			theApp.app_running = false;
	}
	return 0;
}
#endif

VideoApp::VideoGrabThread::VideoGrabThread(VideoInput& input):_input(input)
{
	fps = 0;
}

bool VideoApp::VideoGrabThread::is_dirty()
{
	bool ret = _dirty;
	_dirty = false;
	return ret;
}

void VideoApp::VideoGrabThread::threadedFunction()
{
	MiniTimer timer;
	int frame_counter = 0;
	int ms_counter = 0;	
	while(true)
	{
		timer.resetStartTime();
		if (!theApp.app_running)
			return;
		_input.get_frame();
		_dirty = true; 
		if (_input._InputType == _input.From_Video)
		{
			unsigned int elapse = timer.getTimeElapsedMS();
			if (elapse < 40)
				SLEEP(40 - elapse);
		}

		frame_counter++;
		if ((ms_counter += timer.getTimeElapsedMS()) > 1000)
		{
			fps = frame_counter;
			frame_counter = 0;
			ms_counter = 0;
		}

		timer.profileFunction("<<thread>>input.get_frame()");
	}
} 

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

	app_running = true;
	input_inited = false;

	g_Fx = g_Fy = g_prevFx = g_prevFy = 0;
	channels = 3;
}

bool VideoApp::init(int argc, char** argv)
{
	if (theConfig.minim_window)
	{
#ifdef WIN32
		keybd_event(VK_LWIN, 0, 0, 0);
		keybd_event('D', 0, 0, 0);
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
#endif
	}

	if (!input.init(argc,argv))
	{
		input_inited = true;
		system("pause");
		return false;
	}
#ifdef MODE_320X240
	if (input._InputType == VideoInput::From_Camera)
	{
		if (input._frame->width > 320)
			input.resize(320, 240);
	}
#endif
	input_inited = true;
	grab_thread = new VideoGrabThread(input);
#ifdef _DEBUG
	grab_thread->startThread(true, true);//debug mode is noisy
#else
	grab_thread->startThread(true, false);
#endif

	//grab unrelated
	sender.setup( theConfig.CLIENT, theConfig.PORT );
	printf("[OSC] setup client as %s : %d\n", theConfig.CLIENT.c_str(), theConfig.PORT);

	//theConfig.auto_explosure = _input.getAutoExplosure();
	/*if (face_track)*/
#ifndef DISABLE_FACE_DETECTION
	if (!haar.init("../../data/haarcascade_frontalface_alt.xml"))
	{//try another folder
		haar.init("../data/haarcascade_frontalface_alt.xml");
	}
	haar.scale = 2;
#endif
	//grab related
	grab_thread->lock();//wait for VideoInput::init() returns
	size = input._size;
	channels = input._channel;

	if (size.width < 400)
	{
		size = size*2;//enlarge the size of camera with small resolution
	}
	total = cvCreateImage(cv::Size(size.width*1.5, size.height*1.5), 8, 3);
	cvSet(total, CV_RGB(122,122,122));

	float k = 0;
	HalfWidth = size.width*0.75;
	HalfHeight = size.height*0.75;
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

	half_raw = cvCreateImage(half, 8, channels);
	half_flip = cvCreateImage(half, 8, channels);
	grayBlob = cvCreateImage(half, 8, 1);
	frame = cvCreateImage(half, 8, channels);
	black_frame = cvCreateImage(half, 8, channels);
	white_frame = cvCreateImage(half, 8, channels);
	kinect_frame = cvCreateImage(half, 8, 1);
	grayBuffer = cvCreateImage(half, 8, 1);
	prevBg = cvCreateImage(half, 8, channels);

	cvSet(black_frame, CV_BLACK);
	cvSet(white_frame, CV_WHITE);
	cvSet(kinect_frame, CV_RGB(150,150,150));

	if (theConfig.delay_for_run > 0)					// if necessary
		SLEEP(theConfig.delay_for_run * 1000);	// sleep before the app shows up

	monitor_gui::show(true);
	param_gui::show(true);
	param_gui::init();

	onRefreshBack();

#ifdef WIN32
	cvSetPreprocessFuncWin32(CamServer_WindowCallback);
#endif
	return true;
}

void VideoApp::onParamFlip(int fx, int fy)
{ 
	g_Fx = fx;
	g_Fy = fy;

	if (theConfig.bg_mode == REAL_BG)
	{
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

#define addFloatX(num) m.addFloatArg(num/_W)
#define addFloatY(num) m.addFloatArg(num/_H)

void VideoApp::send_custom_msg()
{
	const float _W = HalfWidth;
	const float _H = HalfHeight;

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
		for (unsigned int i=0;i<nB;i++)
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
#if 0
			int cx = obj.center.x;
			int cy = obj.center.y;
			int x = obj.box.x;
			int y = obj.box.y;
			int w = obj.box.width;
			int h = obj.box.height;
#else
			int cx = obj.rotBox.center.x;
			int cy = obj.rotBox.center.y;
			int w = obj.rotBox.size.width;
			int h = obj.rotBox.size.height;
			float angle = obj.angle;
#endif
			{
				ofxOscMessage m;
				if (obj.isHole)
					m.setAddress( "/hole");
				else
					m.setAddress( "/contour");

				m.addIntArg(id);                        //0
				m.addStringArg(obj.getStatusString());  //1
				addFloatX(cx);                          //2
				addFloatY(cy);                          //3
				addFloatX(w);                           //4
				addFloatY(h);                           //5
				m.addFloatArg(angle);                   //6
				int nPts = obj.pts.size();
				m.addIntArg(nPts);                      //7
				for (int k=0;k<nPts;k++)
				{
					addFloatX(obj.pts[k].x);
					addFloatY(obj.pts[k].y);
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

void VideoApp::send_tuio_msg()
{
	const float _W = HalfWidth;
	const float _H = HalfHeight;

	ofxOscBundle bundle;

	ofxOscMessage alive;
	{
		alive.setAddress("/tuio/2Dcur");
		alive.addStringArg("alive");
	}

	ofxOscMessage fseq;
	{
		fseq.setAddress( "/tuio/2Dcur" );
		fseq.addStringArg( "fseq" );
		fseq.addIntArg(input._frame_num);
	}

	int n_blobs = blobTracker.trackedBlobs.size();
	for(int i=0;i < n_blobs; i++)
	{
		vTrackedBlob& blob = blobTracker.trackedBlobs[i];

		if (blob.isHole)
			vPolyLine(frame, blob.pts, cv::Scalar(0,0,0), 1);
		else
			vPolyLine(frame, blob.pts, vDefaultColor(blob.id), 1);
		//	vDrawRect(frame, obj.box, CV_RGB(0,122,255));
		sprintf(g_buffer, "#%d", blob.id);
		vDrawText(frame, blob.center.x, blob.center.y, g_buffer, vDefaultColor(blob.id));

		//Set Message
		ofxOscMessage m;
		m.setAddress( "/tuio/2Dcur" );
		m.addStringArg("set");
		m.addIntArg(blob.id);				// id
		addFloatX(blob.center.x);	// x
		addFloatX(blob.center.y);	// y
		addFloatX(blob.velocity.x);			// dX
		addFloatX(blob.velocity.y);			// dY
		m.addFloatArg(0);		// maccel
		bundle.addMessage(m);
		alive.addIntArg(blob.id);				// add blob to list of ALL active IDs
	}

	bundle.addMessage(alive);
	bundle.addMessage(fseq);
	sender.sendBundle(bundle);
}

void VideoApp::onRefreshBack()
{
	to_reset_back = true;
}
