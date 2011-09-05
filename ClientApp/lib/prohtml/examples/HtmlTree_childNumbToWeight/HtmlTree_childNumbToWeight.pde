import prohtml.*;

HtmlTree htmlTree;

void setup(){
  //enter your url here
  htmlTree = new HtmlTree("http://www.yoururl.com");
  println("Weight of the body: " + 
    htmlTree.childNumbToWeight(htmlTree.pageTree.countAllChildren()));
}