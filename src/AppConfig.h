#pragma once

//////////////////////////////////////////////////////////////////////////
#define VERSION "0.7.0 [Viva OpenGL] "
#undef FACE_DETECTION_ENABLED
#undef VIDEO_INPUT_LIB_USED
//////////////////////////////////////////////////////////////////////////

#include "opencv2/core/utility.hpp"

#include <string>

enum BackGroundMode
{
    REAL_BG,    //real time background
    WHITE_BG,   //white
    BLACK_BG,   //black
    DIFF_BG,    //diff 
};

#define CONFIG_FILE "CamServer.config.xml"

struct AppConfig
{
    AppConfig();

    //return the input_source
    std::string parse_args(int argc, char** argv);
    bool load_from(char* filename);
    bool save_to(char* filename);

    int breakpt;
    int bg_mode;

    std::string CLIENT;
    int PORT;
    int fixed_back_mode;	
    int tuio_mode;
    int face_track;
    int hand_track;
    int hull_mode;
    int gray_detect_mode;
    int minim_window;
    int delay_for_run; 
    int paramFlipX;
    int paramFlipY;
    int paramDark;
    int paramBright;
    int paramBlur1;
    int paramBlur2;
    int paramNoise;
    int paramMinArea;
    int paramMaxArea;
    int n_diff_frames;

    bool log_file;

    cv::Point2f cornersA[4];
    cv::Point2f cornersB[4];

    int scene_plot_mode;    // for Nike-flex
    std::vector<cv::Point2f> scene_plots;
};

extern AppConfig theConfig;