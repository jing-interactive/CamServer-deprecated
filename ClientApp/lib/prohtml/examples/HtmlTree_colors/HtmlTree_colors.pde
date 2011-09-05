import prohtml.*;

HtmlTree htmlTree;
int numbOfColors,stripeWidth;

void setup(){
  //enter your url here
  htmlTree = new HtmlTree("http://www.yoururl.com");

  numbOfColors = htmlTree.getNumbOfColors();
  stripeWidth = width/numbOfColors;
}

void draw(){
  if(!(numbOfColors < 1)){
    int stripeWidth = width/numbOfColors;
    for (int i = 0;i<numbOfColors;i++){
      noStroke();
      fill(htmlTree.getPageColor(i));
      rect(i*stripeWidth,0,stripeWidth,height);
    }
  }else{
    println("no color in html code");
  }
}