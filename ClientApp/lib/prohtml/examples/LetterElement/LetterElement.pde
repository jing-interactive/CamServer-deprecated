import prohtml.*;

LetterElement letter;
               
void setup(){
  letter = new LetterElement("t");
  
  println("countAllChildren:.."+letter.countAllChildren());
  println("getChildren:......."+letter.getChildren());
  println("getDepth:.........."+letter.getDepth());
  println("hasChildren:......."+letter.hasChildren());
  letter.printElementTree("");println();
  println("toString:.........."+letter.toString());
  println("type:.............."+letter.type());
}