#include "OpenCV/OpenCV.h" 
#include "OpenCV/cvButtons.h"
#include "ofxOsc/ofxOsc.h"
#include "MiniTimer.h"
#include "ofxArgs.h"

bool using_debug_file = true;
#define HOST "localhost"
int PORT = 3333;

//cvPointPolygonTest
bool fixed_back_mode = true;//固定背景的模式 
bool mute_mode = false;//是否显示命令行界面
bool fixed_fps = false;//是否限制帧数
bool total_gui = true;//是否显示所有图像
bool contour_mode = true;//是否发送轮廓点 
bool face_track = false;//是否人脸识别
bool finger_track = true;//是否手指识别
int hull_mode = 0;//

int fps = 120;

cv::Point2f corners[4];
cv::Point2f* selected = NULL;
cv::Point2f dstQuad[4];
vector<vBlob> blobs;
cv::Rect roi[4]; 
CvMat* warp_matrix = NULL;


#define PARAM_DARK 255
#define PARAM_BRIGHT 255
#define PARAM_AUTO 40
#define PARAM_BLUR1 80
#define PARAM_BLUR2 7
#define PARAM_NOISE 3
#define PARAM_MINAREA 1000

int paramFlipX = 0;
int paramFlipY = 0;
int paramDark = 220;
int paramBright = 200;
int  paramAuto = 8;
int paramBlur1 = 35;
int paramBlur2 = 1;
int paramNoise = 0;
int paramMinArea = 2;

Ptr<IplImage> total;
Ptr<IplImage> frame;
Ptr<IBackGround> backModel;
int W, H;
bool to_reset_back = false;

char g_buffer[256];

int flip_mode = 1;//horizontal (flip=0),   vertical (flip=1) or both(flip=-1)
/*
flip_param -> flip_mode
0 -> null
1 -> 0
2 -> 1
3 -> -1
*/
void onParamFlip(int fx, int fy)
{	
	int flip = fx*2 + fy;
	switch(flip)
	{
	case 0:
		flip_mode = 1000;break;//NO FLIP
	case 1:
		flip_mode = 0;break;
	case 2:
		flip_mode = 1;break;
	case 3:
		flip_mode = -1;break;
	default:break;
	}
	to_reset_back = true;
}

CvFGDStatModelParams paramFGD = cvFGDStatModelParams();
CvGaussBGStatModelParams paramMoG;

void onParamAuto(int v)
{	
	paramMoG.win_size = (v+1)*50; //200;
	paramMoG.n_gauss = 3; //5;
	paramMoG.bg_threshold = 0.3; //0.7;
	paramMoG.std_threshold = 2; //2.5;
	paramMoG.minArea = 100.f; //15.f;
	paramMoG.weight_init = 0.01; //0.05;
	paramMoG.variance_init = 30; //30*30;
	
	paramFGD.minArea = 5 + v;
	backModel.release();	
	backModel = new vBackGaussian();
	//backModel = new vBackFGDStat();
	to_reset_back = true;
}

