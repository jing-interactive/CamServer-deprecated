#include "_hge.h"

#pragma comment(lib, "hge.lib")
#pragma comment(lib, "hgehelp.lib")

HGE *hge=0;
hgeParticleManager *particleManager = 0;

hgeVector mouse;
hgeVector prev_mouse;
hgeVector delta_mouse;
bool mouse_inside = true;
bool left_down = false;
bool left_click = false;
bool right_click = false;
bool middle_down = false;
bool middle_click = false;
int mouse_wheel = 0;

bool init_hge(int w, int h, hgeCallback FrameFunc, hgeCallback RenderFunc,
			  /*int bpp,*/ bool windowed)
{
	// Get HGE interface
	hge = hgeCreate(HGE_VERSION);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "运行信息.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "App @ vinjn");
	hge->System_SetState(HGE_USESOUND, true);
	hge->System_SetState(HGE_SHOWSPLASH, false);
	// Set up video mode
	hge->System_SetState(HGE_WINDOWED, windowed);
	hge->System_SetState(HGE_SCREENWIDTH, w);
	hge->System_SetState(HGE_SCREENHEIGHT, h);
//	hge->System_SetState(HGE_SCREENBPP, bpp);
	hge->System_SetState(HGE_FPS, 60);

	bool res =  hge->System_Initiate();

	particleManager = new hgeParticleManager();

	return res;
}

void release_hge()
{
	delete particleManager;
	hge->System_Shutdown();
	hge->Release();
}

int msg_box(TCHAR* info, UINT flag)
{
	return MessageBox(NULL, info, L"HGE", flag);
}


void show_last_error()
{
	MessageBoxA(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
}


void hgeVector_clamp(hgeVector& vec, float min, float max)
{ 
	float len = vec.Length();
	if(len > max)
	{ 
		vec.Normalize();
		vec.x *= max; vec.y *= max; 
	}
	if(len < min)
	{ 
		vec.Normalize();
		vec.x *= min; vec.y *= min; 
	}
}

void update_mouse()
{
	left_click = hge->Input_KeyDown(HGEK_LBUTTON);
	if (left_click)
	{
		left_down = !left_down;
	}
	middle_click = hge->Input_KeyDown(HGEK_MBUTTON);
	if (middle_click)
	{
		middle_down = !middle_down;
	}

	right_click = hge->Input_KeyDown(HGEK_RBUTTON);
	mouse_wheel = hge->Input_GetMouseWheel();
	mouse_inside = hge->Input_IsMouseOver();
	hge->Input_GetMousePos(&mouse.x,&mouse.y);

	delta_mouse = mouse - prev_mouse;

	prev_mouse = mouse;
}

bool isPointInsideBox(hgeVector& point, hgeVector& box1, hgeVector& box2)
{
	return point.x <= box2.x && point.y <= box2.y
		&& point.x >= box1.x && point.y >= box1.y;
}


void Gfx_RenderCross(hgeVector& pos, int size, DWORD color)
{
	hge->Gfx_RenderLine(pos.x- size, pos.y- size, pos.x+ size,pos.y+ size,color);
	hge->Gfx_RenderLine(pos.x- size, pos.y+ size, pos.x+ size,pos.y- size,color);
}

void Gfx_RenderPixel(hgeVector& pos, DWORD color)
{
	hge->Gfx_RenderLine(pos.x,pos.y,pos.x+1,pos.y+1);
}

float getAngle(float x, float y)
{
	return hgeVector(x,y).Angle();
} 

#ifdef HAVING_OPENCV

void IplImage_to_HTEXTURE(IplImage* img, HTEXTURE tex)
{
	if (!hge || !tex) return;

	DWORD* pixels = hge->Texture_Lock(tex, false);
	char* ardata = (char*)img->imageData;
	char* final_loc = ardata + img->imageSize;

	while(ardata < final_loc)
	{
		*pixels++ = color_t(ardata[2], ardata[1],ardata[0]);
		ardata += 3;
	}
	//memcpy(pixels, ardata, img->imageSize);
	hge->Texture_Unlock(tex);
}

#endif