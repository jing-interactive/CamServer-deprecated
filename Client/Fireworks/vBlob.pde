//store blob info via OSC
import java.util.Hashtable;

boolean ok_to_draw = true;

Hashtable blobTable = new Hashtable();

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
  ArrayList points;

  void render()
  {
    fill(100,100,100);
    cx = lerp(cx, tx, 0.3);
    cy = lerp(cy, ty, 0.3);
    rect(cx,cy,w,h);
  }
}

void oscEvent(OscMessage msg) {
  /* check if theOscMessage has the address pattern we are looking for. */
  if (msg.checkAddrPattern("/start")) {
    ok_to_draw = false;
  }
  else
  {
    boolean isBlob = msg.checkAddrPattern("/blob");
    boolean isContour = msg.checkAddrPattern("/contour");
    boolean isHole = msg.checkAddrPattern("/hole");

    if (isBlob || isContour || isHole)
    {
      print("### osc "+msg.addrPattern());  
      int id = msg.get(0).intValue();
      String status = msg.get(1).stringValue();
      Integer Key = new Integer(id);

      if (status.equals("leave"))
      {//just erase it, no need to check more
        blobTable.remove(Key);
        println(" #"+id+": "+status);
      }
      else
      {
        float cx = width*msg.get(2).floatValue();
        float cy = height*msg.get(3).floatValue();
        float w = width*msg.get(6).floatValue();
        float h = height*msg.get(7).floatValue();
        println(" #"+id+": "+status+" "+cx+", "+cy+", "+w+", "+h);

        if (blobTable.containsKey(Key) == false) 
        { //insert new blob
          vBlob obj = new vBlob(id, cx, cy, w, h);
          obj.status = status;
          blobTable.put(Key, obj);
        } 
        else 
        {//update exsited blob
          vBlob obj = (vBlob)blobTable.get(Key);
          obj.tx = cx;
          obj.ty = cy;
          obj.w = w;
          obj.h = h;
        }
      }
    }
    else if (msg.checkAddrPattern("/end"))
    {
      // println("");
      ok_to_draw = true;
    }
  }
}
