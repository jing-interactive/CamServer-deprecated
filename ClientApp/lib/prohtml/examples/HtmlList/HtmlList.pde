import prohtml.*;

HtmlList htmlList;

void setup(){
  //enter your url here
  htmlList = new HtmlList("http://www.yoururl.com");
  
  htmlList.printElements();
}