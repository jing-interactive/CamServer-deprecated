/*
#include "irr_utility.h"

struct MyApp: public ISimpleApp
{
	bool init_scene()
	{
		return true;
	}

	void update_scene()
	{

	}

	void render_scene()
	{

	}

};

int main()
{
	MyApp theApp;
	if (theApp.init(800,600))
		theApp.run();

	return 0;
}

*/
#pragma once

#define USING_NEWTON
//#define FPS 1000/30

#include <irrlicht.h>
#include <windows.h>

using namespace irr;
using namespace gui;
using namespace core;
using namespace video;
using namespace scene;

#pragma comment(lib,"irrlicht.lib")


extern IrrlichtDevice* device;
extern scene::ISceneManager* smgr;
extern video::IVideoDriver* driver;
extern gui::IGUIEnvironment* env;
extern scene::ISceneCollisionManager* col;


#define PRINT(VEC)	printf("%s : %.1f,%.1f,%.1f\n", #VEC, VEC.X, VEC.Y, VEC.Z)
bool initIrrlicht(s32 w, s32 h, s32 bpp=32, bool fullscreen=false);
scene::ITerrainSceneNode* loadTerrain();
scene::IAnimatedMeshSceneNode* loadAnimatedMesh(const c8* mesh_file, const c8* tex=0, const c8* tex_normal=0);
scene::IMeshSceneNode* loadMesh(const c8* mesh_file, const c8* texture=0);

void drawSphere(const vector3df& pos, f32 radius, const SColor& color);

void do_special_event(const SEvent& Event);
bool OnMouse(const SEvent::SMouseInput& MouseInput);
bool OnGui(const SEvent::SGUIEvent& guiEvent);
bool OnKeyDown(const SEvent::SKeyInput& KeyInput);

int msg_box(char* info, UINT flag);

#ifdef USING_NEWTON

	#include <irrnewt.hpp>
	#pragma comment(lib, "IrrNewt.lib") 
	extern newton::IWorld* world;

	newton::IBody* createEllipsoid(const c8* tex, f32 mass,bool gravity = true);
	newton::IBody* createCube(const c8* tex, f32 size, f32 mass, bool gravity = true);

	bool hitNewtonBody(newton::IBody* body, 
						core::vector3df pos, core::vector3df& new_pos);
	newton::ICameraBodyFPS* create_newton_camera(ICameraSceneNode* cam, vector3df& box_size, f32 rot, f32 speed);
	
	vector3df getTargetRay(s32 x, s32 y, newton::IBody* body = 0);

	newton::IBody* create_newton_level(IMeshSceneNode* node, IMesh* mesh = 0);
	newton::IBody* create_newton_body(IMeshSceneNode* node, IMesh* mesh = 0);

					
#endif

	bool hitIrrSelector(scene::ITriangleSelector* selector, 
						core::vector3df pos, core::vector3df& new_pos);

extern bool keys[KEY_KEY_CODES_COUNT];

class SimpleEventReceiver : public IEventReceiver
{
public:
    virtual bool OnEvent(const SEvent& event)
    {
		do_special_event(event);

        if (event.EventType == irr::EET_GUI_EVENT)
        {
            return OnGui(event.GUIEvent);
        }

        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
        {
            keys[event.KeyInput.Key] = event.KeyInput.PressedDown;

			if (event.KeyInput.PressedDown)
			{
				return OnKeyDown(event.KeyInput);
			}
        }
		
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            return OnMouse(event.MouseInput);
        }
        return false;
    }
};

template <typename T>
void _clamp(T& vec, T min, T max)
{
	if (vec < min)
		vec = min;
	if (vec > max)
		vec = max;		
}

inline void ISceneNode_add_position(ISceneNode* node, const vector3df& delta)
{
	vector3df pos = node->getPosition();
	node->setPosition(pos + delta);
}


inline u32 getTime(){ return device->getTimer()->getTime();}

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)



struct ISimpleApp
{
	virtual bool init_scene() = 0;
	virtual void update_scene() = 0;
	virtual void render_scene() = 0;

	bool init(int w, int h, SColor bg_clr = SColor(255,122,122,122), IEventReceiver* recv = NULL)
	{
		_bg_clr = bg_clr;
		if (initIrrlicht(w,h) && init_scene())
		{
			if (recv != NULL)
				device->setEventReceiver(recv);
			return true;
		}
		return false;
	}

	~ISimpleApp()
	{
		
	}

	SColor _bg_clr;

	void run()
	{
		while (device->run())
			if (device->isWindowActive())
			{
#ifdef FPS
				u32 begin = getTime();
#endif
				update_scene();
#ifdef USING_NEWTON
				world->update();
#endif			

				driver->beginScene(true, true, _bg_clr);
				smgr->drawAll();
				render_scene();
				env->drawAll();

				driver->endScene();

#ifdef FPS

				u32 time_cost = getTime() - begin;
				if (time_cost < FPS)
				{
					::Sleep(FPS-time_cost);
				}
#endif
				
			}
	}
};




IBillboardSceneNode* create_anim_billboard(array<ITexture*>& tex_array, char* fmt, u32 start, u32 end, s32 timePerFrame, 
	ISceneNode* parent, dimension2df& size, vector3df& pos);


