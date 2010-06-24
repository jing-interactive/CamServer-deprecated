#include "testApp.h"
#include "stdio.h"

//--------------------------------------------------------------
testApp::testApp(){
}

//--------------------------------------------------------------
void testApp::setup() {
	
	
	ofSetVerticalSync(true);
	ofBackground(4, 4, 4);
	ofSetLogLevel(OF_LOG_NOTICE);
	
	bDrawLines  = false;
	bMouseForce = false;
	
	box2d.init();
	box2d.setGravity(0, 10);
	box2d.createFloor();
	box2d.checkBounds(true);
	box2d.setFPS(30.0);
	
	
	// the joints
	for(int i=0; i<5; i++) {
		float x = (ofGetWidth()/2) + cos(i) * 50; 
		float y = (ofGetHeight()/2) + sin(i) * 50;
		ballJoints[i].setPhysics(3.0, 0.53, 0.1);
		ballJoints[i].setup(box2d.getWorld(), x, y, 10);		
	}
	// connect all the ball joints
	for(int i=1; i<5; i++) {
		joints[i].setWorld(box2d.getWorld());
		joints[i].addJoint(ballJoints[i].body, ballJoints[i-1].body, 3.0, 0.5);
		if(i == 4) {
			joints[0].setWorld(box2d.getWorld());
			joints[0].addJoint(ballJoints[4].body, ballJoints[0].body, 3.0, 0.5);
		}
	}
		
	
}

//--------------------------------------------------------------
void testApp::update() {
	
	
	
	box2d.update();
	
	if(bMouseForce) {
		float strength = 8.0f;
		float damping  = 0.7f;
		for(int i=0; i<circles.size(); i++) {
			circles[i].addAttractionPoint(mouseX, mouseY, strength);
			circles[i].addDamping(damping, damping);
		}
		for(int i=0; i<customParticles.size(); i++) {
			customParticles[i].addAttractionPoint(mouseX, mouseY, strength);
			customParticles[i].addDamping(damping, damping);
		}
		
	}
	
}


//--------------------------------------------------------------
void testApp::draw() {
	
	
	for(int i=0; i<circles.size(); i++) {
		circles[i].draw();
	}
	for(int i=0; i<polygons.size(); i++) {
		polygons[i].draw();
	}
	for(int i=0; i<boxes.size(); i++) {
		boxes[i].draw();
	}
	for(int i=0; i<lines.size(); i++) {
		lines[i].draw();
	}
	for(int i=0; i<customParticles.size(); i++) {
		customParticles[i].draw();
	}
	
	
	for(int i=0; i<5; i++) ballJoints[i].draw();
	for(int i=0; i<5; i++) joints[i].draw();
	
	lineStrip.draw();
	box2d.draw();
	
	px = mouseX;
	py = mouseY;
	
	
	string info = "";
	info += "Press [s] to draw a line strip ["+ofToString(bDrawLines)+"]\n"; 
	info += "Press [f] to toggle Mouse Force ["+ofToString(bMouseForce)+"]\n"; 
	info += "Press [c] for circles\n";
	info += "Press [b] for circles\n";
	info += "Press [z] for custom particle\n";
	info += "Total Bobies: "+ofToString(box2d.getBodyCount())+"\n";
	info += "Total Joints: "+ofToString(box2d.getJointCount())+"\n\n";
	info += "FPS: "+ofToString(ofGetFrameRate())+"\n";
	ofSetColor(255, 255, 255);
	ofDrawBitmapString(info, 30, 30);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	
	if(key == 'c') {
		float r = ofRandom(4, 20);		// a random radius 4px - 20px
		ofxBox2dCircle circle;
		circle.setPhysics(3.0, 0.53, 0.1);
		circle.setup(box2d.getWorld(), mouseX, mouseY, r);
		circles.push_back(circle);
	}
	if(key == 'z') {
		float r = ofRandom(14, 40);		// a random radius 4px - 20px
		CustomParticle p;
		p.setPhysics(0.4, 0.953, 0.31);
		p.setup(box2d.getWorld(), mouseX, mouseY, r);
		p.color.r = ofRandom(20, 100);
		p.color.g = 0;//ofRandom(0, 255);
		p.color.b = ofRandom(150, 255);
		customParticles.push_back(p);
	}	
	if(key == 'b') {
		float w = ofRandom(4, 20);	
		float h = ofRandom(4, 20);	
		ofxBox2dRect rect;
		rect.setPhysics(3.0, 0.53, 0.1);
		rect.setup(box2d.getWorld(), mouseX, mouseY, w, h);
		boxes.push_back(rect);
	}
	
	if(key == 's') bDrawLines = !bDrawLines;
	if(key == 'f') bMouseForce = !bMouseForce;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	
	if(bDrawLines) {
		lineStrip.addPoint(x, y);
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	
	if(bDrawLines) {
		lineStrip.setWorld(box2d.getWorld());
		lineStrip.clear();
	}
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if(bDrawLines) lineStrip.createShape();
	
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){
	
}

