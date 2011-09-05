package prohtml;

import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Basic class for parsing html doucments.
 * @invisible
 */

public abstract class HtmlCollection {
    /**
     * Arraylist holding the colors found in the page
     */
    public ArrayList colors;


    /**
     * Keeps the absolutepart (without file information) of the url of this document.
     */

    public String parentUrlPart;

    /**
     * String to keep the String of the page to parse
     */
    Reader page, keep;

    /**
     * Initializes a new HtmlTree according to the given url.
     * @param url String
     */

    public HtmlCollection(String url) throws InvalidUrlException {
        try {
            parentUrlPart = getParentUrlPart(url);
            page = openHtml(url);
            keep = openHtml(url);
            colors = new ArrayList();
        } catch (Exception e) {
            throw new InvalidUrlException(url,e);
        }
    }

    /**
     * Initializes the collection with a StringReader to parse
     * @param url String
     * @throws InvalidUrlException
     */

    public HtmlCollection(StringReader page, String url) throws InvalidUrlException {
        try {
            parentUrlPart = getParentUrlPart(url);
            this.page = page;
            colors = new ArrayList();
        } catch (Exception e) {
            throw new InvalidUrlException();
        }
    }

	/**
	 * Use this MEthod to get the HTMLSourcecode of the HtmlCollection as String
	 * @return String, source of the HtmlCollection Object
	 * @invisible
	 */
    public String getSource() {
        int iChar;
        char cchar;
        StringBuffer result = new StringBuffer();
        try {
            while ((iChar = keep.read()) != -1) {
                result.append((char)iChar);
            }
        } catch (Exception e) {
            return ("fails");
        }
        return result.toString();
    }


    /**
     * Implement this method to initialize values that you need for your parsing
     */

    abstract void initBeforeParsing();

    private boolean firstTag = false;

    /**
     * Parses a given String and gives back box with the parsed Element and the
     * String still have to be parsed.
     * @param toParse String
     * @return BoxToParseElement
     */

