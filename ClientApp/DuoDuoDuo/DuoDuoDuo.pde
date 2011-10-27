import oscP5.*;
import netP5.*;

color red = color(255, 0, 0);
color green = color(0, 255, 0);
color blue = color(0, 0, 255);
color gray = color(122, 122, 122);
int k = 2; 

OscP5 oscP5;
// A reference to our box2d world
PBox2D box2d;
// A list we'll use to track fixed objects
ArrayList boundaries = new ArrayList();
// Our "blob" object
ArrayList monsterList = new ArrayList(); 
ArrayList blobList = new ArrayList(); 
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

int g_prevMonsterID = 0;

void draw()
{  
  background(255);

  // We must always step through time!
  box2d.step();

  for (int i=0;i<blobList.size();i++)
  {
    vBlob obj = (vBlob)blobList.get(i);
    //rect(obj.x, obj.y, obj.w, obj.h);

    noFill();
    if (obj.status.equals("move"))
    {
      stroke(red);
      obj.display();
    }
    else
    {
      stroke(gray);
      obj.display();
    }

    if (obj.isHole && obj.id != g_prevMonsterID &&
    obj.status.equals("leave"))
    {
      Vec2 vel = new Vec2(0, -10);
      monsterList.add(new BlobbyMonster(obj,6, 50));
      //     aHole.setLinearVelocity(vel);
      g_prevMonsterID = obj.id;
    }
  } 

  for (int i=0;i<faceList.size();i++)
  {
    vBlob obj = (vBlob)faceList.get(i);
    rect(obj.x, obj.y, obj.w, obj.h);
  }

  // Show the blob!
  for (int i = 0; i < monsterList.size(); i++) {
    BlobbyMonster b = (BlobbyMonster) monsterList.get(i);
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
   for (int i = 0; i < monsterList.size(); i++) {
    monsterList.remove(i);
  } 
  //physics.setCustomRenderingMethod(physics, "defaultDraw");
  //  physics.destroy();
  //   box2d.world.destroy();
  //   initPhysics(); 
}

/* incoming osc message are forwarded to the oscEvent method. */

void oscEvent(OscMessage msg) {
  /* check if theOscMessage has the address pattern we are looking for. */
  if(msg.checkAddrPattern("/start")) {
    // println("");
    blobList.clear();
    faceList.clear();
  }
  else
  {
    boolean isBlob = msg.checkAddrPattern("/blob");
    boolean isContour = msg.checkAddrPattern("/contour");
    boolean isHole = msg.checkAddrPattern("/hole");

    if (isBlob || isContour || isHole)
    {
      print("### osc "+msg.addrPattern());  
      int id = msg.get(0).intValue();
      String status = msg.get(1).stringValue();
      float x = W*msg.get(4).floatValue();
      float y = H*msg.get(5).floatValue();
      float w = W*msg.get(6).floatValue();
      float h = H*msg.get(7).floatValue();
      println(" #"+id+": "+status+" "+x+", "+y+", "+w+", "+h);
      vBlob obj = new vBlob(x, y, w, h);
      obj.id = id;
      obj.status = status;

      if (!isBlob)
      {
        int idx_npts = 8;
        int n =  msg.get(idx_npts).intValue();
        for (int i=0; i<n; i++)
        { 
          float px = W*msg.get(idx_npts+1+i*2).floatValue();  
          float py = H*msg.get(idx_npts+2+i*2).floatValue();			
          //	println("("+px+", "+py+") ");
          obj.points.add(new PVector(px,py));
        }
        if (isHole)
          obj.isHole = true;
        else
          obj.isHole = false;
      }
      blobList.add(obj);		
    }
    /*
	  else if(msg.checkAddrPattern("/face")){ 
     		int x = W*msg.get(0).floatValue();  
     		int y = H*msg.get(1).floatValue();
     		int w = W*msg.get(2).floatValue();
     		int h = H*msg.get(3).floatValue();
     		print("### osc message /face");
     		println(" values: "+x+", "+y+", "+w+", "+h);
     		faceList.add(new vBlob(x, y, w, h));
     	  }
     	  */
    else if(msg.checkAddrPattern("/end"))
    {
      // println("");
      // ok_to_draw = true;
    }	
  }

} 



