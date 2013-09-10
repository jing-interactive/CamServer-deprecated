#include "opencv2/core/core.hpp"
#include "opencv/highgui.h"

#define MAIN_WINDOW "Monitor"
#define PARAM_WINDOW "Param Panel"

#define PARAM_DARK 255
#define PARAM_BRIGHT 255
#define PARAM_BLUR1 80
#define PARAM_BLUR2 20
#define PARAM_NOISE 3 
#define PARAM_MAXAREA 200

namespace monitor_gui
{
	extern void* handle;
	void show(bool visible);
}

namespace param_gui
{
	extern void* handle;
	extern cv::Mat setting;
	extern bool is_changing_layout;
	void update();
	void show(bool visible);
	void init();

    void on_realbg(int = 0);
    void on_whitebg(int = 0);
    void on_blackbg(int = 0);
    void on_diffbg(int = 0);
}

