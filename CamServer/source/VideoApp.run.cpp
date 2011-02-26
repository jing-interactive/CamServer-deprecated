#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "MiniTimer.h"

void VideoApp::run()
{	
	MiniTimer timer;
	MiniTimer timer_total;
	int ms_counter = 0;
	int frame_counter = 0;
	int fps=0;

	while (app_running)
	{ 
		FloWriteLn();

		timer.resetStartTime();
		timer_total.resetStartTime();

		IplImage* raw = input._frame;
		if (!raw)
			break;

		if (grab_thread->count == input._frame_num)
		{
			//raw->half_raw->frame
			//grab_thread->lock();
			cvResize(raw, half_raw);
			//grab_thread->unlock();
			timer.profileFunction("cvResize");
		}

		int key = cvWaitKey(1);

		switch (key)
		{
		case VK_ESCAPE:
			app_running = false;
			break;

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
		timer.profileFunction("vPerspectiveTransform");	

		if (to_reset_back)
		{
			to_reset_back = false;

			if (theConfig.bg_mode == BLACK_BG)
			{
				backModel->init(black_frame, (void*)&paramMoG);
				cvCopyImage(black_frame, prevBg);
			}
			else
				if (theConfig.bg_mode == WHITE_BG)
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

#ifdef FINGER_MATCH
		//finger match
		if (blobs.size() > 0)
		{
			vBlob& b = blobs[0];
			Ptr<IplImage> finger = cvCreateImage(cvSize(b.box.width, b.box.height), 8, 1);
			cvSetImageROI(fore, b.box);
			cvCopyImage(fore, finger);
			vThresh(finger, 10);
			cvResetImageROI(fore);
			show_image(finger);

			double match = cvMatchShapes(finger, finger_template, CV_CONTOURS_MATCH_I2);
			IplImage* matched = NULL;
			if (match < 0.01 )
			{
				matched = finger_template;
				printf("%s with %f\n", "match1" , match);
			}
			else
			{
				double match = cvMatchShapes(finger, finger_template2, CV_CONTOURS_MATCH_I2);
				if (match < 0.04 )
				{
					matched = finger_template2;
					printf("%s with %f\n", "match2" , match);
				}
				else
				{
					double match = cvMatchShapes(finger, finger_template3, CV_CONTOURS_MATCH_I2);
					if (match < 0.02 )
					{
						matched = finger_template3;
						printf("%s with %f\n", "match3" , match);
					}
				}
			}
			if (matched)
				show_image(matched);
		}
#endif

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

		frame_counter++;

		if ((ms_counter += timer_total.getTimeElapsedMS()) > 1000)
		{
			fps = frame_counter;
			frame_counter = 0;
			ms_counter = 0;
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