    void parsePage(Reader page) {

        firstTag = true;

        int iChar; //keeps the int value of the current char
        char cChar; //keeps the char value of the current char

        StringBuffer sbText = new StringBuffer(); //StringBuffer to parse words in
        boolean bText = false; //has a word been parsed
        boolean bSpaceBefore = false;
        try {
            while ((iChar = page.read()) != -1) { //as long there is something to read
                cChar = (char)iChar; //get the current char value

                switch (cChar) { //check the char value
					case '\b':
						break;
					case '\n':
						break;
					case '\f':
						break;
					case '\r':
						break;
					case '\t':
						break;
                    case '<': //this opens a tag so...

                        if (bText) {
                            bText = false;
                            handleText(new TextElement(sbText.toString(), null));
                            sbText = new StringBuffer();
                        }
                        if ((iChar = page.read()) != -1) { //check the next sign...
                            cChar = (char)iChar; //get its char value..
                            if (cChar == '/') { //in this case we have an end tag
                                page = handleEndTag(page); // and handle it
                            } else if (cChar == '!') { //this could be a comment, but we need a further test
                                if ((iChar = page.read()) != -1) { //you should know this now
                                    cChar = (char)iChar; //also this one
                                    if (cChar == '-') { //okay its a comment
                                        page = handleComment(page); //handle it
                                    } else {
                                        page = handleStartTag(page, new StringBuffer().append(cChar)); //its not a comment but a tag to be handled
                                    }
                                }
                            } else {
                                page = handleStartTag(page, new StringBuffer().append(cChar)); //no special case just handle  a tag
                            }
                        }

                        break;
                    default:
                        if (cChar == ' ') {
                            if (!bSpaceBefore && bText) {
                                handleText(new TextElement(sbText.toString(), null));
                                sbText = new StringBuffer();
                            }
                            bSpaceBefore = true;
                        } else if (Character.getNumericValue(cChar) < 0) {
                        } else {
                            bSpaceBefore = false;
                            bText = true;
                            sbText.append(cChar);
                        }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Parses a Tag and extracts its Name and Attributes.
     * @param page Reader
     * @param alreadyParsed StringBuffer
     * @return Reader
     * @throws Exception
     */

    Reader handleStartTag(Reader page, StringBuffer alreadyParsed) throws Exception {
        int iChar;
        char cChar;

        boolean bTagName = true;
        boolean bSpaceBefore = false;
        boolean bLeftAttribute = false;

        StringBuffer sbTagName = alreadyParsed;
        StringBuffer sbAttributeName = new StringBuffer();
        StringBuffer sbAttributeValue = new StringBuffer();
        StringBuffer sbActual = sbTagName;

        HashMap attributes = new HashMap();
        boolean inValue = false;
        char oChar = ' ';


        while ((iChar = page.read()) != -1) {
            cChar = (char)iChar;
            switch (cChar) {
				case '\b':
					break;
				case '\n':
					break;
				case '\f':
					break;
				case '\r':
					break;
				case '\t':
					break;
                case ' ':
                    if (!bSpaceBefore) {
                        if (!inValue) {
                            if (bTagName) {
                                bTagName = false;
                            } else {
                                String sAttributeName = sbAttributeName.toString().toLowerCase();
                                String sAttributeValue = sbAttributeValue.toString().toLowerCase();
                                attributes.put(sAttributeName, sAttributeValue);

                                handleAttribute(sAttributeName, sAttributeValue);
                                sbAttributeName = new StringBuffer();
                                sbAttributeValue = new StringBuffer();
                                bLeftAttribute = false;
                            }
                            sbActual = sbAttributeName;
                        } else {
                            sbActual.append(cChar);
                        }
                    }
                    bSpaceBefore = true;
                    break;
                case '=':
                    if (!inValue) {
                        sbActual = sbAttributeValue;
                        bLeftAttribute = true;
                    } else {
                        sbActual.append(cChar);
                    }
                    break;
                case '"':
                    inValue = !inValue;
                    break;
                case '\'':
                    break;
                case '/':
					if(inValue)sbActual.append(cChar);
                    break;
                case '>':
                    if (bLeftAttribute) {
                        String sAttributeName = sbAttributeName.toString().toLowerCase();
                        String sAttributeValue = sbAttributeValue.toString().toLowerCase();
                        attributes.put(sAttributeName, sAttributeValue);
                        handleAttribute(sAttributeName, sAttributeValue);
                    }
                    String sTagName = sbTagName.toString().toLowerCase();
                    if(firstTag){
                        firstTag = false;
                        if(!(sTagName.equals("doctype")||sTagName.equals("html")||sTagName.equals("?xml")))throw new InvalidUrlException();
                    }
                    if (oChar == '/'){
                        handleStartTag(sTagName, attributes,true);
                    }else{
                        handleStartTag(sTagName, attributes,false);
                    }
                    return page;

                default:
                    bSpaceBefore = false;
                    sbActual.append(cChar);
            }
            oChar = cChar;
        }

        throw new InvalidUrlException();
    }

    /**
     * In this Method you have to define how parsed Starttags are handles.
     * For Example HtmlElementFinder filters the elements according to its given
     * kindOfElement in its implementation of this method.
     * @param sTagName String
     * @param attributes HashMap
     */

    abstract void handleStartTag(String sTagName, HashMap attributes,boolean standAlone);

    /**
     * Implementing this Method gives you the possibility to handle Attributes during parsing.
     * For Example HtmlTree save the color values of a page right here.
     * @param sAttributeName String
     * @param sAttributeValue String
     */

    abstract void handleAttribute(String sAttributeName, String sAttributeValue);

    /**
     * Parses the end tags of a html document
     * @param toParse Reader
     * @return Reader
     * @throws Exception
     */

    Reader handleEndTag(Reader toParse) throws Exception {
        int iChar;
        char cChar;

        while ((iChar = toParse.read()) != -1) {

            cChar = (char)iChar;
            switch (cChar) {
				case '\b':
					break;
				case '\n':
					break;
				case '\f':
					break;
				case '\r':
					break;
				case '\t':
					break;
                case '>':
                    doAfterEndTag();
                    return toParse;
                default:
            }
        }
        throw new InvalidUrlException();

    }

    /**
     * In this method you can call operations being executed after parsing an end tag.
     */

    abstract void doAfterEndTag();

    /**
     * Parses the comments of a htmldocument
     * @param toParse Reader
     * @return Reader
     * @throws Exception
     */

    Reader handleComment(Reader toParse) throws Exception {
        int iChar;
        char cChar;
        char prevChar = ' ';

        while ((iChar = toParse.read()) != -1) {
            cChar = (char)iChar;
            if (prevChar == '-' && cChar == '>') {
                return toParse;
            }
            prevChar = cChar;
        }
        throw new InvalidUrlException();
    }


    /**
     * You have to implement this method to describe what happens to TextElements
     * @param toHandle TextElement
     */

    abstract void handleText(TextElement toHandle);


    /**
     * Gives you the absolute part of an url without any file parts.
     * http://www.mypage.de/index.html becomes http://www.mypage.de/
     * @param url String
     * @return String
     */

    String getParentUrlPart(String url) {
        File test = new File(url);
        File keep = test;
        int countDepth = 0;
        while (true) {
            keep = keep.getParentFile();
            if (keep == null) {
                break;
            }
            countDepth++;
        }
        if (countDepth > 1 && url.lastIndexOf("/") < url.lastIndexOf(".")) {
            return url.substring(0, url.lastIndexOf("/"));
        } else {
            return url;
        }
    }

    String getDomain(String url) {
        File test = new File(url);
        File keep = test;
        int countDepth = 0;
        while (keep.getParentFile() != null) {
            test = keep;
            keep = keep.getParentFile();
        }
        return test.getPath().replaceAll("\\\\", "//");
    }

    /**
     * Opens the document from a specific Url
     * @param url String
     * @throws IllegalArgumentException
     * @return String
     */

    Reader openHtml(String url) throws InvalidUrlException {
        try {
            return openHtmlFile(url);
        } catch (Exception e) {
            //System.out.println(e);
        }

        try {
            return openHtmlUrl(url);
        } catch (Exception e) {
            //System.out.println("not open:" + url);
            //e.printStackTrace();
        }

        try {
            return openHtmlUrl("open_url.php?url=" + url);
        } catch (Exception e) {
            //System.out.println(e);
        }

        throw new InvalidUrlException();
    }

    /**
     * Returns the Reader connected to the source of a given File
     * @param file String
     * @return Reader
     * @throws IOException
     */

    Reader openHtmlFile(String file) throws IOException {
        FileReader in;
        int c;
        in = new FileReader(file);
        return (in);
    }

    /**
     * Returns the Reader connected to the source of a given url
     * @param givenUrl String
     * @return Reader
     * @throws IOException
     */

    Reader openHtmlUrl(String givenUrl) throws IOException {
        URL url = new URL(givenUrl);
        URLConnection urlCon = url.openConnection();
        BufferedReader in = new BufferedReader(new InputStreamReader(urlCon.getInputStream()));
        return in;
    }

    /**
     * returns the int value of a sequence of two hexvalues.
     * @param hex String
     * @return int
     */

    int doubleHexToInt(String hex) {
        return Integer.parseInt(hex, 16);
    }

    /**
     * Returns a Color corresponding to the given color in Hexformat
     * @param hex String
     * @return Color
     */

    int hexToColor(String hex) {
		hex = hex.replaceAll("#", "");
		int x, y, z;
		try {
			x = doubleHexToInt(hex.substring(0, 2));
		} catch (Exception e) {
			hex = ColorConts.colornameTohexvalue(hex);
			x = doubleHexToInt(hex.substring(0, 2));
		}

		y = doubleHexToInt(hex.substring(2, 4));
		z = doubleHexToInt(hex.substring(4, 6));

		if (x > 255)
			x = 255;
		else if (x < 0)
			x = 0;
		if (y > 255)
			y = 255;
		else if (y < 0)
			y = 0;
		if (z > 255)
			z = 255;
		else if (z < 0)
			z = 0;

		return 0xff000000 | (x << 16) | (y << 8) | z;
	}

    /**
	 * GetPageColor gives you a specified color of an HtmlPage. Use getNumbOfColors to get the number of found colors. 
	 * 
	 * @shortdesc GetPageColor gives you a specified color of an HtmlPage.
	 * @param value int, the number of the color you want to be returned
	 * @return color, the color for the given value
	 * @example HtmlTree_colors
	 * @related HtmlTree
	 * @related childNumbToWeight ( ) 
	 * @related getLinks ( ) 
	 * @related getNumbOfColors ( )
	 */

    public int getPageColor(int value) {
        return ((Integer)colors.get(value)).intValue();
    }

    /**
     * Use this Method to get the number of all different colors of a HTML document.
     * 
     * @return int, the number of found colors
     * @example HtmlTree_colors
     * @related HtmlTree
     * @related childNumbToWeight ( ) 
     * @related getLinks ( ) 
     * @related getPageColor ( ) 
     */

    public int getNumbOfColors() {
        return colors.size();
    }

    /**
     * Parses an url and checks if it is a valid link to another document
     * @param url String
     * @throws IllegalArgumentException
     * @return String
     */

    static String before = "";
    static String after = "";

    Url parseUrl(String url) throws IllegalArgumentException, InvalidUrlException {
        //Check if the link is to an Anchor
        int pos = url.indexOf("#");
        if (pos == 0) {
            throw new InvalidUrlException("Just a link to an anchor: " + url);
        } else if (pos > 0) {
            url = url.substring(0, pos);
        }

        //Check that the link is no javascriptcall
        pos = url.indexOf("javascript:");
        if (pos != -1) {
            throw new InvalidUrlException("Just a link to a javascript: " + url);
        }

        //Check if the link is to a file in Webformat
        String toCheck = url.replaceAll("(\\.mp3)|(\\.pdf)|(\\.zip)|(\\.exe)", "");
        if (toCheck.length() < url.length()) {
            throw new InvalidUrlException("Just a link to a not valid file " + url);
        }

        //Check if the link goes to parent folders
        int startLook = 0;
        String urlPart = parentUrlPart;
        while (true) {
            if (url.indexOf("..") == -1) {
                break;
            }
            startLook = url.indexOf("..") + 3;
            url = url.substring(startLook, url.length());
            try {
                urlPart = urlPart.substring(0, urlPart.lastIndexOf("/"));
            } catch (Exception e) {

            }
        }

        //Check if the link is absolute or relative
        pos = url.indexOf(":");
        if (pos == -1) {

            return new Url(url, urlPart + "/", getDomain(urlPart + "/"));
        }

        return new Url(url, "", getDomain(url));

    }
}
