import org.processing.wiki.triangulate.*;

class vBlob
{
  public vBlob(float _cx, float _cy, float _w, float _h)
  {
    cx = _cx;
    cy = _cy;
    w = _w;
    h = _h;
    //对新添加的成员进行初始化
    isHole = false;
    points = new ArrayList();
    //  triangles = new ArrayList();
  }
  float cx,cy;//中心点的坐标
  float w,h;//宽度和高度

  public void draw()//进行绘制
  {
    if (points.size() > 0)//CamServer开启了detail模式后points会包含轮廓点，因此它的size会大于零
      drawContour();
    else//否则仅仅是显示一个白色的包围框
    {
      fill(255);
      rect( cx, cy, w, h);
    }
  }

  public void build_tri()
  {      
    triangles = Triangulate.triangulate(points);
  }

  public void draw_tri()//进行绘制
  {
    // draw the mesh of triangles
    stroke(0, 40);
    fill(255, 40);
    beginShape(TRIANGLES);

    for (int i = 0; i < triangles.size(); i++) {
      Triangle t = (Triangle)triangles.get(i);
      vertex(t.p1.x, t.p1.y);
      vertex(t.p2.x, t.p2.y);
      vertex(t.p3.x, t.p3.y);
    }

    endShape();
  }

  //++++++++++++++++第四课新添加的
  boolean isHole;//是否是洞
  ArrayList points = null;//保存轮廓点
  ArrayList triangles = null;//delaunay

  private void drawContour()
  {
    if (isHole)//如果是洞，使用不同的颜色
      fill(255,0,0);
    else
      fill(255,255,255);
    beginShape();
    for (int j=0; j<points.size();j++ )
    {
      PVector pos = (PVector)points.get(j);
      vertex(pos.x, pos.y);
    }
    endShape(CLOSE);
  }
}

