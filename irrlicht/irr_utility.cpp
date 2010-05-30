#include "irr_utility.h"



IrrlichtDevice* device = 0;
scene::ISceneManager* smgr = 0;
video::IVideoDriver* driver = 0;
gui::IGUIEnvironment* env = 0;
scene::ISceneCollisionManager* col = 0;

#ifdef USING_NEWTON
	newton::IWorld* world = 0;
#endif



bool keys[KEY_KEY_CODES_COUNT];

bool initIrrlicht(s32 w, s32 h, s32 bpp, bool fullscreen)
{
    device = createDevice(EDT_OPENGL, core::dimension2d<s32>(w, h),
                          bpp,fullscreen, true);//EDT_DIRECT3D9
    driver = device->getVideoDriver();
    env = device->getGUIEnvironment();
    driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT,true);
    smgr = device->getSceneManager();
	col = smgr->getSceneCollisionManager();

#ifdef USING_NEWTON
	world = newton::createPhysicsWorld(device);
#endif

	return true;
}

int msg_box(char* info, UINT flag)
{
	return MessageBoxA(NULL, info, "irrlicht", flag);
}

scene::ITerrainSceneNode* loadTerrain()
{
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
											"media/terrain-heightmap.bmp");

	terrain->setScale(core::vector3df(500, 50.0f, 500));
	terrain->setMaterialFlag(EMF_LIGHTING, false);
	//terrain->setPosition(core::vector3df(-15200,-2500,-15400));
	terrain->setMaterialTexture(0, driver->getTexture("media/terrain-texture.jpg"));
	terrain->setMaterialTexture(1, driver->getTexture("media/detailmap2.jpg"));

	terrain->setMaterialType(EMT_DETAIL_MAP);

	terrain->scaleTexture(1.0f, 50.0f);

	return terrain;
}


scene::IAnimatedMeshSceneNode* loadAnimatedMesh(const c8* mesh_file, const c8* tex, const c8* tex_normal)
{
	scene::IAnimatedMesh* mesh = smgr->getMesh(mesh_file);
	scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (tex)
	{
		node->setMaterialTexture( 0, driver->getTexture(tex) );
		if (tex_normal)
		{
			node->setMaterialTexture( 1, driver->getTexture(tex_normal) );
			node->setMaterialType(EMT_NORMAL_MAP_SOLID);
		}
	}
    node->setMaterialFlag(EMF_LIGHTING, true);

	return node;
}


scene::IMeshSceneNode* loadMesh(const c8* mesh_file, const c8* texture)
{
	scene::IMesh* mesh = smgr->getMesh(mesh_file);
	scene::IMeshSceneNode* node = smgr->addMeshSceneNode( mesh );

	if (node)
	{
		if (texture)
		{
			node->setMaterialTexture( 0, driver->getTexture(texture) );
		}    
		node->setMaterialFlag(EMF_LIGHTING, true);
	}
	return node;
}

