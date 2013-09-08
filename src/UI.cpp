#include "../_common/OpenCV/cvButtons.h"
#include "UI.h"
#include "AppConfig.h"
#include "VideoApp.h"

using namespace cv;

struct ButtonInfo
{
	int x;
	int y;
	char *text;
	void (*cb)(int);
	int*	value;
};

namespace monitor_gui
{
	CvButtons buttons;

	void onMonitorMouse(int Event,int x,int y,int flags,void* param )
	{
        // TODO: why 1000?
		if (x < 0 || x > 1000)
			x = 0;
		else if (x > theApp.HalfWidth ) x = theApp.HalfWidth;
		if (y < 0 || y > 1000) y = 0;
		else if (y > theApp.HalfHeight ) y = theApp.HalfHeight;

        if(theConfig.fixed_back_mode &&  Event == EVENT_RBUTTONUP)
		{
			theApp.onRefreshBack();
			param_gui::on_realbg();			
		}
		else
			if( Event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON) )
				theApp.selectedCorner = NULL;
			else	if( Event == EVENT_LBUTTONDOWN )
			{
				theApp.selectedCorner = NULL;

                //todo
				for (int i=0;i<4;i++)
				{
					cv::Point2f* pt = &theConfig.corners[i];
                    const int kThreshold = 15;
					if ( abs(pt->x - x) < kThreshold && abs(pt->y - y) < kThreshold)
					{
						theApp.selectedCorner = pt;
						break;
					}
				}
			}
			else if( Event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON) )
			{
				if (theApp.selectedCorner)
				{
					theApp.selectedCorner->x = x;
					theApp.selectedCorner->y = y;
                    theApp.warp_matrix = getPerspectiveTransform(theConfig.corners, theApp.dstQuad);
				}
			}
	}

	void* handle = NULL;

    void OpenGlDrawCallback(void* userdata)
    {
        theApp.renderMainWindow();
    }

	void show(bool visible)
	{
		if (visible)
		{
			namedWindow(MAIN_WINDOW, WINDOW_OPENGL);
			resizeWindow(MAIN_WINDOW, theApp.total.cols, theApp.total.rows);
            theApp.setupOpenglResources();
			setMouseCallback(MAIN_WINDOW, onMonitorMouse);
            setOpenGlDrawCallback(MAIN_WINDOW, OpenGlDrawCallback);
			handle = cvGetWindowHandle(MAIN_WINDOW);
		}
		else
		{
			destroyWindow(MAIN_WINDOW);
		}
	}
}

namespace param_gui
{
	Mat setting;
	bool is_changing_layout = false;

	CvButtons buttons;
	const int _w = 55;//btn width
	const int dw = _w+10;//btn+space
	int w = 20;
	const int _h = 25;
	const int dh = _h+10;//btn+space
	int h = 35;

	int minus_one = -1;

	void* handle = NULL;

	void on_x(int t)
	{
		theConfig.paramFlipX = t;
		theApp.onParamFlip(theConfig.paramFlipX, theConfig.paramFlipY);
	}

	void on_y(int t)
	{
		theConfig.paramFlipY = t;
		theApp.onParamFlip(theConfig.paramFlipX, theConfig.paramFlipY);
	}

	void on_hull(int t)
	{
		theConfig.hull_mode = t;
	}

	void on_tuio(int t)
	{
		theConfig.tuio_mode = t;
	}

	void on_face(int t)
	{
		theConfig.face_track = t;
	}

	void on_dialog(int )
	{
		theApp.input.showSettingsDialog();
	}

	void on_mode(int t)
	{
		theConfig.gray_detect_mode = t;
		theApp.backModel.release();
		if (t)
			theApp.backModel = new vBackGrayDiff();
		else
			theApp.backModel = new vBackColorDiff();

		if (theApp.prevBg.empty())//if not initialized
			theApp.onRefreshBack();//let main loop do it
		else
		{
			theApp.to_reset_back = false;
			theApp.backModel->init(theApp.prevBg);
		}
	}

	void on_realbg(int t)
	{
		if (theConfig.bg_mode != REAL_BG)
		{
			theConfig.bg_mode = REAL_BG;
			show(true);			
		}
		theApp.onRefreshBack();		
	}

	void on_whitebg(int t)
	{
		if (theConfig.bg_mode != WHITE_BG)
		{
			theConfig.bg_mode = WHITE_BG;
			show(true);			
		}
		theApp.onRefreshBack();
	}

	void on_diffbg(int t)
	{
		if (theConfig.bg_mode != DIFF_BG)
		{
			theConfig.bg_mode = DIFF_BG;
			show(true);
		}
		
		theApp.onRefreshBack();
	}