void onMouse(int Event,int x,int y,int flags,void* param )
{ 
	if (x < 0 || x > 1000) 
		x = 0;
	else if (x > W ) x = W;
	if (y < 0 || y > 1000) y = 0;
	else if (y > H ) y = H;

	if(fixed_back_mode &&  Event == CV_EVENT_RBUTTONUP)
	{//固定背景模式下，按右键保存背景
		to_reset_back = true;
	}
	else
	if( Event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
        selected = NULL;
    else	if( Event == CV_EVENT_LBUTTONDOWN )
	{
		selected = NULL;

		for (int i=0;i<4;i++)
		{
			cv::Point2f* pt = &corners[i];
			if ( abs(pt->x - x) < 15 && abs(pt->y - y) < 15)
			{
				selected = pt;
				break;
			}
		}
	}
    else if( Event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
    {
		if (selected)
		{
			selected->x = x;
			selected->y = y;
			vGetPerspectiveMatrix(warp_matrix, corners, dstQuad);
		} 
    }
}


void save_config();

namespace the_gui
{
	CvButtons buttons;
	const int _w = 40;
	const int _h = 25;
	const int _dw = _w+20;
	int w = 20;
	Ptr<IplImage> setting;

	void on_x(int t)
	{
		paramFlipX = t;
		onParamFlip(paramFlipX, paramFlipY);
	}

	void on_y(int t)
	{
		paramFlipY = t;
		onParamFlip(paramFlipX, paramFlipY);
	}

	void on_hull(int t)
	{
		hull_mode = t;
	}

	void on_contour(int t)
	{
		contour_mode = t;
	}

	//void on_save(int t)
	//{
	//	save_config();
	//}

	struct btn_inf
	{
		int x;
		int y;
		int t;
		char *text;
		void (*cb)(int);
	}btn_infs[]=
	{
		{ w+=_dw, 35, 0, " X", on_x},
		{ w+=_dw, 35, 0, " Y", on_y},
		{ w+=_dw, 35, 0, "cont", on_contour},
	//	{ w+=_dw, 35, -1, "save", on_save},
		{ w+=_dw, 35, 0, "hull", on_hull},
	};

	int num_btns = sizeof(btn_infs)/sizeof(btn_inf);

 	void update()
	{
		cvSet(setting, CV_RGB(122,122,122));
		buttons.paintButtons(setting);		
	}

	void init()
	{
		setting.release();
		setting = cvCreateImage(cvSize(400,120),8,3);
		buttons.release();
		for (int i=0;i<num_btns;i++)
		{
			btn_inf& inf = btn_infs[i];
			buttons.addButton(PushButton( inf.x, inf.y, _w, _h, inf.t, inf.text, inf.cb ));			
		}
		cvSetMouseCallback( "参数设置", cvButtonsOnMouse, &buttons); 
//		_beginthread(the_gui::update, 0, 0);
	}
}

void load_config();
void save_config();
void parse_args(int argc, char** argv);

HANDLE output = 0;
#define _write(str) if (output) WriteConsole(output,str,strlen(str),NULL,NULL);

//int main(int argc, char** argv )
int WINAPI WinMain(      
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{	
	int argc = __argc;
	char ** argv = __argv;

	parse_args(argc, argv);

	VideoInput video_input;

	ofxOscSender sender;
	sender.setup( HOST, PORT );
 
	cvNamedWindow("主窗口");
	cvSetMouseCallback("主窗口",onMouse);
	cvResizeWindow("主窗口",640,480);

	cvNamedWindow("参数设置");
	cvResizeWindow("参数设置",400,480);

	if (argc > 2)
		argc = 2;//hack the video_input.init

	if (video_input.init(argc,argv))
	{
		Ptr<IplImage> half_raw;
		Ptr<IplImage> half_flip;
		Ptr<IplImage> grayBlob;
		Ptr<IplImage> grayBuffer;
		Ptr<IplImage> fore;

		cv::Size size = video_input._size;
		int channel = video_input._channel;

		if (size.width < 400)
		{
			size = size*2;//让小分辨率的摄像头大一些
		}
		total = cvCreateImage(size, 8, 3);
		cvSet(total, CV_RGB(122,122,122));

		float k = 0;
		W = size.width/2;
		H = size.height/2;
		cv::Size half(W-k, H-k);

		roi[0] = cv::Rect(Point(k,k), half);
		roi[1] = cv::Rect(Point(k+W,k), half);
		roi[2] = cv::Rect(Point(k,k+H), half);
		roi[3] = cv::Rect(Point(k+W,k+H), half);	

		dstQuad[0] = Point(0,0);
		dstQuad[1] = Point(W,0);
		dstQuad[2] = Point(W,H);
		dstQuad[3] = Point(0,H);
		load_config();
		onParamFlip(paramFlipX, paramFlipY);

		the_gui::init();

		MiniTimer timer;
		MiniTimer timer2;
		
		vHaarDetector haar;
		if (face_track)
			haar.init("../media/haarcascade_frontalface_alt.xml");
		vector<CvRect> face_rects;

		vFingerDetector finger;

		while (true)
		{
			_write("\n");

			timer.resetStartTime();
			timer2.resetStartTime();

			IplImage* raw = video_input.get_frame();
			if (!raw)
				break;			

			timer.profileFunction("get_frame");

			if (half_raw == NULL)
			{
				half_raw = cvCreateImage(half, 8, channel);
				half_flip = cvCreateImage(half, 8, channel);
				grayBlob = cvCreateImage(half, 8, 1);
				frame = cvCreateImage(half, 8, channel);
				grayBuffer = cvCreateImage(half, 8, 1);
				fore = cvCreateImage(half, 8, 1);
				
				video_input.wait(10);
				raw = video_input.get_frame(); 

			//	cvCreateTrackbar("坐标反转","参数设置",&paramFlip,3, onParamFlip);

				if (fixed_back_mode)
				{
					backModel = new vBackGrayDiff();					
					cvCreateTrackbar("黑暗区域","参数设置",&paramDark,PARAM_DARK, NULL);
					cvCreateTrackbar("明亮区域","参数设置",&paramBright,PARAM_BRIGHT, NULL);
				}
				else
				{
					cvCreateTrackbar("自动","参数设置",&paramAuto,PARAM_AUTO, onParamAuto);
					onParamAuto(paramAuto);
				}

				to_reset_back = true;

				cvCreateTrackbar("模糊","参数设置",&paramBlur1,PARAM_BLUR1, NULL);
				cvCreateTrackbar("去噪","参数设置",&paramBlur2,PARAM_BLUR2, NULL);
			//	cvCreateTrackbar("形态","参数设置",&paramNoise,PARAM_NOISE, NULL);
				cvCreateTrackbar("最小面积","参数设置",&paramMinArea,PARAM_MINAREA, NULL);
			}

			int key = cvWaitKey(1);
			if (key == VK_ESCAPE)
				break; 
			if (key == VK_SPACE)
			{
				total_gui = !total_gui;
				if (total_gui)
				{
					cvNamedWindow("主窗口");
					cvSetMouseCallback("主窗口",onMouse);
					cvResizeWindow("主窗口",640,480);
				}
				else
				{
					cvDestroyWindow("主窗口");
				}
			}

			//raw->half_raw->frame
			cvResize(raw, half_raw);
			if (flip_mode < 2)
				cvFlip(half_raw, 0, flip_mode);
			timer.profileFunction("cvFlip");
			cvWarpPerspective(half_raw, frame, warp_matrix);

			timer.profileFunction("vPerspectiveTransform");

			if (to_reset_back)
			{
				to_reset_back = false;
				backModel->init(frame, (void*)&paramMoG);
			}

			if (face_track)
				haar.detect_object(frame, face_rects);

			IplImage* back = backModel->getBackground();

			//[dark mode]
			backModel->setThresh(PARAM_DARK - paramDark);//取个反值，逻辑上更通顺
			backModel->update(frame, DETECT_DARK);
			IplImage* fore1 = backModel->getForeground();
			cvCopyImage(fore1, fore);

			//[bright mode]
			backModel->setThresh(PARAM_BRIGHT - paramBright);
			backModel->update(frame, DETECT_BRIGHT);			
			IplImage* fore2 = backModel->getForeground();

			cvOr(fore, fore2, fore);
						
			timer.profileFunction("update");
			//fore->blobs
			vHighPass(fore, grayBuffer, paramBlur1, paramBlur2);		
			cvCopyImage(grayBuffer, fore);
			timer.profileFunction("vHighPass");

//			vOpen(grayBuffer, paramNoise);
			vFindBlobs(grayBuffer, blobs, paramMinArea, hull_mode);
			timer.profileFunction("vFindBlobs");

			UINT nBlobs = blobs.size();

			sprintf(g_buffer, "%d object\n", nBlobs);
			_write(g_buffer);
			{
				ofxOscMessage m;
				m.setAddress( "/start" );
				sender.sendMessage( m );
			}
			if (face_track)
			{
				for (int i=0;i<face_rects.size();i++)
				{
					CvRect& f = face_rects[i];
					ofxOscMessage m;
					m.setAddress( "/face");
					m.addIntArg(f.x);
					m.addIntArg(f.y);
					m.addIntArg(f.width);
					m.addIntArg(f.height);
					sender.sendMessage( m );

					vDrawRect(frame, f, vDefaultColor(i));
				}
			}

			if (finger_track)
			{
				for (UINT i=0;i<nBlobs;i++)
				{
					bool ffound=finger.findFingers(blobs[i], 12);
				//	bool hfound=finger.findHands(blobs[0]);
					if (ffound)
					{
						for (int f=0;f<finger.ppico.size();f++)
						{	
							cvCircle(frame, finger.ppico[f], 10, vRandomColor());
						}
					}
				}
			}

			for (UINT i=0;i<nBlobs;i++)
			{
				vBlob& obj = blobs[i];

				vPolyLine(frame, obj.pts, vDefaultColor(i),2);
			//	vDrawRect(frame, obj.box, CV_RGB(0,122,255));

				int cx = obj.center.x;
				int cy = obj.center.y;
				int x = obj.box.x;
				int y = obj.box.y;
				int w = obj.box.width;
				int h = obj.box.height;

				sprintf(g_buffer, "center<%d, %d> bbox<%d, %d, %d, %d>\n", 
					cx, cy, 
					x, y, w, h);
				_write(g_buffer);
				{
					ofxOscMessage m;
					m.setAddress( "/object");
					m.addIntArg(cx);
					m.addIntArg(cy);
					m.addIntArg(x);
					m.addIntArg(y);
					m.addIntArg(w);
					m.addIntArg(h);
					sender.sendMessage( m );
				}
				if (contour_mode)
				{
					ofxOscMessage m;
					if (obj.isHole)
						m.setAddress( "/hole");
					else
						m.setAddress( "/contour");
					int n = obj.pts.size();
					m.addIntArg(n);
					for (int k=0;k<n;k++)
					{
						m.addIntArg(obj.pts[k].x);
						m.addIntArg(obj.pts[k].y);
					}
					sender.sendMessage( m );
				}
			}
			{
				ofxOscMessage m;
				m.setAddress( "/end" );
				sender.sendMessage( m );
			}

			timer.profileFunction("msg");

			if (total_gui)
			{
				vCopyImageTo(half_raw, total, roi[0]);
				vCopyImageTo(frame, total, roi[1]);
				vCopyImageTo(fore, total, roi[2]);
				vCopyImageTo(back, total, roi[3]);

				timer.profileFunction("copy");

				for (int i=0;i<4;i++)
				{
					cvLine(total, corners[i], corners[(i+1)%4], CV_RGB(255,0,0));
					cvRectangle(total, corners[i] - Point2f(3,3), corners[i] + Point2f(3,3), CV_RGB(255,0,0), CV_FILLED);
				}
				if (selected)
					cvRectangle(total, *selected - Point2f(3,3), *selected + Point2f(3,3), CV_RGB(0,0,255), CV_FILLED);

				//show_image(total);
				cvShowImage("主窗口",total);
			}

			the_gui::update(); 
			timer.profileFunction("update");
	//		HWND wnd = (HWND)cvGetWindowHandle("主窗口");
	//		RECT rc;
	//		::GetWindowRect(wnd, &rc);
	//		cvMoveWindow("参数设置", rc.right+2, rc.top-22);

			if (fixed_fps)
			{
				DWORD elapse = timer2.getTimeElapsedMS();
				if (elapse < fps)
					::Sleep(fps - elapse);
			}

			timer.profileFunction("final"); 
			if (face_track)
				sprintf(g_buffer, "cost: %d ms, %d object  %d face", timer2.getTimeElapsedMS(), blobs.size(), face_rects.size());
			else
				sprintf(g_buffer, "cost: %d ms, %d object", timer2.getTimeElapsedMS(), blobs.size());
			vDrawText(the_gui::setting, 20,20, g_buffer);
			cvShowImage("参数设置", the_gui::setting);	
			timer2.profileFunction("total"); 
		}
		save_config();
	} 

	return 0;
}


void load_config()
{
	FILE* config = fopen("config.ini","r");
	if (config == NULL)
	{
		corners[0] = cv::Point2f(0,0);
		corners[1] = cv::Point2f(W,0);
		corners[3] = cv::Point2f(0,H);
		corners[2] = cv::Point2f(W,H); 
	}
	else
	{
		float x,y; 
		for (int i=0;i<4;i++)
		{
			fscanf(config, "%f,%f", &x, &y);
			corners[i] = cvPoint(x*W,y*H);
		}
		fscanf(config, "%d", &paramFlipX);
		fscanf(config, "%d", &paramFlipY);
		fscanf(config, "%d", &paramDark);
		fscanf(config, "%d", &paramBright);
//		fscanf(config, "%d", &paramAuto);
		fscanf(config, "%d", &paramBlur1);
		fscanf(config, "%d", &paramBlur2);
//		fscanf(config, "%d", &paramNoise);
		fscanf(config, "%d", &paramMinArea);
		fscanf(config, "%d", &hull_mode);
		printf("坐标信息已读取\n");
	}  
	vGetPerspectiveMatrix(warp_matrix, corners, dstQuad);
}
 

void save_config()
{
	FILE* config = fopen("config.ini", "w");
	if (config)
	{
		for (int i=0;i<4;i++)
		{
			fprintf(config, "%.1f, %.1f\n", corners[i].x/(float)W, corners[i].y/(float)H);
		}
		fprintf(config, "%d\n", paramFlipX);
		fprintf(config, "%d\n", paramFlipY);
		fprintf(config, "%d\n", paramDark);
		fprintf(config, "%d\n", paramBright);
	//	fprintf(config, "%d\n", paramAuto);
		fprintf(config, "%d\n", paramBlur1);
		fprintf(config, "%d\n", paramBlur2);
//		fprintf(config, "%d\n", paramNoise);
		fprintf(config, "%d\n", paramMinArea);
		fprintf(config, "%d\n", hull_mode);
	}
	printf("坐标信息已保存\n");
}

void parse_args(int argc, char** argv)
{
	ofxArgs args(argc, argv);
	//if (args.contains("-a"))
	//	fixed_back_mode = false;
	//else
	//	fixed_back_mode = true;
	if (args.contains("-dos"))
	{
		AllocConsole();
		output = GetStdHandle(STD_OUTPUT_HANDLE);		
	}

	contour_mode = args.contains("-contour"); 

	using_debug_file = args.contains("-log");

	if (args.contains("-fps"))
	{
		fixed_fps = true;
		fps = args.getInt("-fps");
	}

	face_track = args.contains("-face");

	if (args.contains("-p"))
		PORT = args.getInt("-p");
}