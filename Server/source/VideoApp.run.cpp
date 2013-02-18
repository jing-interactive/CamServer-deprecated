#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "MiniTimer.h"
#include <opencv2/flann/logger.h>

using namespace cv;
using cvflann::Logger;

#ifndef WIN32
#define VK_BACK 8
#define VK_RETURN 13
#define VK_ESCAPE 27
#define VK_SPACE 32
#define VK_LEFT 37
#define VK_UP 38
#define VK_RIGHT 39
#define VK_DOWN 40
#endif

void VideoApp::run()
{	
	MiniTimer timer(theConfig.log_file);
	MiniTimer timer_total(theConfig.log_file);
	int _ms_counter = 0;
	int _frm_counter = 0;
	int fps=0;

	while (app_running)
	{
		if (theConfig.log_file)
        {
			MiniLog("\n");
            //todo: replace MiniLog with 
            //Logger::info();
        }

		timer.resetStartTime();
		timer_total.resetStartTime();

		Mat raw = input._frame; 
		if (raw.empty())
			break;

		if (raw.cols == HalfWidth)
			raw.copyTo(half_raw);
		else
			resize(raw, half_raw, half);
		timer.profileFunction("cvResize");

		int key = cvWaitKey(1);

		switch (key)
		{
		case VK_ESCAPE:
			{
				app_running = false;
			}break;
		case VK_BACK:
			{//reset four corner points
				theConfig.corners[0] = cv::Point2f(0,0);
				theConfig.corners[1] = cv::Point2f(HalfWidth,0);
				theConfig.corners[3] = cv::Point2f(0,HalfHeight);
				theConfig.corners[2] = cv::Point2f(HalfWidth,HalfHeight);

				vGetPerspectiveMatrix(warp_matrix, theConfig.corners, dstQuad);

				onRefreshBack();
			}break;
		case VK_SPACE:
			{//toggle big window visibility
				monitorVisible = !monitorVisible;
				monitor_gui::show(monitorVisible);
			}break;
		case 'b':
			{
				param_gui::on_realbg(0);
				param_gui::update();
			}break;
		default:break;
		}
		vFlip(cv::Mat(half_raw), g_Fx, g_Fy);
		timer.profileFunction("cvFlip");

		if (theConfig.corners[0] == cv::Point2f(0,0) && theConfig.corners[1] == cv::Point2f(HalfWidth,0)
			&& theConfig.corners[3] == cv::Point2f(0,HalfHeight) && theConfig.corners[2] == cv::Point2f(HalfWidth,HalfHeight)
			)
		{
            // if original ROI (region of interest), it saves time
			half_raw.copyTo(frame);
		}
		else
		{
            // needs perspective transform
			warpPerspective(half_raw, frame, Mat(warp_matrix), half);
//			cvWarpPerspective(half_raw, frame, warp_matrix);
		}
		if (grab_thread->is_dirty())
		{ 
			if (theConfig.bg_mode == DIFF_BG)
			{
				//prevFrame->diff_Bg
				vBackGrayDiff* diff = (vBackGrayDiff*)(IBackGround*)backModel;
				if (channels == 3)
					vGrayScale(prevBg, diff->bg);
				else
					prevBg.copyTo(diff->bg);
				//clean frame->prevFrame
				frame.copyTo(prevBg);
			}
		}
		timer.profileFunction("vPerspectiveTransform");	

		if (to_reset_back)
		{
			to_reset_back = false;

			switch (theConfig.bg_mode)
			{
			case REAL_BG:
				{
					//only real time background needs take care of flip
					backModel->init(frame, (void*)&paramMoG);
					g_prevFx = g_Fx;
					g_prevFy = g_Fy;
					frame.copyTo(prevBg);
				}break;
			case BLACK_BG:
				{
					backModel->init(black_frame, (void*)&paramMoG);
					black_frame.copyTo(prevBg); 
				}break;
			case WHITE_BG:
				{
					backModel->init(white_frame, (void*)&paramMoG);
					white_frame.copyTo(prevBg);
				}break;
			case DIFF_BG:
				{
					//copy it in case huge blobs generated
					backModel->init(frame, (void*)&paramMoG);
				}break;
			default:
				break;
			}
 		}

		if (theConfig.face_track)
		{
			haar.find(&(IplImage)frame, theConfig.paramMinArea);
			timer.profileFunction("haar.find");
		}

		{
			backModel->setIntParam(0, theConfig.paramBright);
			backModel->setIntParam(1, theConfig.paramDark);
			backModel->update(frame, DETECT_BOTH);
			fore = backModel->getForeground();
			back = backModel->getBackground();//for render only
		}

		timer.profileFunction("backModel->update");
		//fore->blobs
		vHighPass(fore, grayBuffer, theConfig.paramBlur1, theConfig.paramBlur2);		
		grayBuffer.copyTo(fore);

		timer.profileFunction("vHighPass");

		const int scale =HalfWidth*HalfHeight/PARAM_MAXAREA;
		int minArea = (theConfig.paramMinArea)*scale/10;
		int maxArea = minArea+(theConfig.paramMaxArea)*scale;
		vFindBlobs(grayBuffer, blobs, minArea ,maxArea, theConfig.hull_mode == 1);
		timer.profileFunction("vFindBlobs");

		if (theConfig.finger_track)
		{
			for (unsigned int i=0;i<blobs.size();i++)
			{
				bool ffound=finger.findFingers(blobs[i], 12);
				//	bool hfound=finger.findHands(blobs[0]);
				if (ffound)
				{
					for (int f=0;f<finger.ppico.size();f++)
					{	
						circle(frame, finger.ppico[f], 10, vRandomColor());
					}
				}
			}
		}

		blobTracker.trackBlobs(blobs);
		timer.profileFunction("blobTracker");

		if (theConfig.tuio_mode)
		{
			send_tuio_msg();
			timer.profileFunction("send_tuio_msg");
		}
		else
		{
			send_custom_msg();
			timer.profileFunction("send_osc_msg");
		}

		if (monitorVisible)
		{
            setOpenGlContext(MAIN_WINDOW);

            mTexMainWindows[0]->copyFrom(half_raw);
            mTexMainWindows[1]->copyFrom(frame);
            mTexMainWindows[2]->copyFrom(fore);
            mTexMainWindows[3]->copyFrom(back);

            updateWindow(MAIN_WINDOW);
            // InvalidateRect --> issue WM_PAINT
            // HighGUIProc --> drawGl(window); 
            // VideoApp::renderMainWindow
		}

		_frm_counter++;

		if ((_ms_counter += timer_total.getTimeElapsedMS()) > 1000)
		{
			fps = _frm_counter;
			_frm_counter = 0;
			_ms_counter = 0;
		}

		sprintf(g_buffer, "FPS {Cam %d Server %d} %d object", grab_thread->fps, fps, blobs.size());
		if (theConfig.face_track)
			sprintf(g_buffer, "%s %d face", g_buffer, haar.blobs.size()); 
		rectangle(param_gui::setting, Point(0,0), Point(400,30), CV_RGB(122,122,122), CV_FILLED);
		cv::Mat m = param_gui::setting;
		vDrawText(m, 20,20, g_buffer);

		imshow(PARAM_WINDOW, param_gui::setting);
		timer.profileFunction("show Param Panel");

		timer_total.profileFunction("total"); 
	}

	theConfig.save_to(CONFIG_FILE);
	cvDestroyAllWindows();
}

