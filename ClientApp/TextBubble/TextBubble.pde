//press ctrl+F will exit fullscreen mode
//press again to return fullscreen mode

import oscP5.*;
import netP5.*;
import java.util.Hashtable;

PImage bg;

PFont myFont;
String lines[];
String one_line;

OscP5 oscP5; 
Hashtable blobTable = new Hashtable();
ArrayList blobList = new ArrayList();

void setup() {
  size(1024, 768);
  frameRate(30);
  bg = loadImage("back.jpg");

  oscP5 = new OscP5(this,3333);

  myFont = createFont("黑体", 32);
  textFont(myFont);

  lines = loadStrings("data.txt");
  one_line = lines[0];
}

int j = 0;//index of line
int i = 0;//index of character

String getNextStr()
{
  if (++j > one_line.length()-2)
  { 
    j = 0;
    if (++i > lines.length-1)
      i = 0;
    one_line = lines[i];
  }

  return one_line.substring(j,j+1);
}

boolean ok_to_draw = true;

void draw()
{
  if (!ok_to_draw)
    return;

  background(bg); 

  // Get Hashtable Enumeration to get key and value
  Enumeration em=blobTable.keys();

  while(em.hasMoreElements())
  {
    //nextElement is used to get key of Hashtable
    int key = (Integer)em.nextElement();

    //get is used to get value of key in Hashtable
    vBlob obj=(vBlob)blobTable.get(key);
    obj.render(); 
    println(obj.status);
  }
  /*
  for (int i=0;i<blobList.size();i++)
   {
   vBlob obj = (vBlob)blobList.get(i);
   obj.render();
   }
   */
}

void oscEvent(OscMessage msg) {
  /* check if theOscMessage has the address pattern we are looking for. */
  if(msg.checkAddrPattern("/start")) {
    // println("");
    ok_to_draw = false;
    //   blobList.clear();
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
          obj.info = getNextStr();
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
    else if(msg.checkAddrPattern("/end"))
    {
      // println("");
      ok_to_draw = true;
    }
  }
} 

