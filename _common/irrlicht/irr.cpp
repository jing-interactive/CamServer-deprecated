#include "irr.h"

#ifdef _DEBUG
#pragma comment(lib,"irrlicht_d.lib")
#else
#pragma comment(lib,"irrlicht.lib")
#endif

using namespace gui;
using namespace core;
using namespace video;
using namespace scene;

IrrlichtDevice* device = 0;
scene::ISceneManager* smgr = 0;
video::IVideoDriver* driver = 0;
gui::IGUIEnvironment* env = 0;
scene::ISceneCollisionManager* coll = 0;
scene::IMeshManipulator* mm = 0;
video::IGPUProgrammingServices* gpu = 0;

SKeyMap g_KeyMap[6];

bool fps_on = false;
u32 timePerFrame = 1000/60;
u32  timeElapsed = 0; 
void fpsOn(int fps)
{
	fps_on = true;
	timePerFrame = 1000/fps;
}
void fpsOff(void)
{
	timePerFrame = false;
} 

keyStatesENUM keys[KEY_KEY_CODES_COUNT]; 

bool initIrrlicht(s32 w, s32 h, bool opengl, s32 bpp, bool fullscreen, bool stentil)
{
	SIrrlichtCreationParameters param;
	param.WindowSize = core::dimension2d<u32>(w, h);
	param.AntiAlias = true;
	param.Fullscreen = fullscreen;
	param.Bits = bpp;
	param.Stencilbuffer = stentil;
	param.DriverType = opengl ? EDT_OPENGL : EDT_DIRECT3D9;

#if 1
	device = createDeviceEx(param);
#else
	device = createDevice(EDT_OPENGL, core::dimension2d<u32>(w, h),
		bpp,fullscreen, false);//EDT_DIRECT3D9
#endif	
	if (device)
	{
		driver = device->getVideoDriver();
		env = device->getGUIEnvironment();
		//driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT,true);
		smgr = device->getSceneManager();
		coll = smgr->getSceneCollisionManager();
		mm = smgr->getMeshManipulator();
		gpu = driver->getGPUProgrammingServices();

		//setCursorVisible(false);

		g_KeyMap[0].Action = EKA_MOVE_FORWARD;
		g_KeyMap[0].KeyCode = KEY_KEY_W;
		g_KeyMap[1].Action = EKA_MOVE_BACKWARD;
		g_KeyMap[1].KeyCode = KEY_KEY_S;
		g_KeyMap[2].Action = EKA_STRAFE_LEFT;
		g_KeyMap[2].KeyCode = KEY_KEY_A;
		g_KeyMap[3].Action = EKA_STRAFE_RIGHT;
		g_KeyMap[3].KeyCode = KEY_KEY_D;
		g_KeyMap[4].Action = EKA_JUMP_UP;
		g_KeyMap[4].KeyCode = KEY_SPACE;
		g_KeyMap[5].Action = EKA_CROUCH;
		g_KeyMap[5].KeyCode = KEY_CONTROL;

		return true;
	}
	else
		return false;
}

int msg_box(const c8* info, UINT flag)
{
	return MessageBoxA(NULL, info, "irrlicht", flag);
}

scene::ITerrainSceneNode* loadTerrain(const c8* heightmap, const vector3df& scale, const c8* tex, const c8* detail)
{
//	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(heightmap);
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
		heightmap,
		0,					                // parent node
		-1,					                // node id
		core::vector3df(0.f, 0.f, 0.f),		// position
		core::vector3df(0.f, 0.f, 0.f),		// rotation
		core::vector3df(1.f, 1.f, 1.f),	// scale
		video::SColor(255, 255, 255, 255),	// vertexColor
		5,					                // maxLOD
		scene::ETPS_17,				        // patchSize
		4					                // smoothFactor
		);

	if (terrain)
	{
		terrain->setScale(scale);
		terrain->setName(heightmap);
		terrain->setMaterialFlag(EMF_LIGHTING, false);
		if (tex)
			terrain->setMaterialTexture(0, driver->getTexture(tex));
		if (detail)
			terrain->setMaterialTexture(1, driver->getTexture(detail));

		terrain->setMaterialType(EMT_DETAIL_MAP);

		terrain->scaleTexture(1.0f, 50.0f);
	}
	return terrain;
}


