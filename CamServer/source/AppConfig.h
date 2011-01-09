#pragma once

#include <cxcore.h>
#include <string>

struct AppConfig
{
	AppConfig();
	void parse_args(int argc, char** argv);
	bool load_from(char* filename);
	bool save_to(char* filename);

	std::string CLIENT;
	int PORT;
	int fixed_back_mode;	
	int detailed_mode ;
	int face_track;
	int hand_track;
	int finger_track;
	int hull_mode;
	int gray_detect_mode;
	int minim_window;
	int delay_for_run; 
	int paramFlipX;
	int paramFlipY;
	int paramDark;
	int paramBright;
	int paramAuto;
	int paramBlur1;
	int paramBlur2;
	int paramNoise;
	int paramMinArea;
	int paramMaxArea;

	int auto_explosure;
	bool no_osc;	
	bool isDemo;

	cv::Point2f corners[4];
};

extern AppConfig theConfig;