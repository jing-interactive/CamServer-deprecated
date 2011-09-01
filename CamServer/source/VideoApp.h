#pragma once

#define MODE_320X240	//always 320*240

#include "vOpenCV/OpenCV.h"
#include "vOpenCV/cvButtons.h"
#include "vOpenCV/BlobTracker.h"

#include "ofxThread.h"
#include "ofxOsc/ofxOsc.h"

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
		int fps;

		VideoInput& _input;

		VideoGrabThread(VideoInput& input);
		void threadedFunction();
		bool is_dirty();
	private:
		bool _dirty;
	};

	//thread
	Ptr<VideoGrabThread> grab_thread;
	VideoInput input;

	//the important objects
	vHaarFinder haar;
	vFingerDetector finger;
	vBlobTracker blobTracker;
	ofxOscSender sender;

	//
	char g_buffer[256];
	bool app_running;
	bool input_inited;

	cv::Point2f* selected;
	cv::Point2f dstQuad[4];
	vector<vBlob> blobs;
	cv::Rect roi[4];
	CvMat* warp_matrix;

	//the IplImages
	Ptr<IplImage> total;
	Ptr<IplImage> frame, black_frame, white_frame, kinect_frame;
	Ptr<IplImage> prevBg;

	//
	Ptr<IplImage> half_raw;
	Ptr<IplImage> half_flip;
	Ptr<IplImage> grayBlob;
	Ptr<IplImage> grayBuffer;

	cv::Size size;
	int channels;
	cv::Size half;
	bool monitorVisible;

	Ptr<IBackGround> backModel;
	CvGaussBGStatModelParams paramMoG;

	int HalfWidth, HalfHeight;
	bool to_reset_back;
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
	void send_custom_msg();
	void send_tuio_msg();
};

extern VideoApp theApp;
