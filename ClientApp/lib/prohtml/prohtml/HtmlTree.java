package prohtml;

import java.util.*;
import java.io.*;

/**
 * Object for parsing a valid Html Document to convert its String 
 * into a HtmlTree representing its structure. After successful 
 * initialisation of the tree you can use different methods and 
 * fields to get information of the page. <br>
 * The Example prints the tree structure of the processing reference 
 * page and the values of the fields of the initialised HtmlTree object. 
 * @example HtmlTree
 * @related pageTree
 */

public class HtmlTree extends HtmlCollection {


    /**
     * HashMap to hold the weights of the Element, they correspond to its MaximumChildNumbs
     */
    private HashMap weights;

    /**
     * PageTree is a field containing the tree structure of the document. 
     * It contains every element, excluding comments and scripts. 
     * PageTree is a HtmlElement so see its methods for further information.
     * @shortdesc This field holds the treestructure of the parsed document.
     * @example HtmlTree_pagetree
     * @related HtmlTree
     */
    public HtmlElement pageTree;

    /**
     * field to keep the parent element to put the children elements in
     */
    private HtmlElement actualElement;


    /**
     * Initializes a new HtmlTree according to the given url.
     * @param url String, url to be loaded
     */

    public HtmlTree(String url) throws InvalidUrlException {
        super(url);
        initBeforeParsing();
        parsePage(page);
        weights = this.getAllChildrenNumbs();
    }

    /**
     * Initializes a new HtmlTree from a StringReader holding the document.
     * @param page StringReader, holds the document
     * @param url String, url of  the document
     * @throws InvalidUrlException
     */

    public HtmlTree(StringReader page, String url) throws InvalidUrlException {
        super(page, url);
        initBeforeParsing();
        try {
            parsePage(page);
        } catch (Exception e) {
            throw new InvalidUrlException(url);
        }
        weights = this.getAllChildrenNumbs();
    }


    /**
     * This makes sure that the given kindOfElement is to UpperCase
     */

    void initBeforeParsing() {
        pageTree = new HtmlElement("page", null);
        actualElement = pageTree;
    }

    /**
     * Implments the handleStartTag method of HtmlCollection. Here all Elements
     * are placed in the tree structure, according to its position in the page.
     * @param sTagName String
     * @param attributes HashMap
     */

    void handleStartTag(String sTagName, HashMap attributes, boolean standAlone) {
        try{
            if (isStandAlone(sTagName)||standAlone) {
                actualElement.addChild(new StandAloneElement(sTagName, attributes, actualElement));
            } else {
                HtmlElement keep = new HtmlElement(sTagName, attributes, actualElement);
                actualElement.addChild(keep);
                actualElement = keep;
            }
        }catch(Exception e){
            throw new InvalidUrlException();
        }
    }

    /**
     * String Array with all possible standalone Tags
     * @invisible
     */

    public static final String[] standAloneTags = {"area", "base", "basefont", "br", "col", "frame", "hr", "img", "input", "isindex",
                                                  "link", "meta", "param", "doctype"
    };

    /**
     * Checks if the given Tag is a standalone tag
     * @param kindOfElement String
     * @return boolean
     */

    private boolean isStandAlone(String kindOfElement) {
        for (int i = 0; i < standAloneTags.length; i++) {
            if (standAloneTags[i].equals(kindOfElement)) {
                return true;
            }
        }
        return false;
    }


    /**
     * In this Implementation all colorAttributes during parsing are stored
     * @param sAttributeName String
     * @param sAttributeValue String
     */

    void handleAttribute(String sAttributeName, String sAttributeValue) {
        if (sAttributeName.equals("bgcolor") || sAttributeName.equals("color")) {
            Integer keep = new Integer(hexToColor(sAttributeValue));
            if (!colors.contains(keep)) {
                colors.add(keep);
            }
        }
    }

    /**
     * After parsing an end tag. The acualElement is its parent Element
     */

    void doAfterEndTag() {
        try{
            actualElement = actualElement.getParent();
        }catch(Exception e){

            System.out.println("+++++++++++"+actualElement);
            throw new InvalidUrlException();
        }
    }

    /**
     * Places the text elements in the tree structure
     * @param toHandle TextElement
     */

    void handleText(TextElement toHandle) {
        toHandle.parent = actualElement;
        actualElement.addChild(toHandle);
    }


    /**
     * This method converts the number of descendants of an element of the HTML document, 
     * into a float value between 1 and 20. The values result from taking the number 
     * of all descendants from each element. Now the programm takes all different values 
     * and spreads them over the range from 1 to 20. <br>
     * For example the values 1234, 444, 344, 322, 234, 233, 232, 230,44,33 would result in
     * the following values: 1.0, 2.9, 4.8, 6.7, 8.6, 10.5, 12.4, 14.299999, 16.2, 18.099998. <br>
     * So the exponentially increasing numbers of descendants are changed into 
     * proportionally growing float values. 
     * @param childNumb int, the number of childs
     * @return float, the weight for the number of childs
     * @shortdesc This method converts the number of descendants of an element of the HTML document, 
     * into a float value between 1 and 20.
     * @example HtmlTree_childNumbToWeight
     * @related HtmlTree
     * @related getLinks ( ) 
     * @related getNumbOfColors ( )
     * @related getPageColor ( )
     */
    public float childNumbToWeight(int childNumb) {
        return ((Float)weights.get(new Integer(childNumb))).floatValue();
    }

