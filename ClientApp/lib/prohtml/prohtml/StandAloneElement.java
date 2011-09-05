package prohtml;

import java.util.*;

/**
 * Object for keeping a standaloneelement of the HTML document.
 * @example StandAloneElement
 * @related HtmlElement
 * @related addAttribute ( ) 
 * @related countAllChildren ( )
 * @related getAttribute ( )
 * @related getAttributes ( )
 * @related getChildren ( )
 * @related getDepth ( )
 * @related hasAttribute ( )
 * @related hasAttributes ( )
 * @related hasChildren ( )
 * @related kindOfElement
 * @related printElementTree ( )
 * @related toString ( )
 * @related type ( ) 
 */

public class StandAloneElement extends LetterElement {
    /**
     * Holds the values and keys of the elements attributes.
     */
    HashMap attributes;

    /**
     * Holds the parent of this Element
     */

    HtmlElement parent;

   /**
    * Initializes a new Instance of StandaloneElement with the given kind, attributes and parent element
    * @param kindOfElement String, the kind of the element
    * @param attributes HashMap, holds the attributes as key value pairs
    * @param parent HtmlElement, the parent element 
    */
    public StandAloneElement (String kindOfElement, HashMap attributes, HtmlElement parent) {
        super(kindOfElement.toLowerCase());
        this.attributes = attributes;
        this.parent = parent;
    }

    /**
     * Initializes a StandaloneElement with a given kind and parent element
     * @param kindOfElement String
     */

    public StandAloneElement (String kindOfElement, HtmlElement parent) {
        this(kindOfElement.toLowerCase(), new HashMap(), parent);
    }

    /**
     * Checks if a List has Content
     * @param toCheck List
     * @return boolean
     */

    boolean has (List toCheck) {
        if (toCheck.isEmpty() || toCheck == null) {
            return false;
        } else {
            return true;
        }
    }

    /**
     * Use this method to get the keys of all available attributes of an element.
     * @return String[], Array with all attribute keys
     * @example StandAloneElement_getAtttributes
     * @related StandAloneElement
     * @related addAttribute ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public String[] getAttributes () {
        return (String[])attributes.keySet().toArray();
    }

    /**
     * Use this method to get the value of a given attribute of an element. 
     * @param key String, name of the attribute you want the value for
     * @return String, the value of the attribute
     * @example StandAloneElement
     * @related StandAloneElement
     * @related addAttribute ( ) 
     * @related countAllChildren ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public String getAttribute (String key) {
        return (String)attributes.get(key.toLowerCase());
    }

    /**
     * Use this method to see if an element has attributes. 
     * @return boolean, true if this element has attributes
     * @example StandAloneElement_hasAttributes
     * @related StandAloneElement
     * @related addAttribute ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasAttribute ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public boolean hasAttributes () {
        return!attributes.isEmpty();
    }

	/**
	 * Use this method to see if an element has a given attribute. 
	 * @param key, attribute you want to check
	 * @return boolean, true if the element has the attribute
	 * @example StandAloneElement_hasAttribute
	 * @related StandAloneElement
	 * @related addAttribute ( ) 
	 * @related countAllChildren ( )
	 * @related getAttribute ( )
	 * @related getAttributes ( )
	 * @related getChildren ( )
	 * @related getDepth ( )
	 * @related hasAttributes ( )
	 * @related hasChildren ( )
	 * @related kindOfElement
	 * @related printElementTree ( )
	 * @related toString ( )
	 * @related type ( ) 
	 */
    public boolean hasAttribute (String key) {
        return attributes.containsKey(key.toLowerCase());
    }

	/**
	 * Use this method to add an attribute to the element. 
	 * @param key String, the key of the attribute
	 * @param value String, the value of the attribute
	 * @example StandAloneElement
	 * @related StandAloneElement
	 * @related countAllChildren ( )
	 * @related getAttribute ( )
	 * @related getAttributes ( )
	 * @related getChildren ( )
	 * @related getDepth ( )
	 * @related hasAttribute ( )
	 * @related hasAttributes ( )
	 * @related hasChildren ( )
	 * @related kindOfElement
	 * @related printElementTree ( )
	 * @related toString ( )
	 * @related type ( ) 
	 */
    public void addAttribute (String key, String value) {
        attributes.put(key, value);
    }

    /**
     * Gives back the parent of this Element.
     * @return HtmlElement, the parentElement
     * @related StandAloneElement
	 * @related countAllChildren ( )
	 * @related getAttribute ( )
	 * @related getAttributes ( )
	 * @related getChildren ( )
	 * @related getDepth ( )
	 * @related hasAttribute ( )
	 * @related hasAttributes ( )
	 * @related hasChildren ( )
	 * @related kindOfElement
	 * @related printElementTree ( )
	 * @related toString ( )
	 * @related type ( ) 
     */
    public HtmlElement getParent () {
        return parent;
    }

    /**
     * This method returns the type of an element: For the StandAloneElement it returns StandAloneElement.
     * @return String, the type of the element
     * @example StandAloneElement_type
     * @related StandAloneElement
     * @related addAttribute ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public String type () {
        return Conts.STAND_ALONE_ELEMENT;
    }

    /**
     * This method returns the String representing an element. 
     * For a StandAloneElement it returns the String representation of the element and its attributes.
     * @return String, String representation of the element
     * @shortdesc This method returns the String representing an element.
     * @example StandAloneElement_toString
     * @related StandAloneElement
     * @related addAttribute ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related type ( ) 
     */
    public String toString () {
        String result = "<" + kindOfElement;
        for (Iterator it = attributes.keySet().iterator(); it.hasNext(); ) {
            String key = (String)it.next();
            result += " " + key + "=\"" + attributes.get(key) + "\"";
        }
        result += ">";
        return result;
    }

    /**
     * This method prints the treestrucure of an element and its decendents by using the given dist String. 
     * For a StandAloneElement it simply prints the element and its attributes. 
     * @param dist String, to format the output
     * @example StandAloneElement_printElementTree
     * @shortdesc This method prints the treestrucure of an element.
     * @related StandAloneElement
     * @related addAttribute ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related toString ( )
     * @related type ( ) 
     */

    public void printElementTree (String dist) {
        printElementTree("", dist);
    }

    /**
     * Prints this Element
     * @param dist String
     * @invisible
     */

    public void printElementTree (String start, String dist) {
        System.out.println(start + this);
    }
}
