import prohtml.*;

HtmlList htmlList;

void setup(){
  size(100,100);
  //enter your url here
  htmlList = new HtmlList("www.http://yoururl.com");

  for (int i = 0;i<htmlList.pageList.size();i++){
    println(htmlList.pageList.get(i));
  }
}