package prohtml;

import java.util.*;
import java.io.*;

/**
 * Use this class to directly find all elements of a given type. 
 * HtmlElementFinder parses a HtmlDocument and puts all elements matching 
 * the given type into a list. You can search for existing html elements. 
 * To search for textelements only type in &quot;text &quot;.  
 * The result of the HtmlElementFinder contains only standalone elements 
 * and text elements. This is because the tree structure of the html document 
 * gets lost parsing it into a HtmlELementFinder. 
 * There is no element having children. 
 * @example HtmlElementFinder
 * @related HtmlList
 * @related HtmlImageFinder 
 */

public class HtmlElementFinder extends HtmlCollection {

    /**
     * holds the elements matching the given type
     */

    List elements = new ArrayList();

    /**
     * holds the given type of elements
     */

    String kindOfElement;



	/**
	 * Initializes a new HtmlTree according to the given url.
	 * @param url String, url to the document you want to scan
	 * @param kindOfElement String, name of the element you look for
	 * @throws InvalidUrlException
	 */
    public HtmlElementFinder(String url, String kindOfElement) throws InvalidUrlException {
        super(url);
        this.kindOfElement = kindOfElement;
        initBeforeParsing();
        try {
            parsePage(page);
        } catch (Exception e) {
            e.printStackTrace();
            throw new InvalidUrlException(url);
        }
    }


	/**
	 * Initializes a new HtmlTree according to the given url.
	 * @param page, StringReader holding the document youwant to scan
	 * @throws InvalidUrlException
	 */
    public HtmlElementFinder(StringReader page, String url, String kindOfElement) throws InvalidUrlException {
        super(page, url);
        this.kindOfElement = kindOfElement;
        initBeforeParsing();
        try {
            parsePage(page);
        } catch (Exception e) {
            throw new InvalidUrlException(url);
        }
    }

    /**
     * This makes sure that the given kindOfElement is to UpperCase
     */

    void initBeforeParsing() {
        this.kindOfElement = kindOfElement.toUpperCase();
    }

    /**
     * Implments the handleStartTag method of HtmlCollection. All Elements matching
     * to the kindOfElement are filtered here.
     * @param sTagName String
     * @param attributes HashMap
     */

    void handleStartTag(String sTagName, HashMap attributes,boolean standAlone) {
        if (sTagName.toUpperCase().equals(this.kindOfElement)) {
            elements.add(new StandAloneElement(sTagName, attributes, null));
        }
    }

    /**
     * Just implements handleAttribute. Here nothing has to be done.
     * @param sAttributeName String
     * @param sAttributeValue String
     */

    void handleAttribute(String sAttributeName, String sAttributeValue) {
    }


    /**
     * Just an empty implementation. HEre nothing has to be done after parsing an end tag
     */

    void doAfterEndTag() {
    }


    /**
     * If the type of elements to be found is Text, text elements are to element list.
     * @param toHandle TextElement
     */

    void handleText(TextElement toHandle) {
        if (kindOfElement.equals("TEXT")) {
            elements.add(toHandle);
        }
    }

    /**
     * This methods gives you an ArrayList with the elements you looked for. 
     * @return List, of the elements you  have been looking for
     * @example HtmlElementFinder
     * @related HtmlElementFinder
     * @related printElements ( )
     */

    public List getElements() {
        return elements;
    }

    /**
     * Use this method for a simple trace of all elements in the list of the parsed html document.
     * @example HtmlElementFinder_printElements
     * @related HtmlElementFinder
     * @related getElements ( ) 
     */

    public void printElements() {
        for (int i = 0; i < elements.size(); i++) {
            System.out.println(i + " : " + elements.get(i));
        }
    }
}
