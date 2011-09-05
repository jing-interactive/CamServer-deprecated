prohtml.StandAloneElement standAlone;

void setup(){
  standAlone = new prohtml.StandAloneElement("img");
  standAlone.addAttribute("scr","images/getAttribute");
  
  println(standAlone.getDepth());
}