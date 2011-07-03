#ifndef OFXPS3_H
#define OFXPS3_H

#include "PS3EyeMulticam.h"

struct _IplImage;

class ofxPS3
{
	// Enumerate the cameras on the bus.
	static unsigned int	camNum;
	int camWidth, camHeight;
	PBYTE pBuffer;
	_IplImage* frame;
public:
	ofxPS3();
	~ofxPS3();
	bool initPS3(int width,int height, int framerate = 60);
	int getCamWidth();
	int getCamHeight();
	void listDevices();
	static int getDeviceCount();
	void showSettings();
	bool isFrameNew();
	PBYTE getPixels();
	_IplImage* getFrame();
};
#endif // OFXPS3_H_
