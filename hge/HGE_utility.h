#pragma once

//#define USING_GUICHAIN

#include <hge.h>
#include <hgesprite.h>
#include <hgefont.h>
#include <hgeparticle.h>
#include <hgeVector.h>
#include <hgedistort.h>
#include <hgeresource.h>

#pragma comment(lib, "hge.lib")
#pragma comment(lib, "hgehelp.lib")

#pragma comment(linker, "/NODEFAULTLIB:LIBCD.LIB") 

extern HGE *hge;
extern hgeParticleManager *particleManager;

bool init_hge(int w, int h, hgeCallback FrameFunc, hgeCallback RenderFunc,
			  int bpp = 32, bool windowed  = true);
void release_hge();

int msg_box(char* info, UINT flag = MB_OK);
void show_last_error();

void update_mouse();

extern hgeVector mouse;
extern hgeVector prev_mouse;
extern hgeVector delta_mouse;
extern bool mouse_inside;
extern bool left_down;
extern bool left_click;
extern bool right_click;
extern bool middle_down;
extern bool middle_click;
extern int mouse_wheel;

extern const int win_width;
extern const int win_height;

#define RANDOM_DEGREE hge->Random_Float(0,2*M_PI)

void Gfx_RenderCross(hgeVector& pos, int size, DWORD color);
void Gfx_RenderPixel(hgeVector& pos, DWORD color);
void Gfx_RenderRect(hgeRect& rect, DWORD color);

template <typename T>
void _clamp(T& vec, T min, T max)
{
	if (vec < min)
		vec = min;
	if (vec > max)
		vec = max;		
}

inline int random_win_x(){return hge->Random_Int(0, win_width);} 
inline int random_win_y(){return hge->Random_Int(0, win_height);} 

void hgeVector_clamp(hgeVector& vec, float min, float max);

bool isPointInsideBox(hgeVector& point, hgeVector& box1, hgeVector& box2);

float getAngle(float x, float y);

#ifdef USING_GUICHAIN
	#include <guichan.hpp>
	#include <guichan/hge.hpp>
	#ifdef _DEBUG
		#pragma comment(lib, "guichain_d.lib")
	#else
		#pragma comment(lib, "guichain.lib")
	#endif

extern gcn::Gui* gui;
extern gcn::HGEGraphics* graphics;
extern gcn::HGEInput* input;
extern gcn::HGEImageLoader* imageLoader;

extern gcn::Container* top;
extern gcn::ImageFont* fontGui;

void init_gui(int w, int h);
void update_gui();
void render_gui();
void release_gui();

#endif