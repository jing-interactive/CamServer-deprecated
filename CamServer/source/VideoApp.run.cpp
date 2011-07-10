#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "MiniTimer.h"

void VideoApp::run()
{	
	MiniTimer timer;
	MiniTimer timer_total;
	int _ms_counter = 0;
	int _frm_counter = 0;
	int fps=0;

	while (app_running)
	{ 
		FloWriteLn();

		timer.resetStartTime();
		timer_total.resetStartTime();

		IplImage* raw = input._frame;
		if (!raw)
			break;

		cvResize(raw, half_raw);
		timer.profileFunction("cvResize");

		int key = cvWaitKey(1);

		switch (key)
		{
		case VK_ESCAPE:
			app_running = false;

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
		vFlip(half_raw, g_Fx, g_Fy);
		timer.profileFunction("cvFlip");

		if (theConfig.corners[0] == cv::Point2f(0,0) && theConfig.corners[1] == cv::Point2f(HalfWidth,0)
			&& theConfig.corners[3] == cv::Point2f(0,HalfHeight) && theConfig.corners[2] == cv::Point2f(HalfWidth,HalfHeight)
			)
		{//original ROI (region of interest), it saves time
			cvCopyImage(half_raw, frame);
		}
		else
		{//needs perspective transform
			cvWarpPerspective(half_raw, frame, warp_matrix);
		}
		if (grab_thread->is_dirty())
		{ 
			if (theConfig.bg_mode == DIFF_BG)
			{
				//prevFrame->diff_Bg
				vBackGrayDiff* diff = (vBackGrayDiff*)(IBackGround*)backModel;
				if (channels == 3)
					vGrayScale(prevBg, diff->Bg);
				else
					cvCopyImage(prevBg, diff->Bg);
				//clean frame->prevFrame
				cvCopyImage(frame, prevBg);
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
					cvCopyImage(frame, prevBg);
				}break;
			case BLACK_BG:
				{
					backModel->init(black_frame, (void*)&paramMoG);
					cvCopyImage(black_frame, prevBg);
				}break;
			case WHITE_BG:
				{
					backModel->init(white_frame, (void*)&paramMoG);
					cvCopyImage(white_frame, prevBg);
				}break;
			case DIFF_BG:
				{
					//copy it in case huge blobs generated
					backModel->init(frame, (void*)&paramMoG);
				}break;
			case KINECT_BG:
				{

				}break;
			default:
				break;
			}
 		}

		if (theConfig.face_track)
		{
			haar.find(frame, theConfig.paramMinArea);
			timer.profileFunction("haar.find");
		}

		IplImage* back = NULL;//for render only
		IplImage* fore = NULL;

		if (theConfig.bg_mode == KINECT_BG)
		{
			cvInRangeS( frame, cvScalar(theConfig.paramDark), cvScalar(theConfig.paramBright), kinect_frame );
			fore = kinect_frame;
			back = kinect_frame;
		}
		else
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
		cvCopyImage(grayBuffer, fore);
		timer.profileFunction("vHighPass");

		const int scale =HalfWidth*HalfHeight/PARAM_MAXAREA;
		int minArea = (theConfig.paramMinArea+1)*scale*0.1;
		int maxArea = theConfig.paramMaxArea*scale;// + HalfWidth*HalfHeight*0.1;
		if (maxArea < minArea*10)
			maxArea = minArea*10;
		vFindBlobs(grayBuffer, blobs, minArea ,maxArea, theConfig.hull_mode == 1);
		timer.profileFunction("vFindBlobs");

		if (theConfig.finger_track)
		{
			for (UINT i=0;i<blobs.size();i++)
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
			vCopyImageTo(half_raw, total, roi[0]);
			vCopyImageTo(frame, total, roi[1]);
			vCopyImageTo(fore, total, roi[2]);
			vCopyImageTo(back, total, roi[3]);

			const Point2f kk(5,5);
			for (int i=0;i<4;i++)
			{
				cvLine(total, theConfig.corners[i], theConfig.corners[(i+1)%4], CV_RGB(255,0,0),2);
				cvRectangle(total, theConfig.corners[i] - kk, theConfig.corners[i] + kk, CV_RGB(255,0,0), CV_FILLED);
			}
			if (selected)
				cvRectangle(total, *selected - kk, *selected + kk, CV_RGB(0,0,255), CV_FILLED);

			const int spac = HalfWidth*0.1;
			cvLine(total, cvPoint(HalfWidth-spac, HalfHeight), cvPoint(HalfWidth+spac, HalfHeight), CV_BLUE);
			cvLine(total, cvPoint(HalfWidth, HalfHeight-spac), cvPoint(HalfWidth, HalfHeight+spac), CV_BLUE);
			cvShowImage(MAIN_WINDOW,total);

			timer.profileFunction("show Monitor");
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
		cvRectangle(param_gui::setting, Point(0,0), Point(400,30), CV_RGB(122,122,122), CV_FILLED);
		vDrawText(param_gui::setting, 20,20, g_buffer);

		cvShowImage(PARAM_WINDOW, param_gui::setting);
		timer.profileFunction("show Param Panel");

		timer_total.profileFunction("total"); 
	}

	theConfig.save_to("config.xml");
	cvDestroyAllWindows();
}