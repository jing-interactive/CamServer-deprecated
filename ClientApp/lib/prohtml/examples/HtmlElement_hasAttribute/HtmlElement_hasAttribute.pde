import prohtml.*;

HtmlElement table;

void setup(){
  table = new HtmlElement("table",null);
  table.addAttribute("width","200");

  HtmlElement tr1 = new HtmlElement("tr",table);
  HtmlElement td1 = new HtmlElement("td",tr1);
  HtmlElement td2 = new HtmlElement("td",tr1);

  td1.addChild(new TextElement("1",td1));
  td2.addChild(new TextElement("2",td2));
  tr1.addChild(td1);
  tr1.addChild(td2);
  table.addChild(tr1);

  HtmlElement tr2 = new HtmlElement("tr",table);
  HtmlElement td3 = new HtmlElement("td",tr2);
  HtmlElement td4 = new HtmlElement("td",tr2);

  td3.addChild(new TextElement("3",td3));
  td4.addChild(new TextElement("4",td4));
  tr2.addChild(td3);
  tr2.addChild(td4);
  table.addChild(tr2);

  println(table.hasAttribute("width"));
}