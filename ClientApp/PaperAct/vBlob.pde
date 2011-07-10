//store blob info via OSC
import java.util.Hashtable;

boolean ok_to_draw = true;

Hashtable blobTable = new Hashtable();

class vBlob
{
  public vBlob(int _id, float _cx, float _cy, float _w, float _h, float _a)
  {
    id = _id;
    cx = tx = _cx;
    cy = ty = _cy;
    w = tw = _w;
    h = th = _h;
    angle = ta = _a;
  }
  int id;
  String status;
  float cx, cy;
  float tx, ty;
  // float x,y;
  float w, h;
  float tw, th;
  boolean isHole = false;
  ArrayList points = new ArrayList();
  boolean drawing = false;
  FPoly poly = null;
  float angle, ta;

  void draw()
  {
    noFill();
    cx = lerp(cx, tx, 0.3);
    cy = lerp(cy, ty, 0.3);
    angle = lerp(angle, ta, 0.1);
    w = lerp(w, tw, 0.1);
    h = lerp(h, th, 0.1);

    pushMatrix();
    {
      translate(cx, cy);
      rotate(angle);    
      rect(0, 0, w, h);
    }
    popMatrix();
    drawContour();
  }

  private void drawContour()
  {
    if (isHole)//如果是洞，使用不同的颜色
      fill(255, 0, 0);
    else
      fill(255, 255, 255);
    beginShape();
    for (int j=0; j<points.size();j++ )
    {
      PVector pos = (PVector)points.get(j);
      vertex(pos.x, pos.y);
    }
    endShape(CLOSE);
  }
}

void oscEvent(OscMessage msg)
{
  /* check if theOscMessage has the address pattern we are looking for. */
  if (msg.checkAddrPattern("/start")) 
  {
    ok_to_draw = false;
    return;
  }

  if (msg.checkAddrPattern("/end"))
  {
    ok_to_draw = true;
    return;
  }

  boolean isContour = msg.checkAddrPattern("/contour");
  boolean isHole = msg.checkAddrPattern("/hole");

  if (isContour || isHole)
  {
    String status = msg.get(1).stringValue();
    print("###"+msg.addrPattern());  
    int id = msg.get(0).intValue();
    Integer Key = new Integer(id);

    if (status.equals("leave"))
    {//just erase it, no need to check more
      blobTable.remove(Key);
      println(" #"+id+": "+status);
      return;
    }

    if (isContour)
    {
      float cx = width*msg.get(2).floatValue();
      float cy = height*msg.get(3).floatValue();
      float w = width*msg.get(4).floatValue();
      float h = height*msg.get(5).floatValue();
      float angle = msg.get(6).floatValue();
      println(" #"+id+": "+status+" "+cx+", "+cy+", "+w+", "+h);

      vBlob obj = null;
      if (blobTable.containsKey(Key) == false) 
      { //insert new blob
        obj = new vBlob(id, cx, cy, w, h, angle);
        blobTable.put(Key, obj);
      }
      else 
      {//update exsited blob
        obj = (vBlob)blobTable.get(Key);
        obj.status = status;
        obj.tx = cx;
        obj.ty = cy;
        obj.w = w;
        obj.h = h;
        obj.angle = angle;        

        while (obj.drawing) {
          println("waiting for "+id);
        }//waiting
        obj.points.clear();
      }
      int idx_npts = 7;
      int nPts =  msg.get(idx_npts).intValue();
      for (int i=0; i < nPts; i++)
      { 
        float px = width*msg.get(idx_npts+1+i*2).floatValue();
        float py = height*msg.get(idx_npts+2+i*2).floatValue();
        obj.points.add(new PVector(px, py));
      }
    }
  }
}