void VideoApp::renderMainWindow()
{
    if (!monitorVisible)
        return;

    mCam2d.setupProjectionMatrix();
    for (int i=0;i<4;i++)
    {
        render(*mTexMainWindows[i], mRoiMainWindows[i]);
    }

    std::vector<Point3f> lines;
    {
#ifndef VERSION_OPENGL_COMES
        const Point2f kk(5,5);
        for (int i=0;i<4;i++)
        {
            line(total, theConfig.corners[i], theConfig.corners[(i+1)%4], CV_RGB(255,0,0),2);
            rectangle(total, theConfig.corners[i] - kk, theConfig.corners[i] + kk, CV_RGB(255,0,0), CV_FILLED);
        }
        if (selected)
            rectangle(total, *selected - kk, *selected + kk, CV_RGB(0,0,255), CV_FILLED);
#endif
        const float spac = 0.05f;
        lines.push_back(Point3f(0.5f-spac,  0.5f,       0));
        lines.push_back(Point3f(0.5f+spac,  0.5f,       0));
        lines.push_back(Point3f(0.5f,       0.5f-spac,  0));
        lines.push_back(Point3f(0.5f,       0.5f+spac,  0));
    }
    mVboLines->setVertexArray(lines);
    render(*mVboLines, RenderMode::LINES, CV_BLUE);
}

void VideoApp::setupOpenglResources()
{
    setOpenGlContext(MAIN_WINDOW);
    for (int i=0;i<4;i++)
    {
        mTexMainWindows[i] = new cv::GlTexture;
    }
    mVboLines = new cv::GlArrays;
}