scene::IAnimatedMeshSceneNode* loadAnimatedMesh(const c8* mesh_file, const c8* tex)
{
	scene::IAnimatedMesh* mesh = smgr->getMesh(mesh_file);
	scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (node)
	{
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		node->setName(mesh_file);
		if (!isStringEmpty(tex))
		{
			node->setMaterialTexture( 0, driver->getTexture(tex) );
		}
	}
	return node;
}


scene::IMeshSceneNode* loadMesh(const c8* mesh_file, const c8* tex)
{
	scene::IMesh* mesh = smgr->getMesh(mesh_file);
	scene::IMeshSceneNode* node = smgr->addMeshSceneNode( mesh );

	if (node)
	{
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		node->setName(mesh_file);
		if (!isStringEmpty(tex))
			node->setMaterialTexture( 0, driver->getTexture(tex) );
	}
	return node;
}


IMeshSceneNode* loadOctMesh(const c8* mesh_file, const c8* tex, s32 minPolyPerNode)
{
	scene::IMesh* mesh = smgr->getMesh(mesh_file);
	scene::IMeshSceneNode* node = smgr->addOctTreeSceneNode( mesh, NULL, -1, minPolyPerNode );

	if (node)
	{
		node->setMaterialFlag(video::EMF_LIGHTING, false);
		node->setName(mesh_file);
		if (!isStringEmpty(tex))
			node->setMaterialTexture( 0, driver->getTexture(tex) );
	}
	return node;
}

void setNodeNormalTexture(scene::ISceneNode* node, const c8* tex_normal)
{
	if (node && tex_normal)
	{
		node->setMaterialTexture( 1, driver->getTexture(tex_normal) );
		node->setMaterialType(EMT_NORMAL_MAP_SOLID);
	}
}

scene::ISceneNode* loadSkyBox()
{
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	scene::ISceneNode* skybox=smgr->addSkyBoxSceneNode(
		driver->getTexture("sky/irrlicht2_up.jpg"),
		driver->getTexture("sky/irrlicht2_dn.jpg"),
		driver->getTexture("sky/irrlicht2_lf.jpg"),
		driver->getTexture("sky/irrlicht2_rt.jpg"),
		driver->getTexture("sky/irrlicht2_ft.jpg"),
		driver->getTexture("sky/irrlicht2_bk.jpg"));
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	return skybox;
}

scene::ISceneNode* loadSkyDome(const c8* sky_tex)
{
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	scene::ISceneNode* skydome=smgr->addSkyDomeSceneNode(driver->getTexture(sky_tex),16,8,0.95f,2.0f);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	return skydome;
}

bool hitIrrSelector(scene::ITriangleSelector* selector, 
					core::vector3df pos, core::vector3df& new_pos)
{
	core::line3d<f32> ray(pos, pos);
	ray.start.Y += 9999;
	ray.end.Y -= 9999;

	core::vector3df hitPt;
	core::triangle3df hitTri; 

	ISceneNode* node =  coll->getSceneNodeAndCollisionPointFromRay(ray, hitPt, hitTri);

	bool hit = node != NULL;
	return hit;	
}


void drawSphere(const vector3df& pos, f32 radius, const SColor& color)
{
	static IAnimatedMesh* sphereMesh = 0;

	if (sphereMesh == 0)
	{
		sphereMesh = smgr->addSphereMesh("sphereMesh", 1.0f);
	}

	const matrix4 oldWorldMat = driver->getTransform(ETS_WORLD);

	matrix4 worldMat;

	worldMat.setTranslation(pos);
	worldMat.setScale(vector3df(radius, radius, radius));
	driver->setTransform(ETS_WORLD, worldMat);

	IMesh* mesh = sphereMesh->getMesh(0);

	//driver->setMaterial(mesh->getMeshBuffer(0)->getMaterial());

	SMaterial material;

	material.AmbientColor = color;
	material.DiffuseColor = color;
	material.Lighting = true;

	//material.
	driver->setMaterial(material);

	driver->drawMeshBuffer(mesh->getMeshBuffer(0));

	//to restore
	driver->setTransform(ETS_WORLD, oldWorldMat);
}


