#pragma once
//ofxCLeye
//by Elliot Woods, Kimchi and Chips 2010
//MIT license

//tested with openFrameworks 0.061 / Visual Studio 2008

#include "CLEyeMulticam.h"
#include <opencv2/opencv.hpp>

class ofxCLeye
{
	public:
		ofxCLeye();
		~ofxCLeye();
		static int 		listDevices();
		bool			getFrame();
		void			close();
		bool			init(int w, int h, bool isGrey = false);
		IplImage* frame;

	protected: 
		int	attemptFramerate;
		int deviceID;

		CLEyeCameraColorMode	colourMode;
		CLEyeCameraInstance		_cam;
		CLEyeCameraResolution	_resolution;

		BYTE* viPixels;
		BYTE* pixels;
		int width,height;
};