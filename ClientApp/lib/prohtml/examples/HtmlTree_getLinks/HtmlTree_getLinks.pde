import prohtml.*;

HtmlTree htmlTree;

void setup(){
  //enter your url here
  htmlTree = new HtmlTree("http://www.yoururl.com");
  java.util.List links = htmlTree.getLinks();

  for (int i = 0; i < links.size(); i++) {
    println(((prohtml.Url)(links.get(i))));
  }
}