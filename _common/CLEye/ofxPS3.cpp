// Multicam version of PS3Eye fix
#include "ofxPS3.h"
#include <stdio.h>
#include <opencv/cv.h>

#pragma comment(lib, "PS3EyeMulticam.lib")

#define CAM_CONFIG_FILE "../../data/ps3_cam.xml"

unsigned int ofxPS3::camNum=0;

ofxPS3::ofxPS3() 
{
}

void ofxPS3::listDevices()
{
	// Enumerate the cameras on the bus.
	camNum = PS3EyeMulticamGetCameraCount();
 	printf("\nFound %d PS3Eye camera(s)...\n", camNum);
}

PBYTE ofxPS3::getPixels()
{
	return pBuffer;
}

bool ofxPS3::isFrameNew()
{
	return PS3EyeMulticamGetFrame(pBuffer);
}

bool ofxPS3::initPS3(int width,int height, int framerate)
{
	bool result = false;
	result = PS3EyeMulticamOpen(camNum, height==480?VGA:QVGA, framerate);
	result = PS3EyeMulticamLoadSettings(CAM_CONFIG_FILE);
	// get stitched image width
	PS3EyeMulticamGetFrameDimensions(camWidth, camHeight);
	// Allocate image buffer for grayscale image
	pBuffer = new BYTE[camWidth*camHeight];
	frame = cvCreateImageHeader(cvSize(camWidth, camHeight), 8, 1);

	// Start capturing
	return PS3EyeMulticamStart();
}

int ofxPS3::getDeviceCount()
{
	return PS3EyeMulticamGetCameraCount();
}

int ofxPS3::getCamWidth()
{
	return camWidth;
}

int ofxPS3::getCamHeight()
{
	return camHeight;
}

void ofxPS3::showSettings()
{
	PS3EyeMulticamShowSettings();
}

// Clean up
ofxPS3::~ofxPS3()
{
	// Stop capturing
	PS3EyeMulticamStop();
	Sleep(50);
	PS3EyeMulticamSaveSettings(CAM_CONFIG_FILE);
	PS3EyeMulticamClose();	
	delete [] pBuffer;

	cvReleaseImageHeader(&frame);
}

IplImage* ofxPS3::getFrame()
{
	cvSetImageData(frame, pBuffer, frame->widthStep);
	return frame;
}