#pragma once

//#define USING_GUICHAIN

#include <hge/hge.h>
#include <hge/hgesprite.h>
#include <hge/hgefont.h>
#include <hge/hgeparticle.h>
#include <hge/hgeVector.h>
#include <hge/hgedistort.h>
#include <hge/hgeresource.h>

#pragma comment(linker, "/NODEFAULTLIB:LIBCD.LIB") 

extern HGE *hge;
extern hgeParticleManager *particleManager;

bool init_hge(int w, int h, hgeCallback FrameFunc, hgeCallback RenderFunc,
			  /*int bpp = 32,*/ bool windowed  = true);
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

struct color_t
{
	//color_t(const DWORD argb = 0xFF000000){
	//	_argb = argb;
	//}
	color_t( BYTE gray ){
		_r = _g = _b = gray;
		_a = 0xFF;
	} 
	color_t( BYTE r, BYTE g, BYTE b, BYTE a = 0xFF ){
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	} 
	operator DWORD() const{
		return _argb;
	}
	union 
    {
		DWORD _argb;                  // compressed format
		BYTE _argb_M[4];             // array format
		struct {  BYTE _b,_g,_r,_a;  }; // explict name format
    }; 
};

inline int random_win_x(){return hge->Random_Int(0, win_width);} 
inline int random_win_y(){return hge->Random_Int(0, win_height);} 

void hgeVector_clamp(hgeVector& vec, float min, float max);

bool isPointInsideBox(hgeVector& point, hgeVector& box1, hgeVector& box2);

float getAngle(float x, float y);
 
#ifdef HAVING_OPENCV
#include "cv.h"
void IplImage_to_HTEXTURE(IplImage* img, HTEXTURE tex);
#endif