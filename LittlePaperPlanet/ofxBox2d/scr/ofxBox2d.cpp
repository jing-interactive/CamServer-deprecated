#include "ofxBox2d.h"

// ------------------------------------------------------
ofxBox2d::ofxBox2d() {

}
// ------------------------------------------------------ init
void ofxBox2d::init() {

	//settings
	bCheckBounds   = false;
	enableGrabbing = true;
	bWorldCreated  = false;
	scale		   = 30.0f;
	doSleep		   = true;

	//gravity
	gravity.set(0, 5.0f);
	setFPS(60.0);

	//mouse grabbing
	mouseJoint = NULL;
	ground	   = NULL;

	//debug drawer
	debugRender.setScale(scale);
	debugRender.SetFlags(1);

	worldAABB.lowerBound.Set(-100.0f, -100.0f);
	worldAABB.upperBound.Set(100.0f, 100.0f);

	world = new b2World(worldAABB, b2Vec2(gravity.x, gravity.y), doSleep);
	world->SetDebugDraw(&debugRender);


	float bw = ofGetWidth()/scale;
	ofLog(OF_LOG_NOTICE, "- Box2D Created -\n");

	/*
	ofAddListener(ofEvents.mousePressed, this, &ofxBox2d::mousePressed);
	ofAddListener(ofEvents.mouseDragged, this, &ofxBox2d::mouseDragged);
	ofAddListener(ofEvents.mouseReleased, this, &ofxBox2d::mouseReleased);
	*/

	bWorldCreated = true;

}

// ------------------------------------------------------ grab shapes
void ofxBox2d::mousePressed(ofMouseEventArgs &e) {

	if(enableGrabbing) {
		b2Vec2 p(e.x/OFX_BOX2D_SCALE, e.y/OFX_BOX2D_SCALE);

		if (mouseJoint != NULL) {
			return;
		}

		// Make a small box.
		b2AABB aabb;
		b2Vec2 d;
		d.Set(0.001f, 0.001f);
		aabb.lowerBound = p - d;
		aabb.upperBound = p + d;

		// Query the world for overlapping shapes.
		const int32 k_maxCount = 10000;
		b2Shape* shapes[k_maxCount];
		int32 count = world->Query(worldAABB, shapes, k_maxCount);
		b2Body* body = NULL;

		for (int32 i = 0; i < count; ++i) {

			b2Body* shapeBody = shapes[i]->GetBody();
			if (shapeBody->IsStatic() == false && shapeBody->GetMass() > 0.0f) {
				bool inside = shapes[i]->TestPoint(shapeBody->GetXForm(), p);
				if (inside) {
					body = shapes[i]->GetBody();
					break;
				}
			}
		}

		if (body) {

			b2MouseJointDef md;
			md.body1 = world->GetGroundBody();
			md.body2 = body;
			md.target = p;
#ifdef TARGET_FLOAT32_IS_FIXED
			md.maxForce = (body->GetMass() < 16.0)?
			(1000.0f * body->GetMass()) : float32(16000.0);
#else
			md.maxForce = 1000.0f * body->GetMass();
#endif
			mouseJoint = (b2MouseJoint*)world->CreateJoint(&md);
			body->WakeUp();

		}
	}
}
void ofxBox2d::mouseDragged(ofMouseEventArgs &e) {
	b2Vec2 p(e.x/OFX_BOX2D_SCALE, e.y/OFX_BOX2D_SCALE);
	if (mouseJoint && enableGrabbing) mouseJoint->SetTarget(p);
}
void ofxBox2d::mouseReleased(ofMouseEventArgs &e) {

	if(mouseJoint && enableGrabbing) {
		world->DestroyJoint(mouseJoint);
		mouseJoint = NULL;
	}
}

// ------------------------------------------------------ set gravity
void ofxBox2d::setGravity(float x, float y) {
	world->SetGravity(b2Vec2(x, y));
}
void ofxBox2d::setGravity(ofPoint pt) {
	world->SetGravity(b2Vec2(pt.x, pt.y));
}

// ------------------------------------------------------ set bounds
void ofxBox2d::setBounds(ofPoint lowBounds, ofPoint upBounds) {
}

