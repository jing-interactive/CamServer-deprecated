//ofxCLeye
//by Elliot Woods, Kimchi and Chips 2010
//MIT license

#include "ofxCLeye.h"
#include <stdio.h>
#include <opencv2/core/core_c.h>

#pragma comment(lib, "CLEyeMulticam.lib")

ofxCLeye::ofxCLeye()
{
	attemptFramerate = 60;
	deviceID = 0;
	frame = NULL;
}

ofxCLeye::~ofxCLeye()
{ 
	close(); 
}

int ofxCLeye::listDevices()
{
	int numCams = CLEyeGetCameraCount();

	printf("ofxCLeye found %d PlayStation Eye\n", numCams);

	GUID guidCam;
	for (int i=0; i<numCams; i++)
	{
		guidCam = CLEyeGetCameraUUID(i);
		printf("Camera %d GUID: [%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x]\n", 
			i, guidCam.Data1, guidCam.Data2, guidCam.Data3,
			guidCam.Data4[0], guidCam.Data4[1], guidCam.Data4[2],
			guidCam.Data4[3], guidCam.Data4[4], guidCam.Data4[5],
			guidCam.Data4[6], guidCam.Data4[7]);
	}

	return numCams;
}

bool ofxCLeye::getFrame()
{
	bool grab = CLEyeCameraGetFrame(_cam, (PBYTE) viPixels); 

	if (colourMode == CLEYE_MONO_PROCESSED)
		memcpy(pixels, viPixels, width*height);
	else
	{
		for (int i=0; i<width*height; i++)
		{
			pixels[i*3+0] = viPixels[i*4+0];
			pixels[i*3+1] = viPixels[i*4+1];
			pixels[i*3+2] = viPixels[i*4+2];
		}
	}
	cvSetData(frame, pixels, frame->widthStep);

	return grab;
}

void ofxCLeye::close()
{ 
	// Stop camera capture
	CLEyeCameraStop(_cam);
	// Destroy camera object
	CLEyeDestroyCamera(_cam);
	_cam = NULL;

	cvReleaseImageHeader(&frame);
	delete[] viPixels;
	delete[] pixels;
}

bool ofxCLeye::init(int w, int h, bool isGrey)
{
	if (isGrey)
		colourMode = CLEYE_MONO_PROCESSED;
	else
		colourMode = CLEYE_COLOR_PROCESSED;

	width = w;
	height = h; 

	////////////////////////////////////////
	// RESOLUTION
	////////////////////////////////////////
	if (width == 640 && height == 480)
		_resolution=CLEYE_VGA;
	else if (width == 320 && height == 240)
		_resolution=CLEYE_QVGA;
	else
		// 	{
		// 		ofLog(OF_LOG_WARNING, "Selected resolution " + ofToString(width) + "x" + ofToString(height) + " is not available with ofxCLeye");
		// 		ofLog(OF_LOG_WARNING, "ofxCLeye using 640x480 instead");
		_resolution=CLEYE_VGA; 
	////////////////////////////////////////

	////////////////////////////////////////
	// INITIALISE
	////////////////////////////////////////
	GUID guid = CLEyeGetCameraUUID(deviceID);
	_cam = CLEyeCreateCamera(guid, colourMode, _resolution, (float)attemptFramerate);
	bool bOk = (_cam != NULL);
	if (bOk) bOk &= CLEyeCameraStart(_cam);
	////////////////////////////////////////

	if (bOk == true){
		CLEyeCameraGetFrameDimensions(_cam, width, height); 

		CLEyeSetCameraParameter(_cam, CLEYE_AUTO_GAIN, 1);
		CLEyeSetCameraParameter(_cam, CLEYE_AUTO_EXPOSURE, 0);
		CLEyeSetCameraParameter(_cam, CLEYE_AUTO_WHITEBALANCE, 1);

		//this line should also accomodate non-colour values
		//here we init an array at the cam's native resolution
		viPixels= new BYTE[width * height * (isGrey ? 1 : 4)];
		pixels	= new BYTE[width * height * (isGrey ? 1 : 3)];

		frame = cvCreateImageHeader(cvSize(width, height), 8, isGrey ? 1 : 3);
		return true;
	} else { 
		return false;
	}
}