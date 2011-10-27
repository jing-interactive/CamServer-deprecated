import oscP5.*;
import netP5.*;

OscP5 oscP5;//OSC对象

void setup() {
  size(640, 480);
  oscP5 = new OscP5(this,3333);//3333是CamServer的端口号，我提到过一次的
}
void draw()
{
  background(122);
}

void oscEvent(OscMessage msg) 
{  
  boolean isBlob = msg.checkAddrPattern("/blob");
  boolean isContour = msg.checkAddrPattern("/contour");
  boolean isHole = msg.checkAddrPattern("/hole");
  if (isBlob || isContour || isHole)
  {
    println("we find a monster here");
  }  
}