    /**
     * Builds a HashMap with all different ChildNumbs of a document and the corresponding weights
     * @return HashMap
     */

    private HashMap getAllChildrenNumbs() {
        Object[] allChildNumbs = getAllChildrenNumbs(pageTree).toArray();
        HashMap result = new HashMap();
        float max = allChildNumbs.length;
        float step = max / 19;
        for (int i = 0; i < allChildNumbs.length; i++) {
            result.put(allChildNumbs[i], new Float(i / step + 1));
        }
        return result;
    }

    /**
     * Gives back a Treeset with all ChildrenNumbs in the Element
     * @param toCount Element
     * @return TreeSet
     */

    private TreeSet getAllChildrenNumbs(Element toCount) {
        TreeSet result = new TreeSet();
        List toCountChildren = toCount.getChildren();
        for (int i = 0; i < toCountChildren.size(); i++) {
            result.addAll(getAllChildrenNumbs(((Element)toCountChildren.get(i))));
        }
        result.add(new Integer(toCount.countAllChildren()));
        return result;
    }

    /**
     * This methods gives you an ArrayList with Url objects of all Links in the HTML document to other HTML documents. 
     * @return List, with the found Url objects
     * @example HtmlTree_getLinks
     * @related HtmlTree
     * @related childNumbToWeight ( ) 
     * @related getNumbOfColors ( )
     * @related getPageColor ( )
     * @related Url 
     */

    public List getLinks() {
        List links = pageTree.getSpecificElements("a");
        ArrayList result = new ArrayList();
        for (int i = 0; i < links.size(); i++) {
            try {
                HtmlElement keep = ((HtmlElement)links.get(i));
                result.add(parseUrl(keep.getAttribute("href")));
            } catch (Exception e) {
            }
        }
        links = pageTree.getSpecificElements("FRAME");
        for (int i = 0; i < links.size(); i++) {
            try {
                StandAloneElement keep = ((StandAloneElement)links.get(i));
                result.add(parseUrl(keep.getAttribute("src")));
            } catch (Exception e) {
            }
        }
        links = pageTree.getSpecificElements("META");
        for (int i = 0; i < links.size(); i++) {
            try {
                StandAloneElement keep = ((StandAloneElement)links.get(i));
                if(keep.hasAttribute("http-equiv")){
                    String attValue = keep.getAttribute("http-equiv");
                    if(attValue.equals("refresh")){
                        attValue = keep.getAttribute("content");
                        attValue = attValue.substring(attValue.lastIndexOf("=")+1,attValue.length());
                        result.add(parseUrl(attValue));
                    }
                }
            } catch (Exception e) {
            }
        }
        return result;
    }
	
	/**
	 * 
	 * @param args
	 * @invisible
	 */
	public static void main(String[] args) {
		HtmlTree test = null;
	
		try{
			test = new HtmlTree("http://www.texone.org/");
		}catch(Exception e){
			
		}
		TreeSet links = new TreeSet();
		TreeSet childLinks = new TreeSet();
		TreeSet allreadyBuilt = new TreeSet();
		String url = "";
		String path = "";

		List linkis = test.getLinks();
		for (int i = 0;i< linkis.size();i++){
			links.add((Url)(linkis.get(i)));
		}
		while (true){
			Url nextUrl;
			try{
				if (links.isEmpty()){
					System.out.println("add new:" + childLinks.size() + ":" + allreadyBuilt.size());
					childLinks.removeAll(allreadyBuilt);
					links = childLinks;
					childLinks = new TreeSet();
				}
				nextUrl = (Url) links.last();
				links.remove(nextUrl);
			}catch (NoSuchElementException e){
				break;
			}
			try{
				try{
					url = nextUrl.getCompleteUrl();
				}catch (Exception e){
					url = nextUrl.getDomain() + nextUrl.getUrl();

				}
				System.out.println("make        : " + url);
				test = new HtmlTree(url);
				allreadyBuilt.add(nextUrl);
				List newLinks = test.getLinks();

				for (int i = 0; i < newLinks.size(); i++){
					if (!allreadyBuilt.contains(newLinks.get(i))){
						childLinks.add(newLinks.get(i));
					}
				}
				System.out.println("made:         " + url);
				childLinks.addAll(test.getLinks());
			}catch (InvalidUrlException e){
				System.out.println("problem with: " + url);
				e.printStackTrace();
			}
		}
	}
}
