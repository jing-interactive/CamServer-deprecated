import prohtml.*;

HtmlImageFinder htmlImageFinder;

void setup(){
  
  //insert your url here
  htmlImageFinder = new HtmlImageFinder("http://www.yoururl.com");

  PImage[] images = new PImage[htmlImageFinder.getNumbOfImages()];
  
  int maxHeight=0;
  int mywidth = 0;

  for(int i = 0;i< images.length;i++){
    images[i] = loadImage(htmlImageFinder.getImageLink(i));   
    maxHeight = max(images[i].height,maxHeight);
    mywidth+=10;
  }
  
  size(mywidth,maxHeight);
  
  for(int i = 0;i< images.length;i++){
    image(images[i],i*10,0);
  }
}