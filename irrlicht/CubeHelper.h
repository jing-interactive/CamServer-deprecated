#pragma once
//vinjn @ 2010-7-14

#include "irrlicht.h"
#include <windows.h>

using namespace irr;

#pragma comment(lib, "irrlicht.lib")

struct CubeHelper
{
	void setDegree(float a, float b, float c)
	{
		cube->setRotation(core::vector3df(a,b,c));
	}

	void addDegree(float a, float b, float c)
	{
		core::vector3df v = cube->getRotation();
		v += core::vector3df(a,b,c);
		cube->setRotation(v);
	}

	void getDegree(float& a, float& b, float& c)
	{
		core::vector3df v = cube->getRotation();
		a = v.X;
		b = v.Y;
		c = v.Z;
	}

	CubeHelper()
	{
		device = 0;
		smgr = 0;
		driver = 0;
		cam = 0;
		cube = 0;
	}

	void run()
	{
		irr::SIrrlichtCreationParameters param;
		param.DriverType = video::EDT_BURNINGSVIDEO;
//		param.WindowId = reinterpret_cast<void*>(hwnd);

		device = irr::createDevice();
	//	device = irr::createDeviceEx(param);

		if (!device)
			return;

		smgr = device->getSceneManager();
		driver = device->getVideoDriver();
	 
		cam = smgr->addCameraSceneNode();
		cam->setPosition(core::vector3df(0, 0, 145));
		cam->setTarget(core::vector3df(0,0,0));

	//	cube = smgr->addCubeSceneNode(30);
		cube = smgr->addMeshSceneNode(smgr->addArrowMesh("arrow"));
		cube->setScale(core::vector3df(50));

		smgr->addLightSceneNode(0, core::vector3df(50,50,50));

//		cube->setMaterialTexture(0, driver->getTexture("../media/dot.jpg"));
		cube->setMaterialFlag( video::EMF_WIREFRAME, false );
		cube->setMaterialFlag( video::EMF_LIGHTING, false );
		cube->setDebugDataVisible(scene::EDS_NORMALS);

		while (device->run())
		{
			driver->beginScene(true, true, video::SColor(255,122,122,122));
			smgr->drawAll();
			driver->endScene();
		}	 
		device->closeDevice();
		device->drop();
	} 

	irr::IrrlichtDevice* device;
	scene::ICameraSceneNode* cam;
	irr::scene::ISceneManager* smgr;
	video::IVideoDriver* driver;
	scene::ISceneNode* cube;
};