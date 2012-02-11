#include "../../_common/vOpenCV/cvButtons.h"
#include "UI.h"
#include "AppConfig.h"
#include "VideoApp.h"

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
		if (x < 0 || x > 1000)
			x = 0;
		else if (x > theApp.HalfWidth ) x = theApp.HalfWidth;
		if (y < 0 || y > 1000) y = 0;
		else if (y > theApp.HalfHeight ) y = theApp.HalfHeight;

		if(theConfig.fixed_back_mode &&  Event == CV_EVENT_RBUTTONUP)
		{
			theApp.onRefreshBack();
			theConfig.bg_mode = REAL_BG;
			param_gui::update();
		}
		else
			if( Event == CV_EVENT_LBUTTONUP || !(flags & CV_EVENT_FLAG_LBUTTON) )
				theApp.selected = NULL;
			else	if( Event == CV_EVENT_LBUTTONDOWN )
			{
				theApp.selected = NULL;

				for (int i=0;i<4;i++)
				{
					cv::Point2f* pt = &theConfig.corners[i];
					if ( abs(pt->x - x) < 15 && abs(pt->y - y) < 15)
					{
						theApp.selected = pt;
						break;
					}
				}
			}
			else if( Event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON) )
			{
				if (theApp.selected)
				{
					theApp.selected->x = x;
					theApp.selected->y = y;
					vGetPerspectiveMatrix(theApp.warp_matrix, theConfig.corners, theApp.dstQuad);
				}
			}
	}

	void* handle = NULL;

	void show(bool visible)
	{
		if (visible)
		{
			cvNamedWindow(MAIN_WINDOW);
			cvResizeWindow(MAIN_WINDOW,640,480);

			cvSetMouseCallback(MAIN_WINDOW,onMonitorMouse);
			handle = cvGetWindowHandle(MAIN_WINDOW);
		}
		else
		{
			cvDestroyWindow(MAIN_WINDOW);
		}
	}
}

namespace param_gui
{
	cv::Ptr<IplImage> setting;
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

	void on_kinectbg(int t)
	{
		if (theConfig.bg_mode != KINECT_BG)
		{
			show(true);
			theConfig.bg_mode = KINECT_BG;
		}
		theApp.onRefreshBack();
	}
	ButtonInfo btn_infs[]=
	{
		{ w=13, h=35,  "- X -", on_x, &theConfig.paramFlipX},
		{ w+=dw, h,  "- Y -", on_y, &theConfig.paramFlipY},
		{ w+=dw, h,  "tuio", on_tuio, &theConfig.tuio_mode},
		{ w+=dw, h,  "face", on_face, &theConfig.face_track},
		{ w+=dw, h,  "hull", on_hull, &theConfig.hull_mode},
//		{ w+=dw, h,  "gray", on_mode, &theConfig.gray_detect_mode},
//		{ w=13, h+=dh,  "expo", on_expo, &theConfig.auto_explosure},
#ifdef WIN32
		{ w+=dw, h,  "dialog", on_dialog, &minus_one},
#endif // WIN32
		{ w=13, h+=dh,  "now", on_realbg, NULL},
		{ w+=dw, h,  "white", on_whitebg, NULL},
		{ w+=dw, h,  "black", on_blackbg, NULL},
		{ w+=dw, h,  "diff", on_diffbg, NULL},
#ifdef KINECT
		{ w+=dw, h,  "kinect", on_kinectbg, NULL},
#endif
	};
	
	int num_btns = sizeof(btn_infs)/sizeof(btn_infs[0]);

	void update()
	{
		cvSet(setting, CV_RGB(122,122,122));
		buttons.paintButtons(setting);
		cvRectangle(setting, cvPoint(13,100), cvPoint(387,104), CV_RGB(122,10,10), CV_FILLED);

		const int x0 = 13;
		const int y0 = 70;

		int idx = theConfig.bg_mode;
		cvRectangle(setting, cvPoint(x0+dw*idx,y0), cvPoint(x0+dw*idx+_w,y0+_h), CV_RGB(10,10,122), 3);
	}

	void init()
	{
		setting.release();
		setting = cvCreateImage(cvSize(400,120),8,3);
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
			cvDestroyWindow(PARAM_WINDOW);
			is_changing_layout = false;
			cvNamedWindow(PARAM_WINDOW);
			cvResizeWindow(PARAM_WINDOW,400,480);

			handle = cvGetWindowHandle(PARAM_WINDOW);

			cvSetMouseCallback( PARAM_WINDOW, onParamMouse, &buttons);

			if (theConfig.fixed_back_mode)
			{
				param_gui::on_mode(theConfig.gray_detect_mode);
				//		backModel = new vBackGrayDiff();
				//	backModel = new vBackColorDiff();
				//	backModel = new vThreeFrameDiff();
				if (theConfig.bg_mode == WHITE_BG)
					cvCreateTrackbar("Darkness",PARAM_WINDOW,&theConfig.paramDark,PARAM_DARK, NULL);
				else if (theConfig.bg_mode == BLACK_BG)
					cvCreateTrackbar("Brightness",PARAM_WINDOW,&theConfig.paramBright,PARAM_BRIGHT, NULL);
				else if (theConfig.bg_mode == KINECT_BG)
				{
					cvCreateTrackbar("Far",PARAM_WINDOW,&theConfig.paramDark,PARAM_DARK, NULL);
					cvCreateTrackbar("Near",PARAM_WINDOW,&theConfig.paramBright,PARAM_BRIGHT, NULL);
				}
				else
				{
					cvCreateTrackbar("Below",PARAM_WINDOW,&theConfig.paramDark,PARAM_DARK, NULL);
					cvCreateTrackbar("Above",PARAM_WINDOW,&theConfig.paramBright,PARAM_BRIGHT, NULL);
				}
			}
			else
			{
				assert(0 && "[TODO] auto background");
				// 			cvCreateTrackbar("Auto",PARAM_WINDOW,&theConfig.paramAuto,PARAM_AUTO, onParamAuto);
				// 			onParamAuto(paramAuto);
			}
			cvCreateTrackbar("Blur",PARAM_WINDOW,&theConfig.paramBlur1,PARAM_BLUR1, NULL);
			cvCreateTrackbar("Noise",PARAM_WINDOW,&theConfig.paramBlur2,PARAM_BLUR2, NULL);
			//	cvCreateTrackbar("形态",PARAM_WINDOW,&paramNoise,PARAM_NOISE, NULL);

			cvCreateTrackbar("MinArea",PARAM_WINDOW,&theConfig.paramMinArea, PARAM_MAXAREA, NULL);
			cvCreateTrackbar("MaxArea",PARAM_WINDOW,&theConfig.paramMaxArea,PARAM_MAXAREA, NULL);
		}
		else
		{
			cvDestroyWindow(PARAM_WINDOW);
		}
	}
}
