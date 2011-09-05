import prohtml.*;

HtmlTree htmlTree;

void setup(){
  //enter your url here
  htmlTree = new HtmlTree("http://www.yoururl.com");

  ArrayList links = htmlTree.getLinks();

  for(int i = 0;i<links.size();i++){
    println("**********************************************");
    println("Url_"+nf(i,3)+" : "+
      ((Url)(links.get(i))).toString());
    println(" getCompleteUrl: "+
      ((Url)(links.get(i))).getCompleteUrl());
    println(" getDomain : "+
      ((Url)(links.get(i))).getDomain());
    println(" getParentPart : "+
      ((Url)(links.get(i))).getParentPart());
    println(" getUrl : "+
      ((Url)(links.get(i))).getUrl());
    println(" isAbsolute : "+
      ((Url)(links.get(i))).isAbsolute());
  }
}