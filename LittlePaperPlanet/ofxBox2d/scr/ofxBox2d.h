#pragma once

#include "ofxBox2dUtils.h"
#include "ofxBox2dBaseShape.h"

#include "ofxBox2dCircle.h"
#include "ofxBox2dPolygon.h"
#include "ofxBox2dRect.h"
#include "ofxBox2dLine.h"

#include "ofxBox2dSoftBody.h"
#include "ofxBox2dJoint.h"
#include "ofxBox2dRender.h"

class ofxBox2d {
	
private:
	float			fps;
	
public:
	
	b2AABB			worldAABB;
	b2World *		world;
	ofxBox2dRender	debugRender;
	
	float			scale;
	bool			doSleep;
	bool			bWorldCreated;
	bool			enableGrabbing;
	bool			bCheckBounds;
	
	ofPoint			gravity;
	b2BodyDef		bd;
	
	b2Body*				m_bomb;
	b2MouseJoint*		mouseJoint;
	b2Body*				ground;
	b2Body*				mainBody;
	

	// ------------------------------------------------------ 
	ofxBox2d();
	void init();
	void setFPS(float theFps) { fps = theFps; }
	
	void mousePressed(ofMouseEventArgs &e);
	void mouseDragged(ofMouseEventArgs &e);
	void mouseReleased(ofMouseEventArgs &e);
	
	b2World* getWorld()		 { return world; }
	int		 getBodyCount()  { return world->GetBodyCount(); }
	int		 getJointCount() { return world->GetJointCount(); }
	
	
	void setGravity(float x, float y);
	void setGravity(ofPoint pt);
	void setBounds(ofPoint lowBounds, ofPoint upBounds);
	void createBounds();
	void createFloor();
	void checkBounds(bool b);
	
	void update(); 
	void draw();
	
};