	void on_blackbg(int t)
	{
		if (theConfig.bg_mode != BLACK_BG)
		{
			theConfig.bg_mode = BLACK_BG;
			show(true);
		}		
		theApp.onRefreshBack();
	}

	ButtonInfo btn_infs[]=
	{
		{ w=13, h=35,  "- X -", on_x, &theConfig.paramFlipX},
		{ w+=dw, h,  "- Y -", on_y, &theConfig.paramFlipY},
		{ w+=dw, h,  "tuio", on_tuio, &theConfig.tuio_mode},
#ifdef FACE_DETECTION_ENABLED
		{ w+=dw, h,  "face", on_face, &theConfig.face_track},
#endif
		{ w+=dw, h,  "hull", on_hull, &theConfig.hull_mode},
//		{ w+=dw, h,  "gray", on_mode, &theConfig.gray_detect_mode},
#ifdef VIDEO_INPUT_LIB_USED
		{ w+=dw, h,  "dialog", on_dialog, &minus_one},
#endif // VIDEO_INPUT_LIB_USED
		{ w=13, h+=dh,  "now", on_realbg, NULL},
		{ w+=dw, h,  "white", on_whitebg, NULL},
		{ w+=dw, h,  "black", on_blackbg, NULL},
		{ w+=dw, h,  "diff", on_diffbg, NULL},
	};
	
	int num_btns = sizeof(btn_infs)/sizeof(btn_infs[0]);

	void update()
	{
		setting = CV_GRAY;
		buttons.paintButtons(setting);
		rectangle(setting, Point(13,100), Point(387,104), Scalar(122,10,10), FILLED);

		const int x0 = 13;
		const int y0 = 70;

		int idx = theConfig.bg_mode;
		rectangle(setting, Point(x0+dw*idx,y0), Point(x0+dw*idx+_w,y0+_h), Scalar(10,10,122), 3);
	}

	void init()
	{
		setting.create(Size(400,120), CV_8UC3);
		buttons.release();

		for (int i=0;i<num_btns;i++)
		{
			ButtonInfo& inf = btn_infs[i];
			int btn_value = inf.value ? *inf.value : -1;
			buttons.addButton(PushButton( inf.x, inf.y, _w, _h, btn_value, inf.text, inf.cb ));
		}
		update();
	}

	void onParamMouse(int Event,int x,int y,int flags,void* param )
	{
		cvButtonsOnMouse(Event,x,y,flags, param);
		update();
	}

	void show(bool visible)
	{
		if (visible)
		{
			is_changing_layout = true;
			destroyWindow(PARAM_WINDOW);
			is_changing_layout = false;
			namedWindow(PARAM_WINDOW);
			resizeWindow(PARAM_WINDOW,400,480);

			handle = cvGetWindowHandle(PARAM_WINDOW);

			setMouseCallback( PARAM_WINDOW, onParamMouse, &buttons);

			if (theConfig.fixed_back_mode)
			{
				param_gui::on_mode(theConfig.gray_detect_mode);
				//		backModel = new vBackGrayDiff();
				//	backModel = new vBackColorDiff();
				//	backModel = new vThreeFrameDiff();
				if (theConfig.bg_mode == WHITE_BG)
					createTrackbar("Darkness",PARAM_WINDOW,&theConfig.paramDark,PARAM_DARK);
				else if (theConfig.bg_mode == BLACK_BG)
					createTrackbar("Brightness",PARAM_WINDOW,&theConfig.paramBright,PARAM_BRIGHT);
				else
				{
					createTrackbar("Below",PARAM_WINDOW,&theConfig.paramDark,PARAM_DARK);
					createTrackbar("Above",PARAM_WINDOW,&theConfig.paramBright,PARAM_BRIGHT);
				}
			}
			else
			{
				assert(0 && "[TODO] auto background");
				// 			cvCreateTrackbar("Auto",PARAM_WINDOW,&theConfig.paramAuto,PARAM_AUTO, onParamAuto);
				// 			onParamAuto(paramAuto);
			}
			createTrackbar("Blur",PARAM_WINDOW,&theConfig.paramBlur1,PARAM_BLUR1);
			createTrackbar("Noise",PARAM_WINDOW,&theConfig.paramBlur2,PARAM_BLUR2);
			//	cvCreateTrackbar("形态",PARAM_WINDOW,&paramNoise,PARAM_NOISE, NULL);

			createTrackbar("MinArea",PARAM_WINDOW,&theConfig.paramMinArea, PARAM_MAXAREA);
			createTrackbar("MaxArea",PARAM_WINDOW,&theConfig.paramMaxArea,PARAM_MAXAREA);
		}
		else
		{
			destroyWindow(PARAM_WINDOW);
		}
		update();
	}
}
