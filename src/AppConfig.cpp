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
    scene_plot_mode = false;
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

		READ_("cornersA0_x",cornersA[0].x);READ_("cornersA0_y",cornersA[0].y);
		READ_("cornersA1_x",cornersA[1].x);READ_("cornersA1_y",cornersA[1].y);
		READ_("cornersA2_x",cornersA[2].x);READ_("cornersA2_y",cornersA[2].y);
		READ_("cornersA3_x",cornersA[3].x);READ_("cornersA3_y",cornersA[3].y);

        READ_("cornersB0_x",cornersB[0].x);READ_("cornersB0_y",cornersB[0].y);
        READ_("cornersB1_x",cornersB[1].x);READ_("cornersB1_y",cornersB[1].y);
        READ_("cornersB2_x",cornersB[2].x);READ_("cornersB2_y",cornersB[2].y);
        READ_("cornersB3_x",cornersB[3].x);READ_("cornersB3_y",cornersB[3].y);

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

        WRITE_("cornersA0_x",cornersA[0].x);WRITE_("cornersA0_y",cornersA[0].y);
        WRITE_("cornersA1_x",cornersA[1].x);WRITE_("cornersA1_y",cornersA[1].y);
        WRITE_("cornersA2_x",cornersA[2].x);WRITE_("cornersA2_y",cornersA[2].y);
        WRITE_("cornersA3_x",cornersA[3].x);WRITE_("cornersA3_y",cornersA[3].y);

        WRITE_("cornersB0_x",cornersB[0].x);WRITE_("cornersB0_y",cornersB[0].y);
        WRITE_("cornersB1_x",cornersB[1].x);WRITE_("cornersB1_y",cornersB[1].y);
        WRITE_("cornersB2_x",cornersB[2].x);WRITE_("cornersB2_y",cornersB[2].y);
        WRITE_("cornersB3_x",cornersB[3].x);WRITE_("cornersB3_y",cornersB[3].y);

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
