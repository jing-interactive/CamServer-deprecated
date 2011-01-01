/*
#include "irr.h"

struct MyApp: public ISimpleApp
{
	bool init_scene()
	{
		return true;
	}
};

int main()
{
	MyApp theApp;
	if (theApp.init(true,800,600))
		theApp.run();

	return 0;
}

*/
#pragma warning (disable: 4305)
#pragma warning (disable: 4786)
#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4819 )

#pragma once

#include <irrlicht.h>
#include <windows.h>

using namespace irr;
/*
using namespace core;
using namespace scene;
using namespace video;
*/

extern IrrlichtDevice* device;
extern scene::ISceneManager* smgr;
extern video::IVideoDriver* driver;
extern gui::IGUIEnvironment* env;
extern scene::ISceneCollisionManager* coll;
extern scene::IMeshManipulator* mm;
extern video::IGPUProgrammingServices* gpu;

inline IrrlichtDevice* get_device(){return device;}
inline scene::ISceneManager* get_smgr(){return smgr;}
inline video::IVideoDriver* get_driver(){return driver;}
inline gui::IGUIEnvironment* get_env(){return env;}
inline scene::ISceneCollisionManager* get_coll(){return coll;}
inline scene::IMeshManipulator* get_mm(){return mm;}
inline video::IGPUProgrammingServices* get_gpu(){return gpu;}

extern SKeyMap g_KeyMap[];

extern u32 timeElapsed;

#define PRINT(VEC)	printf("%s : %.1f,%.1f,%.1f\n", #VEC, VEC.X, VEC.Y, VEC.Z)
bool initIrrlicht(s32 w, s32 h, bool opengl = true, s32 bpp=32, bool fullscreen=false, bool stentil = false);
scene::ITerrainSceneNode* loadTerrain(const c8* heightmap, const core::vector3df& scale, const c8* tex=NULL, const c8* detail=NULL);
scene::IAnimatedMeshSceneNode* loadAnimatedMesh(const c8* mesh_file, const c8* tex=0);
scene::IMeshSceneNode* loadMesh(const c8* mesh_file, const c8* tex=0);
scene::IMeshSceneNode* loadOctMesh(const c8* mesh_file, const c8* tex=0, s32 minPolyPerNode = 512);
void setNodeNormalTexture(scene::ISceneNode* node, const c8* tex_normal);
 

scene::ISceneNode* loadSkyBox();
scene::ISceneNode* loadSkyDome(const c8* sky_tex);
void drawSphere(const core::vector3df& pos, f32 radius, const video::SColor& color);
core::position2di getCursorPosition();
void setCursorVisible(bool visible);

int msg_box(const c8* info, UINT flag = MB_OK);

bool isStringEmpty(const c8* str);

bool hitIrrSelector(scene::ITriangleSelector* selector, 
					core::vector3df pos, core::vector3df& new_pos);
 
// Enumeration for UP, DOWN, PRESSED and RELEASED key states. Also used for mouse button states. 
enum keyStatesENUM {UP, DOWN, PRESSED, RELEASED}; 

// Keyboard key states. 
extern keyStatesENUM keys[KEY_KEY_CODES_COUNT]; 

class SimpleEventReceiver : public IEventReceiver
{
public:
	SimpleEventReceiver();
    virtual bool OnEvent(const SEvent& event);
protected:
	virtual bool OnMouse(const SEvent::SMouseInput& MouseInput){ return false;};
	virtual bool OnGui(const SEvent::SGUIEvent& guiEvent){return false;};
	virtual bool OnKey(const SEvent::SKeyInput& KeyInput) = 0;//make OnKey a must have
	virtual void OnSpecialEvent(const SEvent& Event){return;};
};

template <typename T>
void _clamp(T& vec, T min, T max)
{
	if (vec < min)
		vec = min;
	if (vec > max)
		vec = max;		
}

inline void ISceneNode_add_position(scene::ISceneNode* node, const core::vector3df& delta)
{
	core::vector3df pos = node->getPosition();
	node->setPosition(pos + delta);
}

inline u32 getRealTime(){ return device->getTimer()->getRealTime();}
inline u32 getTime(){ return device->getTimer()->getTime();}
inline u32 getTimeElapsed(){ return timeElapsed;}

#define KEY_DOWN(vk_code) ((::GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((::GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

inline bool isKeyToggle(int vk, bool* keyStatus = NULL){
	//
	static bool isPrevKeyDown[256];
	bool k = KEY_DOWN(vk);
	if (k && !isPrevKeyDown[vk]){
		isPrevKeyDown[vk] = true;
		if (keyStatus != NULL)
			*keyStatus =! *keyStatus;//key status needs update
		return true;
	}
	if (!k){
		isPrevKeyDown[vk] = false;
	}
	return false;
}


