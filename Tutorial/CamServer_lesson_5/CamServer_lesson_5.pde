import oscP5.*;

OscP5 oscP5;

ArrayList blobList = new ArrayList();
ArrayList surfaceList = new ArrayList();//保存surface

void setup() 
{
  size(500, 480);
  smooth();

  oscP5 = new OscP5(this,3333);

  physics_setup();//初始化物理部分
}

void draw()
{
  background(122);

  for (int i=0;i<blobList.size();i++)
  {
    vBlob obj = (vBlob)blobList.get(i);
    obj.draw();
  }
  for (int i=0;i<surfaceList.size();i++)
  {//绘制带物理属性的surface
    Surface obj = (Surface)surfaceList.get(i);
    obj.display();
  }
  physics_draw();//绘制带物理属性的box
}

void mouseClicked()
{ 
  if (mouseButton == LEFT)
  {//如果按下左键
    for (int i=0;i<surfaceList.size();i++)
    {//删除所有的surface的物理信息
      Surface obj = (Surface)surfaceList.get(i);
      obj.killBody();
    }
    surfaceList.clear();//清除surfaceList

    for (int i=0;i<blobList.size();i++)
    {//用当前帧的blob来创建surface，使用的是blob的points域
      vBlob blob = (vBlob)blobList.get(i);
      surfaceList.add(new Surface(blob));
    }
  }
  else
    if (mouseButton == RIGHT)
    {//如果按下右键
      physics_clear();//清除带物理属性的box
    }
}

void oscEvent(OscMessage msg) 
{  
  if(msg.checkAddrPattern("/start")) 
  {
    blobList.clear();
  }
  else
  {
    boolean isContour = msg.checkAddrPattern("/contour");
    boolean isHole = msg.checkAddrPattern("/hole");

    if (isContour || isHole)
    {
      float cx = width*msg.get(2).floatValue();
      float cy = height*msg.get(3).floatValue();
      float w = width*msg.get(4).floatValue();
      float h = height*msg.get(5).floatValue();
      vBlob obj = new vBlob(cx, cy, w, h);

      if (isContour || isHole)
      {
        int idx_npts = 7;
        int nPts =  msg.get(idx_npts).intValue();
        for (int i=0; i < nPts; i++)
        { 
          float px = width*msg.get(idx_npts+1+i*2).floatValue();
          float py = height*msg.get(idx_npts+2+i*2).floatValue();
          obj.points.add(new PVector(px,py));
        }
        obj.isHole = isHole;
      }
      blobList.add(obj);
    }
  }
}

