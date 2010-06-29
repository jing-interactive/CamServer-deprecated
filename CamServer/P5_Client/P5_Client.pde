import oscP5.*;
import netP5.*;

color red = color(255, 0, 0);
color gray = color(122, 122, 122);
int k = 2; 

OscP5 oscP5;
// A reference to our box2d world
PBox2D box2d;
// A list we'll use to track fixed objects
ArrayList boundaries = new ArrayList();
// Our "blob" object
ArrayList blobs = new ArrayList();

ArrayList blobList = new ArrayList();
ArrayList prevList = new ArrayList();
ArrayList faceList = new ArrayList();

int W = 320*k;
int H = 240*k;

float distance(float x1, float y1, float x2, float y2)
{
  return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void initPhysics()
{
  // Initialize box2d physics and create the world
  box2d = new PBox2D(this);
  box2d.createWorld();
  
   // Here we create a dynamic gravity vector based on the location of our mouse
  //PVector g = new PVector(mouseX-width/2,mouseY-height/2);
 // g.normalize();
  //g.mult(15);
  box2d.setGravity(0, -5);  

  // Add some boundaries
  boundaries.add(new Boundary(width/2,height-5,width,10));
  boundaries.add(new Boundary(width/2,5,width,10));
  boundaries.add(new Boundary(width-5,height/2,10,height));
  boundaries.add(new Boundary(5,height/2,10,height));

  // Make a new blob
  //blob = new Blob(new Vec2(width/2, height/2), 6, 50);
}

void setup() {
  size(W, H);
  frameRate(60);
  smooth();

  oscP5 = new OscP5(this,3333);
  
  initPhysics();
}

void vDrawPoly(ArrayList pt_list)
{ 
  beginShape();
  for (int j=0; j<pt_list.size();j++ )
  {
    PVector pos = (PVector)pt_list.get(j);
    vertex(pos.x, pos.y); 
  }
  endShape(CLOSE); 
}



void draw()
{  
  background(255);

  // We must always step through time!
  box2d.step();
  
  for (int i=0;i<prevList.size() ; i++)
  {    
    boolean hit = false;
    vBlob prev = (vBlob)prevList.get(i);
    for (int j=0;j<blobList.size();j++)
    {
      vBlob obj = (vBlob)blobList.get(j);
      if (distance(prev.x, prev.y, obj.x, obj.y) < width/10)
      {
        hit = true;
        break;
      }
    }
    if (!hit)//!hit makes a hit
    {
//      Body aHole = physics.createRect(prev.x, prev.y, 
//            prev.x+prev.w,prev.y+prev.h);
      Vec2 vel = new Vec2(0, -10);
       blobs.add(new Blob(prev,6, 50));
 //     aHole.setLinearVelocity(vel);
    }
  }

  prevList.clear();

  for (int i=0;i<blobList.size();i++)
  {
    vBlob obj = (vBlob)blobList.get(i);
    //rect(obj.x, obj.y, obj.w, obj.h);

    noFill();
    if (obj.isHole)
    {
      prevList.add(obj);
      stroke(red);
      obj.display();
    }
    else
    {
      stroke(gray);
      obj.display();
    }
  } 

  for (int i=0;i<faceList.size();i++)
  {
    vBlob obj = (vBlob)faceList.get(i);
    rect(obj.x, obj.y, obj.w, obj.h);
  }
  
   // Show the blob!
  for (int i = 0; i < blobs.size(); i++) {
    Blob b = (Blob) blobs.get(i);
    b.display();
  }
    // Show the boundaries!
  for (int i = 0; i < boundaries.size(); i++) {
    Boundary wall = (Boundary) boundaries.get(i);
    wall.display();
  } 
}

void mousePressed() {

}

void keyPressed()
{
   //physics.setCustomRenderingMethod(physics, "defaultDraw");
//  physics.destroy();
 //   box2d.world.destroy();
 //   initPhysics(); 
}

/* incoming osc message are forwarded to the oscEvent method. */

void oscEvent(OscMessage msg) {
  /* check if theOscMessage has the address pattern we are looking for. */
  if(msg.checkAddrPattern("/start")==true) {
    blobList.clear();
    faceList.clear();
  }
  else if(msg.checkAddrPattern("/object")){ 
    int x = msg.get(2).intValue();  
    int y = msg.get(3).intValue();
    int w = msg.get(4).intValue();
    int h = msg.get(5).intValue();
    print("### osc message /object");
    println(" values: "+x+", "+y+", "+w+", "+h);
    blobList.add(new vBlob(x*k, y*k, w*k, h*k));
  }
  else if(msg.checkAddrPattern("/face")){ 
    int x = msg.get(0).intValue();  
    int y = msg.get(1).intValue();
    int w = msg.get(2).intValue();
    int h = msg.get(3).intValue();
    print("### osc message /face");
    println(" values: "+x+", "+y+", "+w+", "+h);
    faceList.add(new vBlob(x*k, y*k, w*k, h*k));
  }
  else if(msg.checkAddrPattern("/contour") || msg.checkAddrPattern("/hole") )  {    
    int n =  msg.get(0).intValue();
    vBlob obj = (vBlob)blobList.get(blobList.size()-1);
    for (int i=0; i<n; i++)
    { 
      int x = msg.get(1+i*2).intValue();  
      int y = msg.get(2+i*2).intValue();			
      //	println("("+x+", "+y+") ");
      obj.points.add(new PVector(x*k,y*k));
    }
    if (msg.checkAddrPattern("/contour"))
    {
      println("### osc message /contour");
      obj.isHole = false;
    }
    else
    {
      println("### osc message /hole");
      obj.isHole = true;
    }
  }
  else if(msg.checkAddrPattern("/end"))
  {
   // ok_to_draw = true;
  }
} 
