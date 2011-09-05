import prohtml.*;

StandAloneElement standAlone;

void setup(){
  standAlone = new StandAloneElement("img",null);
  standAlone.addAttribute("scr","images/getAttribute");
  standAlone.addAttribute("width","250");
  standAlone.addAttribute("height","20");

  Object[] attributes = standAlone.getAttributes();

  for(int i = 0;i<attributes.length;i++){
    println(attributes[i]);
  }
}