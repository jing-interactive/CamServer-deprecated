#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "MiniTimer.h"

const int demo_expire_time = 10*60;//10 minutes

void VideoApp::run()
{	
	MiniTimer timer;
	MiniTimer timer_total;
	MiniTimer timer_demo;//for demo usage, expire in 10 minutes

	while (true)
	{ 
		FloWriteLn();

		if (theConfig.isDemo && timer_demo.getTimeElapsed() > demo_expire_time)
		{
			cvDestroyAllWindows();
			for (int k=0;k<7;k++)
				printf("CamServer is now running as a ten-minutes demo\n");
			system("pause");
			printf("If you like it and want to use it at commercial activities\n"); 
			for (int k=0;k<7;k++)
				printf("Contact me via  vinjn.z@gmail.com\n");
			system("pause");
			break;
		}

		timer.resetStartTime();
		timer_total.resetStartTime();

		IplImage* raw = _input.get_frame();
		if (!raw)
			break;			

		timer.profileFunction("get_frame");

		int key = cvWaitKey(1);
		if (key == VK_ESCAPE)
			break; 
		if (key == VK_BACK)
		{//reset four corner points
			theConfig.corners[0] = cv::Point2f(0,0);
			theConfig.corners[1] = cv::Point2f(HalfWidth,0);
			theConfig.corners[3] = cv::Point2f(0,HalfHeight);
			theConfig.corners[2] = cv::Point2f(HalfWidth,HalfHeight);

			vGetPerspectiveMatrix(warp_matrix, theConfig.corners, dstQuad);

			onRefreshBack();
		}
		if (key == VK_SPACE)
		{//toggle big window visibility
			monitorVisible = !monitorVisible;
			monitor_gui::show(monitorVisible);
		}

		//raw->half_raw->frame
		cvResize(raw, half_raw);

		vFlip(half_raw, g_Fx, g_Fy);
		timer.profileFunction("cvFlip");
		cvWarpPerspective(half_raw, frame, warp_matrix);

		timer.profileFunction("vPerspectiveTransform");

		if (to_reset_back)
		{
			to_reset_back = false;

			if (using_black_bg)
			{
				backModel->init(black_frame, (void*)&paramMoG);
				cvCopyImage(black_frame, prevBg);
			}
			else
				if (using_white_bg)
				{
					backModel->init(white_frame, (void*)&paramMoG);
					cvCopyImage(white_frame, prevBg);
				}
				else
				{//only realtime background needs take care of flip
					backModel->init(frame, (void*)&paramMoG);
					g_prevFx = g_Fx;
					g_prevFy = g_Fy;
					cvCopyImage(frame, prevBg);
				}
		}

		if (theConfig.face_track)
		{
			haar.find(frame, theConfig.paramMinArea);
			timer.profileFunction("haar.find");
		}

		IplImage* back = backModel->getBackground();//for render only

		backModel->setIntParam(0, theConfig.paramBright);
		backModel->setIntParam(1, theConfig.paramDark);
		backModel->update(frame, DETECT_BOTH);
		IplImage* fore = backModel->getForeground();

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

		send_osc_msg();
		timer.profileFunction("send_osc_msg");

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

			//show_image(total);
			const int spac = HalfWidth*0.1;
			cvLine(total, cvPoint(HalfWidth-spac, HalfHeight), cvPoint(HalfWidth+spac, HalfHeight), CV_BLUE);
			cvLine(total, cvPoint(HalfWidth, HalfHeight-spac), cvPoint(HalfWidth, HalfHeight+spac), CV_BLUE);
			cvShowImage(MAIN_WINDOW,total);

			timer.profileFunction("show Monitor");
		}

		if (_input._InputType == _input.From_Video)
		{
			DWORD elapse = timer_total.getTimeElapsedMS();
			if (elapse < 40)
				::Sleep(40 - elapse);
		}

		if (theConfig.face_track)
			sprintf(g_buffer, "cost: %d ms, %d object  %d face", timer_total.getTimeElapsedMS(), blobs.size(), haar.blobs.size());
		else
			sprintf(g_buffer, "cost: %d ms, %d object", timer_total.getTimeElapsedMS(), blobs.size());
		cvRectangle(param_gui::setting, Point(0,0), Point(400,30), CV_RGB(122,122,122), CV_FILLED);
		vDrawText(param_gui::setting, 20,20, g_buffer);

		cvShowImage(PARAM_WINDOW, param_gui::setting);
		timer.profileFunction("show Param Panel");

		timer_total.profileFunction("total"); 
	}
	theConfig.save_to("config.xml");
	cvDestroyAllWindows();
}