import prohtml.*;

StandAloneElement standAlone;

void setup(){
  standAlone = new StandAloneElement("img",null);
  standAlone.addAttribute("scr","images/getAttribute");
  standAlone.addAttribute("width","250");
  standAlone.addAttribute("height","20");

  println(standAlone.kindOfElement);
}