#pragma once

#include <opencv2/core/core.hpp>
#include <string>

#define		REAL_BG	0		//real time background
#define		WHITE_BG 1		//white
#define		BLACK_BG 2		//black
#define		DIFF_BG	3		//diff 

#define CONFIG_FILE "CamServer.config.xml"

#define VERSION "0.7.0 [Viva OpenGL] "

struct AppConfig
{
	AppConfig();

	//return the input_source
	std::string parse_args(int argc, char** argv);
	bool load_from(char* filename);
	bool save_to(char* filename);

	int bg_mode;

	std::string CLIENT;
	int PORT;
	int fixed_back_mode;	
	int tuio_mode;
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
	int n_diff_frames;

	bool log_file;

	cv::Point2f corners[4];
};

extern AppConfig theConfig;