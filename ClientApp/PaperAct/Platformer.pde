import oscP5.*;
PImage bg;

OscP5 oscP5; 

void setup() {
  frameRate(30);
  bg = loadImage("back.jpg");
  size(bg.width, bg.height);
  oscP5 = new OscP5(this, 3333);
  rectMode(CENTER);
  physics_setup();
}

void draw()
{
  if (!ok_to_draw)
    return;
    
  background(bg);
  // Get Hashtable Enumeration to get key and value
  Enumeration em=blobTable.keys();
  while (em.hasMoreElements ())
  {
    //nextElement is used to get key of Hashtable
    int key = (Integer)em.nextElement();

    //get is used to get value of key in Hashtable
    vBlob obj=(vBlob)blobTable.get(key);
    obj.drawing = true;
    obj.draw();
    physics_update_blob(obj);
    obj.drawing = false;
  }
  physics_draw();
}

void keyPressed()
{
 physics_clear(); 
}
