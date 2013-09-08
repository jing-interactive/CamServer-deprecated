#include "AppConfig.h"
#include <stdio.h>

using namespace std;
using cv::FileStorage;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

AppConfig theConfig;

AppConfig::AppConfig():CLIENT("localhost")
{
    breakpt = 0;
	PORT = 3333;
	fixed_back_mode = TRUE;
	tuio_mode = TRUE;
	face_track = FALSE;
	hand_track = FALSE;
	hull_mode = FALSE;
	gray_detect_mode = TRUE;
	minim_window = FALSE;
	delay_for_run= 0;

	bg_mode = DIFF_BG;
	n_diff_frames = 2;//normal diff mode, change according to frame-rate

	paramFlipX = 0;
	paramFlipY = 0;
	paramDark = 220;
	paramBright = 40;
	paramBlur1 = 35;
	paramBlur2 = 1;
	paramNoise = 0;
	paramMinArea = 1;
	paramMaxArea = 50; 

#ifdef _DEBUG
	log_file = true;
#else
	log_file = false;
#endif
};

string AppConfig::parse_args(int argc, char** argv)
{
	const char *keys =
	{
        "{@source   | 0         |the input source, could be camera_idx/video/picture}"
		"{delay     | 0         |delay ms}"
		"{client    | localhost |specify the client ip}"
		"{port      | 3333      |specify the client port}"
		"{log       | false     |write log to file}"
        "{breakpt   | 0         |memory leak detection}"
		"{help h    | false     |display this help text}"
	};

	cv::CommandLineParser args(argc, (const char**)argv, keys);
	if (args.get<bool>("help"))
		args.printMessage();

	fixed_back_mode = !args.get<bool>("a");
	CLIENT = args.get<string>("client");
	log_file = args.get<bool>("log");
	minim_window = args.get<bool>("minim");
	delay_for_run = args.get<int>("delay");
	PORT = args.get<int>("port");

    breakpt = args.get<int>("breakpt");

	return args.get<string>(0);
}

bool AppConfig::load_from(char* filename)
{
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
	{
		printf("%s does not exist, CamServer starts with default values.\n", filename);
		return false;
	}
	else
	{
#define READ_(id, var) fs[id]>>var
#define READ_FS(var) fs[#var]>>(var)

		READ_("corners0_x",corners[0].x);READ_("corners0_y",corners[0].y);
		READ_("corners1_x",corners[1].x);READ_("corners1_y",corners[1].y);
		READ_("corners2_x",corners[2].x);READ_("corners2_y",corners[2].y);
		READ_("corners3_x",corners[3].x);READ_("corners3_y",corners[3].y);
		READ_FS(paramFlipX);
		READ_FS(paramFlipY);
		READ_FS(paramDark);
		READ_FS(paramBright);
		READ_FS(paramBlur1);
		READ_FS(paramBlur2);
		READ_FS(face_track);
		READ_FS(paramMinArea);
		READ_FS(paramMaxArea);
		READ_FS(hull_mode);
		READ_FS(gray_detect_mode);
		READ_FS(bg_mode);
		READ_FS(tuio_mode);
		printf("%s loaded.\n", filename);

		return true;
	}
}

bool AppConfig::save_to(char* filename)
{
	FileStorage fs(filename, FileStorage::WRITE);
	if (!fs.isOpened())
	{
		printf("failed to open %s for writing.\n", filename);
		return false;
	}
	else
	{
#define WRITE_(id, var) fs<<id<<var
#define WRITE_FS(var) fs<<#var<<(var)

		WRITE_("corners0_x",corners[0].x);WRITE_("corners0_y",corners[0].y);
		WRITE_("corners1_x",corners[1].x);WRITE_("corners1_y",corners[1].y);
		WRITE_("corners2_x",corners[2].x);WRITE_("corners2_y",corners[2].y);
		WRITE_("corners3_x",corners[3].x);WRITE_("corners3_y",corners[3].y);
		WRITE_FS(paramFlipX);
		WRITE_FS(paramFlipY);
		WRITE_FS(paramDark);
		WRITE_FS(paramBright);
		WRITE_FS(paramBlur1);
		WRITE_FS(paramBlur2);
		WRITE_FS(face_track);
		WRITE_FS(paramMinArea);
		WRITE_FS(paramMaxArea);
		WRITE_FS(hull_mode);
		WRITE_FS(gray_detect_mode);
		WRITE_FS(bg_mode);
		WRITE_FS(tuio_mode);
		printf("%s saved.\n", filename);
		return true;
	}
}
