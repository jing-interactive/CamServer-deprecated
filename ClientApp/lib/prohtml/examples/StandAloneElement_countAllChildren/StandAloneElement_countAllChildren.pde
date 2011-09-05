import prohtml.*;

StandAloneElement standAlone;

void setup(){
  standAlone = new StandAloneElement("img",null);
  println("countAllChildren:.."+standAlone.countAllChildren());
}