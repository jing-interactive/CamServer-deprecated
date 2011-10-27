//store blob info via OSC

class vBlob
{
  public vBlob(float _x, float _y, float _w, float _h)
  {
    id = -1;
    status = "";
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    isHole = false;
    points = new ArrayList();
  }
  int id;
  String status;
  float cx,cy;
  float x,y,w,h;
  boolean isHole;
  ArrayList points;

  void display()
  {
    vDrawPoly(points);
  }
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

