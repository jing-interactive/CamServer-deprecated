import prohtml.*;

HtmlList htmlList;

void setup(){
  //enter your url here
  htmlList = new HtmlList("http://www.yoururl.com");

  java.util.List links = htmlList.getElements("IMG");

  for (int i = 0; i < links.size(); i++) {
    println(links.get(i));
  }
}