

#pragma once
#include "ofMain.h"
#include "ofxBox2dBaseShape.h"

class ofxBox2dRect : public ofxBox2dBaseShape {
	
public:
	
	
	b2PolygonDef shape;
	
	//------------------------------------------------
	
	ofxBox2dRect() {
	}
	
	//------------------------------------------------
	void setup(b2World * b2dworld, float x, float y, float w, float h, bool isFixed=false) {
		
		if(b2dworld == NULL) {
			ofLog(OF_LOG_NOTICE, "- must have a valid world -");
			return;
		}
		
		world				= b2dworld;
		bIsFixed			= isFixed;
		//Rect Shape
		shape.SetAsBox(w/OFX_BOX2D_SCALE, h/OFX_BOX2D_SCALE);
		
		//Build Body for shape
		b2BodyDef bodyDef;
		
		//set properties
		if(isFixed) {
			shape.density	  = 0;
			shape.restitution  = 0;
			shape.friction	  = 0;
		}
		else {
			shape.density	  = mass;
			shape.restitution  = bounce;
			shape.friction	  = friction;
		}
		
		bodyDef.position.Set(x/OFX_BOX2D_SCALE, y/OFX_BOX2D_SCALE);	
		
		body = world->CreateBody(&bodyDef);
		body->SetLinearVelocity(b2Vec2(0.0, 0.0));
		body->CreateShape(&shape);
		body->SetMassFromShapes();
	}
	
	
	/*
	 //------------------------------------------------
	 float getRadius() {
	 b2Shape* shape		= body->GetShapeList();
	 b2CircleShape *data = (b2CircleShape*)shape;
	 return data->GetRadius() * OFX_BOX2D_SCALE;
	 }
	 
	 //------------------------------------------------
	 float getRotation() {
	 
	 const  b2XForm& xf	= body->GetXForm();
	 float  r			= getRadius()/OFX_BOX2D_SCALE;
	 b2Vec2 a			= xf.R.col1;
	 b2Vec2 p1			= body->GetPosition();
	 b2Vec2 p2			= p1 + r * a;
	 
	 float dx = p2.x+r/2 - p1.x+r/2;
	 float dy = p2.y - p1.y;
	 return ofRadToDeg(atan2(dy, dx));
	 
	 }
	 */
	
	
	//------------------------------------------------
	void draw() {
		
		if(dead) return;
		
		//wow this is a pain
		b2Shape* s = body->GetShapeList();
		const b2XForm& xf = body->GetXForm();
		b2PolygonShape* poly = (b2PolygonShape*)s;
		int count = poly->GetVertexCount();
		const b2Vec2* localVertices = poly->GetVertices();
		b2Assert(count <= b2_maxPolygonVertices);
		b2Vec2 verts[b2_maxPolygonVertices];
		for(int32 i=0; i<count; ++i) {
			verts[i] = b2Mul(xf, localVertices[i]);
		}
		
		
		ofEnableAlphaBlending();
		ofSetColor(10, 10, 10, 100);
		ofFill();
		ofBeginShape();
		for (int32 i = 0; i <count; i++) {
			ofVertex(verts[i].x*OFX_BOX2D_SCALE, verts[i].y*OFX_BOX2D_SCALE);
		}
		ofEndShape();
		
		ofSetColor(0, 255, 255);
		ofNoFill();
		ofBeginShape();
		for (int32 i = 0; i <count; i++) {
			ofVertex(verts[i].x*OFX_BOX2D_SCALE, verts[i].y*OFX_BOX2D_SCALE);
		}
		ofEndShape(true);	
		ofDisableAlphaBlending();
	}
	
};














