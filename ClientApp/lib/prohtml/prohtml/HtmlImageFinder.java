package prohtml;



import java.io.*;
import java.util.HashMap;
/**
 * Use this class to directly find all images of a htmldocument. 
 * HtmlImageFinder scans a page for images and builds the complete urls to this image. 
 * @example htmlImageFinder
 * @nosuperclasses
 * @related getImageLink ( )
 * @related getNumbOfImages ( )
 */
public class HtmlImageFinder extends HtmlElementFinder {

    boolean image = false;

    /**
     * Use this constructor to get images from an url
     * @param url String, url to the document youwant to scan  for images
     */

    public HtmlImageFinder(String url) {
        super(url, "img");
    }

    /**
     * Use this constructor to get images from a page put into a StringReader
     * @param page StringReader, holding the document youwant to scan
     */
    public HtmlImageFinder(StringReader page, String url) {
        super(page, url, "img");
    }

    /**
     * Implements the handleStartTag method of HtmlCollection. All Elements matching
     * to the kindOfElement are filtered here.
     * @param sTagName String
     * @param attributes HashMap
     */

    void handleStartTag(String sTagName, HashMap attributes, boolean standAlone) {
        if (sTagName.toUpperCase().equals(this.kindOfElement)) {
            image = true;
        }
    }

    /**
     * In this Implementation all src attributes from images are handled
     * @param sAttributeName String
     * @param sAttributeValue String
     */

    void handleAttribute(String sAttributeName, String sAttributeValue) {
        if (sAttributeName.equals("src") && image) {
            image = false;
            String imageLink = parseUrl(sAttributeValue).getCompleteUrl();
            if (!elements.contains(imageLink)) {
                elements.add(imageLink);
            }
        }
    }

    /**
     * Use this Method to get the number of all images HtmlImageFinder has found in a HTML document.
     * @return int, number of images found in the scanned page
     * @example htmlImageFinder
     * @related HtmlImageFinder
     * @related getImageLink ( ) 
     */

    public int getNumbOfImages(){
        return elements.size();
    }

    /**
     * Use this Method to get a certain image HtmlImageFinder has found in the given HTML document.
     * @param i int, number of the image you want the link for
     * @return String, link to the image
     * @example htmlImageFinder
     * @related HtmlImageFinder
     * @related getNumbOfImages ( ) 
     */

    public String getImageLink(int i){
        return (String)(elements.get(i));
    }
}
