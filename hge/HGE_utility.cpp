#include "HGE_utility.h"

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
			  int bpp, bool windowed)
{
	// Get HGE interface
	hge = hgeCreate(HGE_VERSION);

	// Set up log file, frame function, render function and window title
	hge->System_SetState(HGE_LOGFILE, "运行信息.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "天平模拟 @ vinjn");
	hge->System_SetState(HGE_USESOUND, false);

	// Set up video mode
	hge->System_SetState(HGE_WINDOWED, windowed);
	hge->System_SetState(HGE_SCREENWIDTH, w);
	hge->System_SetState(HGE_SCREENHEIGHT, h);
	hge->System_SetState(HGE_SCREENBPP, bpp);
	hge->System_SetState(HGE_FPS, 60);

	bool res =  hge->System_Initiate();

	particleManager = new hgeParticleManager();

#ifdef USING_GUICHAIN
	init_gui(w, h);
#endif
	return res;
}

void release_hge()
{
#ifdef USING_GUICHAIN
	release_gui();
#endif
	delete particleManager;
	hge->System_Shutdown();
	hge->Release();
}

int msg_box(char* info, UINT flag)
{
	return MessageBoxA(NULL, info, "HGE", flag);
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


#ifdef USING_GUICHAIN

gcn::Gui* gui = NULL;
gcn::HGEGraphics* graphics = NULL;
gcn::HGEInput* input = NULL;
gcn::HGEImageLoader* imageLoader = NULL;

gcn::Container* top = NULL;
gcn::ImageFont* fontGui = NULL;

void init_gui(int w, int h)
{
    imageLoader = new gcn::HGEImageLoader();
    // The ImageLoader Guichan should use needs to be passed to the Image object
    // using a static function.
    gcn::Image::setImageLoader(imageLoader);
    graphics = new gcn::HGEGraphics();
    input = new gcn::HGEInput();

    // Now we create the Gui object to be used with this HGE application.
    gui = new gcn::Gui();
    // The Gui object needs a Graphics to be able to draw itself and an Input
    // object to be able to check for user input. In this case we provide the
    // Gui object with HGE implementations of these objects hence making Guichan
    // able to utilise HGE.
    gui->setGraphics(graphics);
    gui->setInput(input);

    top = new gcn::Container();
    // We set the dimension of the top container to match the screen.
    top->setDimension(gcn::Rectangle(0, 0, w, h));
    // Finally we pass the top widget to the Gui object.
    gui->setTop(top);

    //[font]
    fontGui = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    // Widgets may have a global font so we don't need to pass the
    // font object to every created widget. The global font is static.
    gcn::Widget::setGlobalFont(fontGui);
}

void update_gui()
{
	gui->logic();
}

void render_gui()
{
	gui->draw();
}

void release_gui()
{
    SAFE_DELETE(gui);
	SAFE_DELETE(fontGui);
	SAFE_DELETE(imageLoader);
	SAFE_DELETE(input);
	SAFE_DELETE(graphics);
}

#endif



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

void Gfx_RenderRect(hgeRect& rect, DWORD color)
{
	hge->Gfx_RenderLine(rect.x1, rect.y1, rect.x1, rect.y2, color);
	hge->Gfx_RenderLine(rect.x1, rect.y2, rect.x2, rect.y2, color);
	hge->Gfx_RenderLine(rect.x2, rect.y2, rect.x2, rect.y1, color);
	hge->Gfx_RenderLine(rect.x2, rect.y1, rect.x1, rect.y1, color);
}


void Gfx_RenderPixel(hgeVector& pos, DWORD color)
{
	hge->Gfx_RenderLine(pos.x,pos.y,pos.x+1,pos.y+1);
}

float getAngle(float x, float y)
{
	return hgeVector(x,y).Angle();
} 