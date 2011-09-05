import prohtml.*;

HtmlElementFinder htmlElementFinder;

void setup(){
  //insert your url here
  htmlElementFinder = new HtmlElementFinder("http://www.yoururl.com","a");

  htmlElementFinder.printElements();
}