import oscP5.*;
import netP5.*;

OscP5 oscP5;//OSC对象

class vBlob
{
  public vBlob(float _cx, float _cy, float _w, float _h)
  {
    cx = _cx;
    cy = _cy;
    w = _w;
    h = _h;
  }
  float cx,cy;//中心点的坐标
  float w,h;//宽度和高度

  void draw()//进行绘制
  {
    rect( cx, cy, w, h);
  }
}

ArrayList blobList = new ArrayList();

void setup() {
  size(640, 480);
  oscP5 = new OscP5(this,3333);//3333是CamServer的端口号，我提到过一次的
}

void draw()
{
  background(122);

  for (int i=0;i<blobList.size();i++)
  {
    vBlob obj = (vBlob)blobList.get(i);
    obj.draw();
  }
}

void oscEvent(OscMessage msg) 
{
  if(msg.checkAddrPattern("/start")) 
  {// 收到 /start 意味着一次更新的开始
    blobList.clear();//将blobList清空，因为我们要把全新的数据塞进去了
  }
  else
  {
    boolean isBlob = msg.checkAddrPattern("/blob");
    boolean isContour = msg.checkAddrPattern("/contour");
    boolean isHole = msg.checkAddrPattern("/hole");
    // 这三者的含义上节课都有讲哦

    if (isBlob || isContour || isHole)
    {
      float cx = width*msg.get(2).floatValue();
      float cy = height*msg.get(3).floatValue();
      float w = width*msg.get(6).floatValue();
      float h = height*msg.get(7).floatValue();
      vBlob obj = new vBlob(cx, cy, w, h);
      
      blobList.add(obj);
    }
  }
}



