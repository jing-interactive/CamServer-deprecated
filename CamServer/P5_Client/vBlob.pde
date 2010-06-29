//store blob info via OSC

class vBlob
{
  public vBlob(int _x, int _y, int _w, int _h)
  {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    isHole = false;
    points = new ArrayList();
  }
  int x,y,w,h;
  boolean isHole;
  ArrayList points;
  
  void display()
  {
    vDrawPoly(points);
  }
}
