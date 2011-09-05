import prohtml.*;

TextElement text;

void setup(){
  text = new TextElement("proHTML",null);
  
  println("countAllChildren:.."+text.countAllChildren());
  println("getChildren:......."+text.getChildren());
  println("getDepth:.........."+text.getDepth());
  println("hasChildren:......."+text.hasChildren());
  text.printElementTree("");println();
  println("toString:.........."+text.toString());
  println("type:.............."+text.type());
}