//store blob info via OSC

int offset = 0;//modify this value

class vBlob
{
  public vBlob(int _id, float _cx, float _cy, float _w, float _h)
  {
    id = _id;
    cx = tx = _cx;
    cy = ty = _cy;
    w = _w;
    h = _h;
    isHole = false;
  }
  int id;
  String status;
  float cx,cy;
  float tx,ty;
  // float x,y;
  float w,h;
  boolean isHole;
  //  ArrayList points;
  String info;

  void render()
  {
    cx = lerp(cx, tx, 0.3);
    cy = lerp(cy, ty, 0.3);
    text(info, cx, cy+offset); 
  }
}


