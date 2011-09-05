import prohtml.*;

StandAloneElement standAlone;

void setup(){
  standAlone = new StandAloneElement("img",null);
  standAlone.addAttribute("scr","images/getAttribute");
  println(standAlone.getAttribute("scr"));
}