IBillboardSceneNode* create_anim_billboard(array<ITexture*>& tex_array, char* fmt, u32 start, u32 end, s32 TimePerFrame, 
										   ISceneNode* parent, dimension2df& size, vector3df& pos)
{
	char tmp[64];
	for (u32 d=start; d<end+1; d++)
	{
		sprintf(tmp, fmt, d);
		ITexture* t = driver->getTexture(tmp);
		tex_array.push_back(t);
	}
	scene::ISceneNodeAnimator* anim = 0;
	anim = smgr->createTextureAnimator(tex_array, TimePerFrame);

	// create sprite
	IBillboardSceneNode* bill = 0;
	bill = smgr->addBillboardSceneNode(parent, size, pos);
	bill->setMaterialFlag(EMF_LIGHTING, true);
	bill->setMaterialFlag(EMF_ZBUFFER, true);
	sprintf(tmp, fmt, 0);
	bill->setMaterialTexture(0, driver->getTexture(tmp));
	bill->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
	bill->addAnimator(anim);
	anim->drop();

	return bill;
}



bool isStringEmpty(const char* str)
{
	return str == NULL || str[0] == '\0';
}


bool ISimpleApp::init( bool opengl /*= true*/, int w/*=800*/, int h/*=600*/, SColor bg_clr /*= SColor(255,122,122,122)*/,IEventReceiver* recv /*= NULL*/, bool stencil/*=false*/ )
{
	_bg_clr = bg_clr;
	if (initIrrlicht(w,h,opengl,16,false, stencil) && init_scene())
	{
		if (recv != NULL)
			device->setEventReceiver(recv);
		return true;
	}
	return false;
}

void ISimpleApp::render_scene()
{
	static s32 oldFps = 0;
	u32 fps = driver->getFPS();
	if(oldFps != fps)
	{		
		oldFps = fps;
		u32 nPolygons = driver->getPrimitiveCountDrawn();
		wchar_t info[256];
		swprintf(info, L"fps: %d, poly: %d", oldFps, nPolygons);
		device->setWindowCaption(info);
	}
}

ISimpleApp::~ISimpleApp()
{
	device->drop();
}

void ISimpleApp::run()
{ 
	while (device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, _bg_clr);
			u32 start = getRealTime();
			
			update_scene();
			smgr->drawAll();			
			render_scene();
			env->drawAll();

			driver->endScene(); 						
			timeElapsed = getRealTime() - start;

			if (fps_on)
			{				
				if (timeElapsed< timePerFrame)
				{
					device->sleep(timePerFrame-timeElapsed);
				} 
			}
		}
		else
		{
		    // if the window is inactive, we should pause the device and release the mouse
		    device->yield();
		}
 	}
	release();
}


void saveScreenShot()
{
	static int number = 0;
	IImage* shot = driver->createScreenShot(); 
	if (shot)
	{
		c8 buf[256];
		snprintf(buf, 256, "snapshot/shot%04d.jpg",                                
			++number);
		if (driver->writeImageToFile(shot, buf, 85 ))
			printf("%s saved\n", buf);
		shot->drop();
	}
}


ISceneNodeAnimatorCollisionResponse* _addNodeCollision(ISceneNode* node, ITriangleSelector* selector, 
									  const vector3df& radius = vector3df(30,60,30),  
									  const vector3df& gravity = vector3df(0,-10,0),  
									  const vector3df& translation = vector3df(), 
									  f32 slidingSpeed = 0.0005f )
{
	scene::ISceneNodeAnimatorCollisionResponse* anim = smgr->createCollisionResponseAnimator(
	//	new CSceneNodeAnimatorCollisionResponseX(smgr,
		selector, node, 
		radius,	gravity,
		translation, slidingSpeed);
	node->addAnimator(anim);
	anim->drop();

	return anim;
}


ISceneNodeAnimatorCollisionResponse* addNodeCollision(ISceneNode* node, ITriangleSelector* selector)
{
	vector3df radius = node->getBoundingBox().getExtent()*0.5;
	return _addNodeCollision(node, selector, radius);
}

ISceneNodeAnimatorCollisionResponse* addNodeCollision(ISceneNode* node, ITriangleSelector* selector, 
									  const vector3df& radius)
{
	return _addNodeCollision(node, selector, radius);
}

ISceneNodeAnimatorCollisionResponse* addNodeCollision(ISceneNode* node, ITriangleSelector* selector, 
									  const vector3df& radius,  const vector3df& gravity,  const vector3df& translation)
{
	return _addNodeCollision(node, selector, radius,gravity,translation);
}

