/*
*  ofxKinectCLNUI.h
*
*  Created by yangyang on 10/1/2011.
*  Welcome to visit http://www.cnblogs.com/yangyangcv/
*
*  CL NUI SDK is developed by AlexP
*  see http://codelaboratories.com/nui/
*/

#ifndef OFXKINECTCLNUI_H_INCLUDED
#define OFXKINECTCLNUI_H_INCLUDED

#include "CLNUIDevice.h"
#include <opencv/cv.h>

class ofxKinectCLNUI
{
private:
	CLNUICamera kinectCam;
	int camWidth;
	int camHeight;
	IplImage *depthShortImage;
	IplImage *bgrImage;
	
	USHORT *depthData;
	BYTE *colorData;
	BYTE *depth8;
	DWORD* depthColorData;
	
public:
	ofxKinectCLNUI();
	~ofxKinectCLNUI();
	bool initKinect(int wid,int hei,double lowbound,double upbound);
	void setUpBound(double bound);//up and low threshold for the depth data
	void setLowBound(double bound);
	bool getDepthBW();//threshold on the depth date we can get this BW image
	bool getDepthColorImage();//pseudo-color depth image
	bool getColorImage();//color image from the rgb camera

	static int getCount();
	
	int getCamWidth();
	int getCamHeight();

	double upBound, lowBound;//up and low threshold for the depth data
	IplImage *bwImage;//threshold on the depth date we can get this BW image
	IplImage *rgbImage;//rgb image from the rgb camera
	IplImage* depthColorImage;//pseudo-color depth image
};

#endif