import prohtml.*;

HtmlElementFinder htmlElementFinder;

void setup(){
  //enter your url here
  htmlElementFinder = new HtmlElementFinder("http://www.yoururl.com","a"); 

  java.util.List links = htmlElementFinder.getElements();

  for (int i = 0;i<links.size();i++){
    println(((StandAloneElement)links.get(i)).getAttribute("href"));
  }
}