struct ISimpleApp
{
	virtual bool init_scene() = 0;//implement me & me only :)
	virtual void update_scene(){};
	virtual void render_scene();

	bool init(bool opengl = true, int w=800, int h=600, video::SColor bg_clr = video::SColor(255,122,122,122),IEventReceiver* recv = NULL, bool stencil=false);

	~ISimpleApp();

	video::SColor _bg_clr;

	void run() ;

protected:
	virtual void release(){};
};

scene::ISceneNode* getNodeByCursor(s32 idMaskBit = 0);
scene::ISceneNode* getMoreByCursorBB(OUT core::vector3df* hitPoint=NULL, OUT core::triangle3df* hitTriangle = NULL, s32 idMaskBit = 0);
scene::ISceneNode* getMoreByCursor(OUT core::vector3df& hitPoint, scene::ITriangleSelector* selector);

void saveScreenShot();
void makeCamResponse(scene::ICameraSceneNode* camera, scene::ITerrainSceneNode* terrain,
					 const core::vector3df& radius,  const core::vector3df& gravity, 
					 const core::vector3df& translation);

scene::IBillboardSceneNode* create_anim_billboard(core::array<video::ITexture*>& tex_array, c8* fmt, u32 start, u32 end, s32 TimePerFrame, 
												  scene::ISceneNode* parent, core::dimension2df& size, core::vector3df& pos);

scene::ITriangleSelector* getSelector(scene::ITerrainSceneNode* terrain);
scene::ITriangleSelector* getSelector(scene::IAnimatedMeshSceneNode* node);
scene::ITriangleSelector* getSelector(scene::IMeshSceneNode* node);
scene::ITriangleSelector* getBBoxSelector(scene::ISceneNode* node);
scene::ITriangleSelector* getSelector(scene::IMeshSceneNode* node, s32 minimalPolysPerNode/*=32*/);

scene::ISceneNodeAnimator* setNodeFlyStraight(scene::ISceneNode* node, const core::vector3df& start, const core::vector3df& end, u32 timeForWay);

scene::ICameraSceneNode* addCameraFPS(f32 rotateSpeed = 100.0f, f32 moveSpeed = 1.0f, bool onTerrain = false, f32 jumpSpeed = 0.0f);


scene::ISceneNodeAnimatorCollisionResponse* addNodeCollision(scene::ISceneNode* node, scene::ITriangleSelector* selector);
scene::ISceneNodeAnimatorCollisionResponse* addNodeCollision(scene::ISceneNode* node, scene::ITriangleSelector* selector, 
									  const core::vector3df& radius);
scene::ISceneNodeAnimatorCollisionResponse* addNodeCollision(scene::ISceneNode* node, scene::ITriangleSelector* selector, 
									  const core::vector3df& radius,  
									  const core::vector3df& gravity,  
									  const core::vector3df& translation);
scene::ISceneNodeAnimatorCollisionResponse* addNodeCollision(scene::ISceneNode* node, scene::ITriangleSelector* selector, 
									  const core::vector3df& radius,  
									  const core::vector3df& gravity,  
									  const core::vector3df& translation, 
									  f32 slidingSpeed);

core::vector3df matrixRotation(core::vector3df rotation, core::vector3df vec);

f32 getNodeAbove(scene::ISceneNode* node,  scene::ITriangleSelector* sel, f32 height);

inline bool fakeEquals(const core::vector3df& v1, const core::vector3df& v2)
{
    return core::equals(v1.X, v2.X) && core::equals(v1.Z, v2.Z);
}

inline f32 fakeDistance(const core::vector3df& v1, const core::vector3df& v2)
{
    f32 dx = v1.X-v2.X;
    f32 dz = v1.Z-v2.Z;

    return sqrt(dx*dx+dz*dz);
}

inline void ISceneNode_scale(scene::ISceneNode* node, f32 k)
{
	node->setScale(core::vector3df(k,k,k));
}

inline core::vector3df ISceneNode_getRadius(scene::ISceneNode* node)
{
	return node->getBoundingBox().getExtent()*0.5;
}

void fpsOn(int fps);
void fpsOff(void);

s32 addHighLevelShader(const c8* shader_file, const c8* vs_entry, const c8* ps_entry = NULL,
					   video::IShaderConstantSetCallBack* callback = NULL, 
					   video::E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID);

#ifdef HAVING_OPENCV
#include "cv.h"
void IplImage_to_ITexture(IplImage* img, ITexture* tex);
#endif