ISceneNodeAnimatorCollisionResponse* addNodeCollision(ISceneNode* node, ITriangleSelector* selector, 
									  const vector3df& radius,  const vector3df& gravity,  const vector3df& translation,
									  f32 slidingSpeed)
{
	return _addNodeCollision(node, selector, radius,gravity,translation,slidingSpeed);
}

void makeCamResponse(ICameraSceneNode* camera, ITerrainSceneNode* terrain,
					 const vector3df& radius,  const vector3df& gravity, 
					 const vector3df& translation)
{
	scene::ITriangleSelector* selector
		= smgr->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);

	// create collision response animator and attach it to the camera
	addNodeCollision(camera, selector, radius, gravity, translation);
	selector->drop();
}


position2di getCursorPosition()
{
	return device->getCursorControl()->getPosition();
}

void setCursorVisible(bool visible)
{
	return device->getCursorControl()->setVisible(visible);
}



ISceneNode* getNodeByCursor(s32 idMaskBit/* = 0*/)
{
	vector2di cursor_pos = getCursorPosition();

	return coll->getSceneNodeFromScreenCoordinatesBB(cursor_pos, idMaskBit);
}


ISceneNode* getMoreByCursorBB(OUT vector3df* hitPoint/* = NULL*/, core::triangle3df* hitTriangle/* = NULL*/, s32 idMaskBit/* = 0*/)
{
	vector2di cursor_pos = getCursorPosition();
	line3df ray = coll->getRayFromScreenCoordinates(cursor_pos);

	core::vector3df pt;
	core::triangle3df tri;
	ISceneNode* hitNode = coll->getSceneNodeAndCollisionPointFromRay(								
		ray,	pt,
		tri,//i dont need it however
		idMaskBit,	 NULL);

	if (hitPoint)
		*hitPoint = pt;

	if (hitTriangle)
		*hitTriangle = tri;

	return hitNode;
}

ISceneNode* getMoreByCursor(OUT vector3df& hitPoint, ITriangleSelector* selector)
{
	const ISceneNode* hitNode = NULL;
	if (selector)
	{
		vector2di cursor_pos = getCursorPosition();
		line3df ray = coll->getRayFromScreenCoordinates(cursor_pos);

		core::triangle3df outTriangle;
		bool hit = coll->getCollisionPoint(								
			ray,	selector, hitPoint,
			outTriangle,//i dont need it however
			hitNode); 
	}
	return (ISceneNode*)hitNode;
} 


f32 getNodeAbove(ISceneNode* node,  ITriangleSelector* selector, f32 h)
{	
	f32 the_y = 0;
	bool hit = false;
	if (node && selector)
	{
		vector3df hitPoint;
		core::triangle3df outTriangle;
		const ISceneNode* hitNode;
		line3df ray;
		f32 height = node->getTransformedBoundingBox().getExtent().Y/2;
		ray.start = ray.end =node->getAbsolutePosition();
		ray.start.Y+= height; 
		ray.end += vector3df(0,-10000,0); 

		hit = coll->getCollisionPoint(								
			ray,	selector, hitPoint,
			outTriangle,//i dont need it however
			hitNode); 
		if (hit)
		{
			the_y = hitPoint.Y + height;
		}
	}
	return the_y;
}

ITriangleSelector* getSelector(ITerrainSceneNode* terrain)
{
	scene::ITriangleSelector* selector	= smgr->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);
	selector->drop();

	return selector;
}

ITriangleSelector* getSelector(IAnimatedMeshSceneNode* node)
{
	scene::ITriangleSelector* selector	= smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop();

	return selector;
}

ITriangleSelector* getBBoxSelector(ISceneNode* node)
{
	scene::ITriangleSelector* selector = smgr->createTriangleSelectorFromBoundingBox(node);
	node->setTriangleSelector(selector);
	selector->drop();

	return selector;
}

ITriangleSelector* getSelector(IMeshSceneNode* node)
{
	scene::ITriangleSelector* selector	= smgr->createTriangleSelector(node->getMesh(), node);
	node->setTriangleSelector(selector);
	selector->drop();

	return selector;
}