bool hitIrrSelector(scene::ITriangleSelector* selector, 
					core::vector3df pos, core::vector3df& new_pos)
{
    core::line3d<f32> ray(pos, pos);
	ray.start.Y += 9999;
    ray.end.Y -= 9999;

	core::triangle3df outTriangle;

	bool hit = col->getCollisionPoint(ray, selector, new_pos, outTriangle);

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


IBillboardSceneNode* create_anim_billboard(array<ITexture*>& tex_array, char* fmt, u32 start, u32 end, s32 timePerFrame, 
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
	anim = smgr->createTextureAnimator(tex_array, timePerFrame);

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


#ifdef USING_NEWTON

newton::IBody* createEllipsoid(const c8* tex, f32 mass, bool gravity)
{
	//create the sphere scene node
	scene::ISceneNode* node = smgr->addSphereSceneNode();

	node->setMaterialFlag(EMF_LIGHTING, true);
	//node->setScale(core::vector3df(1.0f,1.0f,1.0f));
	node->setMaterialTexture(0,driver->getTexture(tex));

	//the physics part

	//then create a body relative to the sphere scene node
	//let's instantiate a newton::SBodyFromNode object
	//to store our body data (like mass, node, mesh,exc..)
	newton::SBodyFromNode data;
	//set the node attached to the body as the sphere scene node
	data.Node = node;
	data.Mass = mass;
	data.Type = newton::EBT_PRIMITIVE_ELLIPSOID;

	newton::IBody* body;
	body = world->createBody(data);

	if (gravity)
	{
		body->addGravityForce();
	}
	
	return body;
}



newton::IBody* createCube(const c8* tex, f32 size, f32 mass, bool gravity)
{
	//create the sphere scene node
	scene::ISceneNode* node = smgr->addCubeSceneNode(size);

	//node->setMaterialFlag(EMF_LIGHTING, false);
	//node->setScale(core::vector3df(1.0f,1.0f,1.0f));
	if (tex)
		node->setMaterialTexture(0,driver->getTexture(tex));
	node->setPosition(core::vector3df(1469.6f,300.474f,1363.34f));

	//the physics part

	//then create a body relative to the sphere scene node
	//let's instantiate a newton::SBodyFromNode object
	//to store our body data (like mass, node, mesh,exc..)
	newton::SBodyFromNode data;
	//set the node attached to the body as the sphere scene node
	data.Node = node;
	data.Mass = mass;
	data.Type = newton::EBT_PRIMITIVE_BOX;

	newton::IBody* body;
	body = world->createBody(data);

	if (gravity)
	{
		body->addGravityForce();
	}
	
	return body;
}

bool hitNewtonBody(newton::IBody *body, core::vector3df pos, core::vector3df& new_pos)
{
    newton::SIntersectionPoint intPoint;

    core::line3d<f32> ray(pos, pos);
	ray.start.Y += 0.1f;
    ray.end.Y -= 999999999.9f;

    bool hit = world->getCollisionManager()->getCollisionPoint(
		body, ray, intPoint);

	new_pos = intPoint.point;

	return hit;
}




newton::ICameraBodyFPS* create_newton_camera(ICameraSceneNode* cam, vector3df& box_size, f32 rot, f32 speed)
{
	//we need to move it so we create a character controller
	newton::SBodyFromNode data;
	data.Node = cam;
	data.Type = newton::EBT_PRIMITIVE_ELLIPSOID;

	//a camera does not have box size so we need to set it manually
	data.BodyScaleOffsetFromNode = box_size;

	newton::ICameraBodyFPS* body = 0;
	body = world->createCameraFPS(data);
	body->setContinuousCollisionMode(true);

	//INI CAMERA
	body->setMouseSensitive(rot);
	body->setMoveSpeed(speed);
	body->setType(newton::ECFT_NORMAL);

	return body;
}


newton::IBody* create_newton_level(IMeshSceneNode* node, IMesh* mesh/* = 0*/)
{
	newton::IBody* body = 0;
	newton::SBodyFromNode data;
	data.Node= (ISceneNode*)node;

	if (mesh)
		data.Mesh= mesh;
	else
		data.Mesh= node->getMesh();
	data.Type=newton::EBT_TREE;
	body = world->createBody(data);

	return body;
}

newton::IBody* create_newton_body(IMeshSceneNode* node, IMesh* mesh/* = 0*/)
{
	newton::IBody* body = 0;
	newton::SBodyFromNode data;
	data.Node= (ISceneNode*)node;

	if (mesh)
		data.Mesh= mesh;
	else
		data.Mesh= node->getMesh();
	data.Type=newton::EBT_CONVEX_HULL;
	body = world->createBody(data);

	return body;
}



vector3df getTargetRay(s32 x, s32 y, newton::IBody* body)
{ 
	clamp(x,0,driver->getScreenSize().Width);
	clamp(y,0,driver->getScreenSize().Height);

	line3d<f32> line =  col->getRayFromScreenCoordinates(position2di(x,y));

	vector3df ray;

	if (body)
	{
		newton::SIntersectionPoint pt = 
			world->getCollisionManager()->getCollisionFirstPoint(line);
		if ( pt.body)
		{
	//		pt.body->getNode()->setDebugDataVisible(EDS_BBOX);		
			//[add] never shoot objects behind dragon

			ray = pt.point - body->getPosition();
			//[bug here]
		}
	}
	else
	{
		//naive shoot		
		ray = line.getVector();
	}

	ray = ray.normalize();

	return ray;
}



#endif