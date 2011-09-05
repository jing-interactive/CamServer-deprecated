package prohtml;

import java.util.*;
import java.io.*;

/**
 * Object for parsing a valid Html Document to convert its String into 
 * a HtmlList representing its elements. Use HtmlList if you just need 
 * a list with all elements. The element List can contain text and html 
 * elements. In the HtmlList all html elements are parsed as standalone 
 * elements. This is because the tree structure of the html document gets 
 * lost parsing it into a List. There is no element having children. 
 * @example HtmlList
 * @related pageList
 */

public class HtmlList extends HtmlCollection {

	/**
	 * PageList is a field containing a complete list of the parsed 
	 * document's elements. It contains every element, excluding comments and scripts.
	 * @example HtmlList_pagelist
	 * @shortdesc This field holds the complete List of elements.
	 * @related HtmlList
	 */
    public List pageList = new ArrayList();


    /**
     * Initializes a new HtmlList according to the given url.
     * @param url String, the url to document to be parsed
     */

    public HtmlList(String url) throws InvalidUrlException {
        super(url);
        initBeforeParsing();
        try {
            parsePage(page);
        } catch (Exception e) {
            throw new InvalidUrlException(url);
        }
    }

	/**
	 * Initializes a new HtmlList from the given StringReader.
	 * @param page StringReader, holding the document to be parsed 
	 * @param url String, the url to document to be parsed
	 * @throws InvalidUrlException
	 */
    public HtmlList(StringReader page, String url) throws InvalidUrlException {
        super(page, url);
        initBeforeParsing();
        try {
            parsePage(page);
        } catch (Exception e) {
            throw new InvalidUrlException(url);
        }
    }

    /**
     * Just implement initBeforeParsing but there is nothing to be done here.
     */

    void initBeforeParsing() {
    }

    /**
     * Implments the handleStartTag method of HtmlCollection. HtmlList puts
     * all Elements into a simple List.
     * @param sTagName String
     * @param attributes HashMap
     */

    void handleStartTag(String sTagName, HashMap attributes,boolean standAlone) {
        pageList.add(new StandAloneElement(sTagName, attributes, null));
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
     * Just an empty implementation. HEre nothing has to be done after parsing an end tag
     */

    void doAfterEndTag() {
    }

    /**
     * Puts the TextElements in the element List
     * @param toHandle TextElement
     */

    void handleText(TextElement toHandle) {
        pageList.add(toHandle);
    }


    /**
     * This methods gives you an ArrayList with the elements you looked for.
     * @param kind String, kind of element you are looking for
     * @return List, the found elements
     * @example HtmlList_getElements
     * @related HtmlList
     * @related getLinks ( ) 
     * @related getNumbOfColors ( )
     * @related getPageColor ( )
     * @related printElements ( ) 
     */

    public List getElements(String kind) {
        kind = kind.toLowerCase();
        ArrayList result = new ArrayList();
        for (int i = 0; i < pageList.size(); i++) {
            LetterElement keep = (LetterElement)pageList.get(i);
            if (keep.kindOfElement.equals(kind)) {
                result.add(keep);
            }
        }
        return result;
    }


    /**
     * This methods gives you an ArrayList with Url objects of all 
     * Links in the HTML document to other HTML documents. 
     * @return List, the found links
     * @example HtmlList_getLinks
     * @shortdesc Use this method to get all Links of a document.
     * @related HtmlList
     * @related getElements ( ) 
     * @related getNumbOfColors ( )
     * @related getPageColor ( )
     * @related Url 
     */

    public List getLinks() {
        List links = getElements("a");
        ArrayList result = new ArrayList();
        for (int i = 0; i < links.size(); i++) {
            try {
                StandAloneElement keep = ((StandAloneElement)links.get(i));
                result.add(parseUrl(keep.getAttribute("href")));
            } catch (Exception e) {
            }
        }
        links = getElements("FRAME");
        for (int i = 0; i < links.size(); i++) {
            try {
                StandAloneElement keep = ((StandAloneElement)links.get(i));
                result.add(parseUrl(keep.getAttribute("src")));
            } catch (Exception e) {
            }
        }

        return result;
    }

    /**
     * Use this method for a simple trace of all elements in the list of the parsed html document. 
     * @example HtmlList
     * @related HtmlList
     * @related getNumbOfColors ( )
     * @related getPageColor ( )
     */

    public void printElements() {
        for (int i = 0; i < pageList.size(); i++) {
            System.out.println(i + " : " + pageList.get(i));
        }
    }
}