ITriangleSelector* getSelector(IMeshSceneNode* node, s32 minimalPolysPerNode/*=32*/)
{
	scene::ITriangleSelector* selector	= smgr->createOctTreeTriangleSelector(node->getMesh(), node, minimalPolysPerNode);
	node->setTriangleSelector(selector);
	selector->drop();

	return selector;
}
//
//ISceneNodeAnimator* setNodeFlyStraight(ISceneNode* node, const vector3df& start, const vector3df& end, u32 timeForWay)
//{
//	ISceneNodeAnimator* anim = NULL;
//	if (node)
//	{
//		//anim =	smgr->createFlyStraightAnimator(start, end, timeForWay);
//		anim = new CSceneNodeAnimatorFlyStraightX(start, end, timeForWay, false, getTime(), false);
//		if (anim)
//		{
//			node->addAnimator(anim);
//			anim->drop();
//		}
//	}
//	return anim;
//}
 
// rotates a bone
core::vector3df matrixRotation(core::vector3df rotation, core::vector3df vec)
{
	f32 pitch=vec.X;
	f32 yaw=vec.Y;
	f32 roll=vec.Z;
	matrix4 m,t;
	m.setRotationDegrees(rotation);

	t.setRotationDegrees(core::vector3df(0,0,roll));
	m*=t;

	t.setRotationDegrees(core::vector3df(pitch,0,0));
	m*=t;

	t.setRotationDegrees(core::vector3df(0,yaw,0));
	t*=m;

	return t.getRotationDegrees();
};


#ifdef HAVING_OPENCV

void IplImage_to_ITexture(IplImage* img, ITexture* tex)
{
	char* pixels = (char*)(tex->lock());
	char* ardata = (char*)img->imageData;
	char* final_loc = img->imageSize + img->imageData;

	switch(tex->getColorFormat())
	{
		case ECF_R8G8B8:
			while(ardata < final_loc)
			{
				*pixels = *ardata;
				pixels++; ardata++;
				*pixels = *ardata;
				pixels++; ardata++;
				*pixels = *ardata;
				pixels++; ardata++;				
			}
			//memcpy(pixels, ardata, img->imageSize);
			break;
			
		case ECF_A8R8G8B8:
			while(ardata < final_loc)
			{
				*pixels = *ardata;
				pixels++; ardata++;
				*pixels = *ardata;
				pixels++; ardata++;
				*pixels = *ardata;
				pixels++; ardata++;
				
				pixels++;
			}
			//memcpy(pixels, ardata, img->imageSize);
			break;
	}

	tex->unlock();
}

#endif

bool SimpleEventReceiver::OnEvent( const SEvent& event )
{
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		//	keys[event.KeyInput.Key] = event.KeyInput.PressedDown;

		if (event.KeyInput.PressedDown == true) 
		{ 
			if (keys[event.KeyInput.Key] == UP || keys[event.KeyInput.Key] == RELEASED) 
				keys[event.KeyInput.Key] = PRESSED; 
			else keys[event.KeyInput.Key] = DOWN; 
		} 
		else 
			if (keys[event.KeyInput.Key] == DOWN || keys[event.KeyInput.Key] == PRESSED)    
				keys[event.KeyInput.Key] = RELEASED; 
			else
				keys[event.KeyInput.Key] = UP;
	}

	OnSpecialEvent(event);

	if (event.EventType == irr::EET_GUI_EVENT)
	{
		return OnGui(event.GUIEvent);
	}
	else
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		if (event.KeyInput.PressedDown == true)
		{
			return OnKey(event.KeyInput);
		}
	}
	else
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
	{
		return OnMouse(event.MouseInput);
	}
	return false;
}

SimpleEventReceiver::SimpleEventReceiver()
{
   for (int i = 0; i < KEY_KEY_CODES_COUNT; i++) keys[i] = RELEASED; 
}


s32 addHighLevelShader(const c8* shader_file, const c8* vs_entry, const c8* ps_entry,
									IShaderConstantSetCallBack* callback, 
									E_MATERIAL_TYPE baseMaterial/* = video::EMT_SOLID*/)
{

	const c8* ps_file = ps_entry ? shader_file : NULL;
	return  gpu->addHighLevelShaderMaterialFromFiles(
		shader_file, vs_entry, video::EVST_VS_2_0,
		ps_file, ps_entry, video::EPST_PS_2_0, 
		callback, baseMaterial);
}

scene::ICameraSceneNode* addCameraFPS( f32 rotateSpeed /*= 100.0f*/, f32 moveSpeed /*= 1.0f*/, bool onTerrain /*= true*/, f32 jumpSpeed /*= 0.0f*/ )
{
	return smgr->addCameraSceneNodeFPS(0,rotateSpeed,moveSpeed,-1,g_KeyMap, 6, onTerrain, jumpSpeed);
}