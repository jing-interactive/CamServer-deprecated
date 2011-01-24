#pragma once

#include "vOpenCV/OpenCV.h"
#include "vOpenCV/cvButtons.h"
#include "vOpenCV/BlobTracker.h"

#include "ofxOsc/ofxOsc.h"

class ofxThread;

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

struct VideoApp
{
	struct VideoGrabThread: public ofxThread
	{
		bool is_new_frame;
		IplImage* frame;

		VideoInput input;
		int _argc;
		char** _argv;

		VideoGrabThread(int argc, char** argv);
		void threadedFunction();
	};

	//thread
	Ptr<VideoGrabThread> grag_thread;

	//the important objects
	vHaarFinder haar;
	vFingerDetector finger;
	vBlobTracker blobTracker;
	ofxOscSender sender;

	//
	char g_buffer[256];

	cv::Point2f* selected;
	cv::Point2f dstQuad[4];
	vector<vBlob> blobs;
	cv::Rect roi[4];
	CvMat* warp_matrix;

	//the IplImages
	Ptr<IplImage> total;
	Ptr<IplImage> frame, black_frame, white_frame;
	Ptr<IplImage> prevBg;

	//
	Ptr<IplImage> half_raw;
	Ptr<IplImage> half_flip;
	Ptr<IplImage> grayBlob;
	Ptr<IplImage> grayBuffer;

	cv::Size size;
	cv::Size half;
	bool monitorVisible;

	Ptr<IBackGround> backModel;
	CvGaussBGStatModelParams paramMoG;

	int HalfWidth, HalfHeight;
	bool to_reset_back;
	bool using_black_bg;
	bool using_white_bg;
	int g_Fx, g_Fy;
	int g_prevFx, g_prevFy;

	VideoApp();
	~VideoApp();

	bool init(int argc, char** argv);

	void onRefreshBack();
	void onParamFlip(int fx, int fy);
	void onParamAuto(int v);

	void run();

	void parse_args(int argc, char** argv);
	void send_osc_msg();
};

extern VideoApp theApp;
