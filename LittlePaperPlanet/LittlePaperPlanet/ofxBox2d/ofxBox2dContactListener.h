#pragma once 

#include "Box2D.h"

class ofxBox2dContactListener : public b2ContactListener {
	
public:
	virtual void Add(const b2ContactPoint* point) {
		
		// point of collision
		b2Vec2 p = point->position;
		p *= OFX_BOX2D_SCALE;
		
		//
		b2Shape* shape1 = point->shape1;		///< the first shape
		b2Shape* shape2 = point->shape2;		///< the second shape
		
		contactAdd(b2Vec2(p.x, p.y));
		
		//b2Body * body_1 = point->shape1->GetBody();
		//MonsterData * theData = (MonsterData*)body_1->GetUserData();
		
	}
	virtual void Remove(const b2ContactPoint* point) {
		// point of collision
		b2Vec2 p = point->position;
		p *= OFX_BOX2D_SCALE;
		
		//
		b2Shape* shape1 = point->shape1;		///< the first shape
		b2Shape* shape2 = point->shape2;		///< the second shape
		
		contactRemove(b2Vec2(p.x, p.y));		
	}
	
	ofxBox2dContactListener() {
	}
	
	
	
	virtual void contactAdd(b2Vec2 p) {
	}
	virtual void contactRemove(b2Vec2 p) {
	}
};



