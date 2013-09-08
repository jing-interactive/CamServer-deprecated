#include "VideoApp.h"
#include "AppConfig.h"
#include "UI.h"
#include "MiniTimer.h"
#include "opencv2/flann/logger.h"
#include "opencv2/core/opengl.hpp"

#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

using namespace cv;
using cvflann::Logger;

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

        int key = waitKey(1);

		switch (key)
		{
		case VK_ESCAPE:
			{
				app_running = false;
			}break;
		case VK_BACK:
			{//reset four corner points
				theConfig.corners[0] = Point2f(0,0);
				theConfig.corners[1] = Point2f(HalfWidth,0);
				theConfig.corners[3] = Point2f(0,HalfHeight);
				theConfig.corners[2] = Point2f(HalfWidth,HalfHeight);

                warp_matrix = getPerspectiveTransform(theConfig.corners, dstQuad);

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

		if (theConfig.corners[0] == Point2f(0,0) && theConfig.corners[1] == Point2f(HalfWidth,0)
			&& theConfig.corners[3] == Point2f(0,HalfHeight) && theConfig.corners[2] == Point2f(HalfWidth,HalfHeight)
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
		if (grab_thread->isDirty())
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
					backModel->init(frame);
					g_prevFx = g_Fx;
					g_prevFy = g_Fy;
					frame.copyTo(prevBg);
				}break;
			case BLACK_BG:
				{
					backModel->init(black_frame);
					black_frame.copyTo(prevBg); 
				}break;
			case WHITE_BG:
				{
					backModel->init(white_frame);
					white_frame.copyTo(prevBg);
				}break;
			case DIFF_BG:
				{
					//copy it in case huge blobs generated
					backModel->init(frame);
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
		Mat m = param_gui::setting;
		vDrawText(m, 20,20, g_buffer);

        imshow(PARAM_WINDOW, param_gui::setting);
		timer.profileFunction("show Param Panel");

		timer_total.profileFunction("total"); 
	}

	theConfig.save_to(CONFIG_FILE);

    setupOpenglResources();// fix pure virtual function call occurred when built statically

    destroyAllWindows();
}

namespace 
{
    template <typename T>
    void drawStrokedRect( const Rect_<T> &rect )
    {
        static cv::ogl::Arrays vbo;
        vector<Point_<T>> verts;
        verts.push_back(Point_<T>(rect.x,                 rect.y));
        verts.push_back(Point_<T>(rect.x + rect.width,    rect.y));
        verts.push_back(Point_<T>(rect.x + rect.width,    rect.y + rect.height));
        verts.push_back(Point_<T>(rect.x,                 rect.y + rect.height));
        vbo.setVertexArray(verts);
        render(vbo, ogl::LINES);
    }

    //void drawStrokedEllipse( const Vec2f &center, float radiusX, float radiusY, int numSegments )
    //{
    //    // automatically determine the number of segments from the circumference
    //    if( numSegments <= 0 ) {
    //        numSegments = (int)math<double>::floor( std::max(radiusX,radiusY) * M_PI * 2 );
    //    }
    //    if( numSegments < 2 ) numSegments = 2;

    //    GLfloat *verts = new float[numSegments*2];
    //    for( int s = 0; s < numSegments; s++ ) {
    //        float t = s / (float)numSegments * 2.0f * 3.14159f;
    //        verts[s*2+0] = center.x + math<float>::cos( t ) * radiusX;
    //        verts[s*2+1] = center.y + math<float>::sin( t ) * radiusY;
    //    }
    //    glEnableClientState( GL_VERTEX_ARRAY );
    //    glVertexPointer( 2, GL_FLOAT, 0, verts );
    //    glDrawArrays( GL_LINE_LOOP, 0, numSegments );
    //    glDisableClientState( GL_VERTEX_ARRAY );
    //    delete [] verts;
    //}

}
void VideoApp::renderMainWindow()
{
    if (!monitorVisible)
        return;

    //mCam2d.setupProjectionMatrix();
    for (int i=0;i<4;i++)
    {
        render(*mTexMainWindows[i], mRoiMainWindows[i]);
    }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, total.cols, total.rows, 0, -1.0f, 1.0f );

    std::vector<Point2f> vertices;
    std::vector<Scalar> colors;
    const Point2f kMarkerSize(5,5);

    for (int i=0;i<4;i++)
    {
        //drawStrokedRect(Rect(theConfig.corners[i] - kMarkerSize, theConfig.corners[i] + kMarkerSize));
        vertices.push_back(theConfig.corners[i]);       colors.push_back(CV_RED);
        vertices.push_back(theConfig.corners[(i+1)%4]); colors.push_back(CV_RED);

        //line(total, theConfig.corners[i], theConfig.corners[(i+1)%4], CV_RGB(255,0,0),2);
        //rectangle(total, theConfig.corners[i] - kMarkerSize, theConfig.corners[i] + kMarkerSize, CV_RGB(255,0,0), CV_FILLED);
    }
    if (selectedCorner)
    {
        rectangle(total, *selectedCorner - kMarkerSize, *selectedCorner + kMarkerSize, CV_RGB(0,0,255), CV_FILLED);
    }

    const float spac = 0.05f * total.rows;
    vertices.push_back(Point2f(0.5f*total.cols-spac,  0.5f*total.rows));      colors.push_back(CV_BLUE);
    vertices.push_back(Point2f(0.5f*total.cols+spac,  0.5f*total.rows));      colors.push_back(CV_BLUE);
    vertices.push_back(Point2f(0.5f*total.cols,       0.5f*total.rows-spac)); colors.push_back(CV_BLUE);
    vertices.push_back(Point2f(0.5f*total.cols,       0.5f*total.rows+spac)); colors.push_back(CV_BLUE);
    
    mVboLines->setVertexArray(vertices);
    mVboLines->setColorArray(colors);
    render(*mVboLines, ogl::LINES);
}

void VideoApp::setupOpenglResources()
{
    setOpenGlContext(MAIN_WINDOW);
    for (int i=0;i<4;i++)
    {
        mTexMainWindows[i] = new ogl::Texture2D;
    }
    mVboLines = new ogl::Arrays;
}