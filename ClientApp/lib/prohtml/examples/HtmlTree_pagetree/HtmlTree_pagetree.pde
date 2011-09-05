import prohtml.*;

HtmlTree htmlTree;

void setup(){
  //enter your url here
  htmlTree = new HtmlTree("http://www.yoururl.com");
  htmlTree.pageTree.printElementTree(". ");

  println("colors:..............."+htmlTree.colors);
  println("pageTree:............."+htmlTree.pageTree);
  println("parentUrlPart:........"+htmlTree.parentUrlPart);
}