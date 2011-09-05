import prohtml.*;

HtmlList htmlList;

void setup(){
  //enter your url here
  htmlList = new HtmlList("http://www.yoururl.com");
  
  ArrayList links = htmlList.getLinks();
  
  for(int i = 0;i<links.size();i++){
    println(((Url)(links.get(i))).toString());
  }
}