// The Nature of Code
// <http://www.shiffman.net/teaching/nature>
// Spring 2010
// PBox2D example

// A BlobbyMonster skeleton
// Could be used to create BlobbyMonsterbly characters a la Nokia Friends
// http://postspectacular.com/work/nokia/friends/start

import pbox2d.*;

import org.jbox2d.collision.shapes.*;
import org.jbox2d.common.*;
import org.jbox2d.dynamics.*;
import org.jbox2d.dynamics.joints.*;

class BlobbyMonster {

  // A list to keep track of all the points in our BlobbyMonster
  ArrayList skeleton;

  float bodyRadius;  // The radius of each body that makes up the skeleton
  float radius;      // The radius of the entire BlobbyMonster
  float totalPoints; // How many points make up the BlobbyMonster

  float minX,minY;
  float maxX,maxY;


  // We should modify this constructor to receive arguments
  // So that we can make many different types of BlobbyMonsters
  BlobbyMonster(vBlob ref, float bodyR, float R) {

    // Create the empty 
    skeleton = new ArrayList();

    // Let's make a volume of joints!
    ConstantVolumeJointDef cvjd = new ConstantVolumeJointDef();

    // Where and how big is the BlobbyMonster
    Vec2 center = new Vec2(width/2, height/2);
    radius = R;
    totalPoints = 20;
    bodyRadius = bodyR;

    ArrayList pts = ref.points;
    for (int i=0;i<pts.size();i++)
    { 
      PVector pos = (PVector)pts.get(i);
      float x = pos.x;
      float y = pos.y;
      // Make each individual body
      BodyDef bd = new BodyDef();
      bd.fixedRotation = true; // no rotation!
      bd.position.set(box2d.screenToWorld(x,y));
      Body body = box2d.createBody(bd);

      // The body is a circle
      CircleDef cd = new CircleDef();
      cd.radius = box2d.scaleScreenToWorld(bodyRadius);
      cd.density = 1.0f;
      // For filtering out collisions
      cd.filter.groupIndex = 1;

      // Finalize the body
      body.createShape(cd);
      // Add it to the volume
      cvjd.addBody(body);
      // We always do this at the end
      body.setMassFromShapes();

      // Store our own copy for later rendering
      skeleton.add(body);
    }

    // These parameters control how stiff vs. jiggly the BlobbyMonster is
    cvjd.frequencyHz = 10.0f;
    cvjd.dampingRatio = 1.0f;

    // Put the joint thing in our world!
    box2d.world.createJoint(cvjd);
  }


  // Time to draw the BlobbyMonster!
  // Can you make it a cute character, a la http://postspectacular.com/work/nokia/friends/start
  void display() {
    minX = minY = 100000;
    maxX = maxY = 0;

    Vec2 prev = new Vec2();
    //[Draw the outline]
    beginShape();
    //noFill();
    fill(155);
    stroke(255, 0, 0);
    strokeWeight(4);
    for (int i = -1; i < skeleton.size()+1; i++) 
    {
      int j = i;
      if (j == -1) j = 0;
      if (j == skeleton.size()) j = skeleton.size()-1;
      Body b = (Body) skeleton.get(j);
      Vec2 pos = box2d.getScreenPos(b);

      float ax = pos.x;
      float ay = pos.y;
      if (distance(prev.x, prev.y, ax, ay) < 5)
        continue;

      curveVertex(ax,ay);

      prev = pos;

      if (ax > maxX) maxX = ax;
      else if (ax < minX) minX = ax;
      if (ay > maxY) maxY = ay;
      else if (ay < minY) minY = ay;
    }
    endShape(CLOSE);

    //[contour]
    if (false)
    {
      noFill();
      stroke(0);
      for (int i = 0; i < skeleton.size(); i++)  
      {
        Body b = (Body) skeleton.get(i);
        Vec2 pos = box2d.getScreenPos(b);    
        ellipse(pos.x,pos.y,2,2);
      }
    }

    float faceW = maxX - minX;
    float faceH = maxY - minY;

    //[eye]
    float leftX = map(0.3, 0, 1, minX, maxX);
    float rightX = map(0.7, 0, 1, minX, maxX);
    float eyeY = map(0.3, 0, 1, minY, maxY);
    fill(0, 0, 0);
    stroke(0);
    float eyeR = 7;
    //ellipse(leftX,eyeY,eyeR,eyeR);
    //ellipse(rightX,eyeY,eyeR,eyeR);
    line(leftX, eyeY, leftX - eyeR, eyeY+eyeR);
    line(rightX, eyeY, rightX + eyeR, eyeY+eyeR);

    //[mouth]
    float mouthX = map(0.5, 0, 1, minX, maxX);
    float mouthY = map(0.7, 0, 1, minY, maxY);
    rectMode(CENTER);
    noFill();
    stroke(0,0,122);
    strokeWeight(2);
    rect(mouthX, mouthY, faceW*0.2, faceH*0.1);

    //[jump AKA duoang duoang]
    if ( maxY > height*0.95)
    {
      for (int i = 0; i < skeleton.size(); i++)  
      {
        Body b = (Body) skeleton.get(i);
        b.setLinearVelocity(new Vec2(random(-10,10),random(4.0f,18.0f)));
        b.setAngularVelocity(random(-10,10));
      }
    }
  }
}