// ------------------------------------------------------ create bounds
void ofxBox2d::createFloor() {

	if(!bWorldCreated) return;

	b2BodyDef bd;
	bd.position.Set(0, 0);
	ground = world->CreateBody(&bd);
	b2PolygonDef sd;
	sd.filter.groupIndex = 1;

	sd.density = 0.0f;
	sd.restitution = 0.0f;
	sd.friction = 0.6;
	float thick = 30/OFX_BOX2D_SCALE;

	//bottom
	sd.SetAsBox((ofGetWidth()/OFX_BOX2D_SCALE)/2, thick, b2Vec2((ofGetWidth()/OFX_BOX2D_SCALE)/2, ofGetHeight()/OFX_BOX2D_SCALE), 0.0);
	ground->CreateShape(&sd);
}

// ------------------------------------------------------ create bounds
void ofxBox2d::createBounds() {

	if(!bWorldCreated) return;

	b2BodyDef bd;
	bd.position.Set(0, 0);
	ground = world->CreateBody(&bd);
	b2PolygonDef sd;
	sd.filter.groupIndex = 1;

	sd.density = 0.0f;
	sd.restitution = 0.0f;
	sd.friction = 0.6;
	float thick = 30/OFX_BOX2D_SCALE;
	// w h x y r
	//right
	sd.SetAsBox(thick, (ofGetHeight()/OFX_BOX2D_SCALE)/2, b2Vec2((ofGetWidth()/OFX_BOX2D_SCALE), (ofGetHeight()/OFX_BOX2D_SCALE)/2), 0.0);
	ground->CreateShape(&sd);
	//left
	sd.SetAsBox(thick, (ofGetHeight()/OFX_BOX2D_SCALE)/2, b2Vec2(0, (ofGetHeight()/OFX_BOX2D_SCALE)/2), 0.0);
	ground->CreateShape(&sd);
	//top
	sd.SetAsBox((ofGetWidth()/OFX_BOX2D_SCALE)/2, thick, b2Vec2((ofGetWidth()/OFX_BOX2D_SCALE)/2, 0), 0.0);
	ground->CreateShape(&sd);
	//bottom
	sd.SetAsBox((ofGetWidth()/OFX_BOX2D_SCALE)/2, thick, b2Vec2((ofGetWidth()/OFX_BOX2D_SCALE)/2, ofGetHeight()/OFX_BOX2D_SCALE), 0.0);
	ground->CreateShape(&sd);
}

// ------------------------------------------------------ check if shapes are out of bounds
void ofxBox2d::checkBounds(bool b) {
	bCheckBounds = b;
}

// ------------------------------------------------------
void ofxBox2d::update() {

	// destroy the object if we are out of the bounds
	if(bCheckBounds) {
		/* need to add a bit of code to remove the bodie */
	}


	float	timeStep		   = (1.0f / fps);
	int		velocityIterations = 40;
	int		positionIterations = 20;

	world->Step(timeStep, velocityIterations, positionIterations);
	world->Validate();

}

// ------------------------------------------------------
void ofxBox2d::draw() {
	if(mouseJoint) {
		b2Body* mbody = mouseJoint->GetBody2();
		b2Vec2 p1 = mbody->GetWorldPoint(mouseJoint->m_localAnchor);
		b2Vec2 p2 = mouseJoint->m_target;

		p1 *= OFX_BOX2D_SCALE;
		p2 *= OFX_BOX2D_SCALE;

		//draw a line from touched shape
		ofEnableAlphaBlending();
		ofSetLineWidth(2.0);
		ofSetColor(200, 200, 200, 200);
		ofLine(p1.x, p1.y, p2.x, p2.y);
		ofNoFill();
		ofSetLineWidth(1.0);
		ofCircle(p1.x, p1.y, 2);
		ofCircle(p2.x, p2.y, 5);
		ofDisableAlphaBlending();
	}

	//draw the ground
	for(b2Shape* s=ground->GetShapeList(); s; s=s->GetNext()) {

		const b2XForm& xf = ground->GetXForm();
		b2PolygonShape* poly = (b2PolygonShape*)s;
		int count = poly->GetVertexCount();
		const b2Vec2* verts = poly->GetVertices();
		ofEnableAlphaBlending();
		ofFill();
		ofSetColor(90, 90, 90, 100);
		ofBeginShape();
		for(int j=0; j<count; j++) {

			b2Vec2 pt = b2Mul(xf, verts[j]);

			ofVertex(pt.x*OFX_BOX2D_SCALE, pt.y*OFX_BOX2D_SCALE);
		}
		ofEndShape();
		ofDisableAlphaBlending();
	}
}

