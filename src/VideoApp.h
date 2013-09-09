#pragma once

#include "../_common/OpenCV/OpenCV.h"
#include "../_common/OpenCV/cvButtons.h"
#include "../_common/OpenCV/BlobTracker.h"

#include "../_common/MiniThread.h"
#include "../_common/ofxOsc/ofxOsc.h"

#ifdef WIN32 
#define SLEEP(ms) ::Sleep((ms))
#else
#define SLEEP(ms)	usleep(ms)
#endif

#ifdef WIN32 
#define BEEP(freq, ms) ::Beep((freq), (ms))
#else
#define BEEP(freq, ms)
#endif

struct VideoGrabThread: public MiniThread
{
	int fps;

	VideoInput& input;

	VideoGrabThread(VideoInput& input);
	void threadedFunction();
	bool isDirty();
private:
	bool mDirty;
};

struct VideoApp
{
	//thread
	cv::Ptr<VideoGrabThread> grab_thread;
    VideoInput mInput;

	//the important objects
	HaarFinder haar;
	vBlobTracker blobTracker;
	ofxOscSender sender;

	//
	char g_buffer[256];
	bool app_running;
	bool input_inited;

	cv::Point2f* selectedCorner;
	cv::Point2f dstQuad[4];
	vector<vBlob> blobs;
	cv::Mat warp_matrix;

	//the Mats
	cv::Mat total;
	cv::Mat frame, black_frame, white_frame;
	cv::Mat prevBg;
	cv::Mat half_raw;
	cv::Mat grayBuffer;

    cv::Mat back;//for render only
    cv::Mat fore;

	cv::Size size;
	int channels;
	cv::Size half;
	bool monitorVisible;

	cv::Ptr<IBackGround> backModel;

	int HalfWidth, HalfHeight;
	bool to_reset_back;
	int g_Fx, g_Fy;
	int g_prevFx, g_prevFy;

	VideoApp();
	~VideoApp();

	bool init(int argc, char** argv);

	void onRefreshBack();
	void onParamFlip(int fx, int fy);

	void run();

private:
	void parse_args(int argc, char** argv);
	void send_custom_msg();
	void send_tuio_msg();

    // opengl related
public:
    void renderMainWindow();
    void setupOpenglResources();

private:
    cv::Ptr<cv::ogl::Texture2D>  mTexMainWindows[4];
    cv::Rect_<double>       mRoiMainWindows[4];
    cv::Ptr<cv::ogl::Arrays>   mVboLines;
    //cv::GlCamera            mCam2d;
};

extern VideoApp theApp;
