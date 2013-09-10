#include "../_common/OpenCV/cvButtons.h"
#include "UI.h"
#include "AppConfig.h"
#include "VideoApp.h"

using namespace cv;

struct ButtonInfo
{
    int x;
    int y;
    const char *text;
    void (*cb)(int);
    int*	value;
};

namespace monitor_gui
{
    CvButtons buttons;

    void onMonitorMouse(int event, int x, int y, int flags, void* param)
    {
        const int kBoundary = 2000;
        if (x < 0 || x > kBoundary) x = 0;
        if (y < 0 || y > kBoundary) y = 0;

        if (theConfig.scene_plot_mode)
        {
            if (x <= theApp.subWindowWidth || x >= theApp.windowWidth ||
                y <= 0 || y >= theApp.subWindowHeight)
            {
                return;
            }

            float normalizedX = (x - theApp.subWindowWidth) / static_cast<float>(theApp.subWindowWidth);
            float normalizedY = y / static_cast<float>(theApp.subWindowHeight);

            if (event == EVENT_RBUTTONUP)
            {
                theConfig.scene_plots.clear();
                return;
            }

            if (event == EVENT_LBUTTONDOWN)
            {
                theConfig.scene_plots.push_back(Point2f(normalizedX, normalizedY));
                return;
            }
        }

        if (x > theApp.subWindowWidth) x = theApp.subWindowWidth;
        if (y > theApp.subWindowHeight) y = theApp.subWindowHeight;

        if (theConfig.fixed_back_mode &&  event == EVENT_RBUTTONUP)
        {
            theApp.onRefreshBack();
            param_gui::on_realbg();
            return;
        }
        if (event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON))
        {
            theApp.selectedCorner = NULL;
            return;
        }

        if (event == EVENT_LBUTTONDOWN)
        {
            theApp.selectedCorner = NULL;

            //todo
            for (int i=0;i<4;i++)
            {
                cv::Point2f* pt = &theConfig.cornersA[i];
                const int kThreshold = 15;
                if (abs(pt->x - x) < kThreshold && abs(pt->y - y) < kThreshold)
                {
                    theApp.selectedCorner = pt;
                    break;
                }
            }
            return;
        }

        if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
        {
            if (theApp.selectedCorner)
            {
                theApp.selectedCorner->x = x;
                theApp.selectedCorner->y = y;
                theApp.warpMatrix = getPerspectiveTransform(theConfig.cornersA, theApp.dstQuad);
            }
            return;
        }
    }

    void* handle = NULL;

    void onOpenGLDraw(void* userdata)
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
            setOpenGlDrawCallback(MAIN_WINDOW, onOpenGLDraw);
            handle = cvGetWindowHandle(MAIN_WINDOW);
        }
        else
        {
            setOpenGlDrawCallback(MAIN_WINDOW, NULL);
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
            theApp.toResetBackground = false;
            theApp.backModel->init(theApp.prevBg);
        }
    }

    void on_change_bg(int mode)
    {
        if (theConfig.bg_mode != mode)
        {
            theConfig.bg_mode = mode;
            show(true);			
        }
        theApp.onRefreshBack();		
    }

    void on_realbg(int t)
    {
        on_change_bg(REAL_BG);	
    }

    void on_whitebg(int t)
    {
        on_change_bg(WHITE_BG);	
    }

    void on_diffbg(int t)
    {
        on_change_bg(DIFF_BG);	
    }

    void on_blackbg(int t)
    {
        on_change_bg(BLACK_BG);
    }

    const ButtonInfo btnInfs[]=
    {
        { w=13, h=35,  "- X -", on_x, &theConfig.paramFlipX},
        { w+=dw, h,  "- Y -", on_y, &theConfig.paramFlipY},
        { w+=dw, h,  "tuio", NULL, &theConfig.tuio_mode},
#ifdef FACE_DETECTION_ENABLED
        { w+=dw, h,  "face", NULL, &theConfig.face_track},
#endif
        { w+=dw, h,  "hull", NULL, &theConfig.hull_mode},
        { w+=dw, h,  "plot", NULL, &theConfig.scene_plot_mode},
        //		{ w+=dw, h,  "gray", on_mode, &theConfig.gray_detect_mode},
        { w=13, h+=dh,  "n)ow", on_realbg, NULL},
        { w+=dw, h,  "w)hite", on_whitebg, NULL},
        { w+=dw, h,  "b)lack", on_blackbg, NULL},
        { w+=dw, h,  "d)iff", on_diffbg, NULL},
    };

    const int num_btns = sizeof(btnInfs)/sizeof(btnInfs[0]);

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
        setting.create(Size(400, 120), CV_8UC3);
        buttons.release();

        for (int i=0;i<num_btns;i++)
        {
            const ButtonInfo& inf = btnInfs[i];
            buttons.addButton(PushButton(inf.x, inf.y, _w, _h, inf.value, inf.text, inf.cb));
        }
        update();
    }

    void onParamMouse(int event, int x, int y, int flags, void* param)
    {
        cvButtonsOnMouse(event, x, y, flags, param);
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

            setMouseCallback(PARAM_WINDOW, onParamMouse, &buttons);

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
