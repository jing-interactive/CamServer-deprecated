import oscP5.*;

OscP5 oscP5;//OSC对象

ArrayList blobList = new ArrayList();
ArrayList surfaceList = new ArrayList();

void setup() {
  size(500, 480);
  smooth();

  oscP5 = new OscP5(this,3333);//3333是CamServer的端口号，我提到过一次的

  physics_setup();
}

void draw()
{
  background(122);

  for (int i=0;i<blobList.size();i++)
  {
    vBlob obj = (vBlob)blobList.get(i);
    obj.draw();

    // obj.build_tri();
    // obj.draw_tri();
  }
  for (int i=0;i<surfaceList.size();i++)
  {
    Surface obj = (Surface)surfaceList.get(i);
    obj.display();
  }
  physics_draw();
}

void mouseClicked()
{ 
  if (mouseButton == LEFT)
  {
    for (int i=0;i<surfaceList.size();i++)
    {
      Surface obj = (Surface)surfaceList.get(i);
      obj.killBody();
    }  
    surfaceList.clear();

    for (int i=0;i<blobList.size();i++)
    {
      vBlob blob = (vBlob)blobList.get(i);
      surfaceList.add(new Surface(blob));
    }
  }
  else
    if (mouseButton == RIGHT)
    {
      physics_clear();
    }
}

void oscEvent(OscMessage msg) 
{  
  if(msg.checkAddrPattern("/start")) 
  {// 收到 /start 意味着一次更新的开始
    blobList.clear();//将blobList清空，因为我们要把全新的数据塞进去了
    // prev_blob = null;
    /*  for (int i=0;i<surfaceList.size();i++)
     {
     Surface obj = (Surface)surfaceList.get(i);
     obj.killBody();
     }
     surfaceList.clear();*/
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

      //++++++++++++++++第四课新添加的
      if (isContour || isHole)
      {
        int idx_npts = 8;//顶点个数处于消息的第8位，见第三课中的消息定义
        int nPts =  msg.get(idx_npts).intValue();
        for (int i=0; i < nPts; i++)//依次读取每个坐标(x, y)，添加到obj的points数组中。
        { 
          float px = width*msg.get(idx_npts+1+i*2).floatValue();
          float py = height*msg.get(idx_npts+2+i*2).floatValue();
          obj.points.add(new PVector(px,py));
        }
        obj.isHole = isHole;
      }
      blobList.add(obj);

      //prev_blob = obj;

      //surfaceList.add(new Surface(obj));
    }
  }
}

