#include <opencv/cv.h>

#define MAIN_WINDOW "Monitor"
#define PARAM_WINDOW "Param Panel"

#define PARAM_DARK 253
#define PARAM_BRIGHT 253
#define PARAM_AUTO 40
#define PARAM_BLUR1 80
#define PARAM_BLUR2 7
#define PARAM_NOISE 3 
#define PARAM_MAXAREA 1000

namespace monitor_gui
{
	void show(bool visible);
}

namespace param_gui
{
	extern cv::Ptr<IplImage> setting;
	void update();
	void show(bool visible);
	void init();

	void on_realbg(int);
}

