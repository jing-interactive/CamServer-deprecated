package prohtml;

import java.util.*;

/**
 * HtmlElement respresent n element of a HTML page
 * having further children Elements. It extends StandAloneElment and uses
 * its methods to manage attributes. HtmlElemt Elemt makes it possible to add
 * further Elements as Children.
 * @example HtmlElement
 * @related HtmlElement
 * @related addAttribute ( )
 * @related addChild ( ) 
 * @related countAllChildren ( )
 * @related getAttribute ( )
 * @related getAttributes ( )
 * @related getChildren ( )
 * @related getDepth ( )
 * @related getSpecificElements ( ) 
 * @related hasAttribute ( )
 * @related hasAttributes ( )
 * @related hasChildren ( )
 * @related kindOfElement
 * @related printElementTree ( )
 * @related toString ( )
 * @related type ( )
 */

public class HtmlElement extends StandAloneElement{

    /**
     * List keeping the children of this Element
     * @invisible
     */

    public List children;

    /**
     * String keeping text of this Element
     */

    String text;

    /**
     * Initializes a new HtmlElement of the given kind with the given attributes and children
     * @param kindOfElement String, the kind of the element
     * @param attributes HashMap, holds the attributes for the Element
     * @param children List, holds the children of the Element
     */

    public HtmlElement (String kindOfElement, HashMap attributes, List children, HtmlElement parent) {
        super(kindOfElement, attributes,parent);
        this.children = children;
    }

    /**
     * Initializes a HtmlElement of the given kind. Without children and attributes.
     * @param kindOfElement String, the kind of the element
     */

    public HtmlElement (String kindOfElement, HtmlElement parent) {
        this(kindOfElement, new HashMap(), new ArrayList(),parent);
    }

    /**
     * Initializes a HtmlElement of the given kind. With the given List of children.
     * @param kindOfElement String, the kind of the element
     * @param children List, holds the children of the Element
     */

    public HtmlElement (String kindOfElement, List children, HtmlElement parent) {
        this(kindOfElement, new HashMap(), children,parent);
    }

    /**
     * Initializes a new HtmlElement of the given kind with the given attributes.
     * @param kindOfElement String, the kind of the element
     * @param attributes HashMap, holds the attributes for the Element
     */

    public HtmlElement (String kindOfElement, HashMap attributes, HtmlElement parent){
        this(kindOfElement, attributes, new ArrayList(),parent);
    }

    /**
     * Method for getting the children of an element. 
     * For a HtmlElement it gives back a List with its childelements. 
     * @return List, holding the children of this element
     * @shortdesc Method for getting the children of an element.
     * @example HtmlElement_getChildren
     * @related HtmlElement
     * @related addAttribute ( )
     * @related addChild ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getDepth ( )
     * @related getSpecificElements ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public List getChildren () {
        return children;
    }

    /**
     * This method looks if the element has Children. 
     * For a HtmlElement it returns true if it has at least one 
     * otherwise it gives back false. 
     * @return boolean, true if the Element has children, otherwise false.
     * @shortdesc This method looks if the element has Children. 
     * @example HtmlElement_hasChildren
     * @related HtmlElement
     * @related addAttribute ( )
     * @related addChild ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related getSpecificElements ( ) 
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public boolean hasChildren () {
        return has(children);
    }

    /**
     * Use this method to add a child to an element. 
     * @param element Element, to add as child
     * @example HtmlElement
     * @related HtmlElement
     * @related addAttribute ( )
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related getSpecificElements ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public void addChild (Element element) {
        children.add(element);
    }

    /**
     * Adds an Element to the Children of the Element at the given position
     * @param element Element, to add as child
     * @param position int, position to where to insert the element.
     */

    public void addChild (Element element, int position) {
        children.add(position, element);
    }

    /**
     * Method for getting the maximum depth of an Element to one of its descendants. 
     * For a HtmlElement it returns the maximum depth of this element to one of its leaves.
     * @return int, maximum depth of this element to one of its leaves
     * @shortdesc Gives back the maximum depth of an Element to one of its descendants
     * @example HtmlElement_getDepth
     * @related HtmlElement
     * @related addAttribute ( )
     * @related addChild ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related getSpecificElements ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public int getDepth () {
        int result = 0;
        List children = getChildren();
        for (int i = 0; i < children.size(); i++) {
            result = Math.max(result, ((Element)children.get(i)).getDepth());
        }
        return 1 + result;
    }

    /**
     * Method for counting the descendants of the element. 
     * For a HtmlElement it returns the number of all its decendents. 
     * @return int, the number of all children
     * @shortdesc Method for counting the descendants of the element. 
     * @example HtmlElement_countAllChildren
     * @related HtmlElement
     * @related addAttribute ( )
     * @related addChild ( ) 
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related getSpecificElements ( )
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public int countAllChildren () {
        int result = 0;
        List children = getChildren();
        for (int i = 0; i < children.size(); i++) {
            result += ((Element)children.get(i)).countAllChildren();
        }
        return 1 + result;

    }

    /**
     * Use this method to get a list with elements of the given kind being decendents of this Element.
     * @param element String, kind of elemts to search for
     * @return List, the found childelements
     * @example HtmlElement_getSpecificElements
     * @related HtmlElement
     * @related addAttribute ( )
     * @related addChild ( ) 
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

    public List getSpecificElements (String element) {
        element = element.toLowerCase();
        ArrayList result = new ArrayList();
        List children = getChildren();
        for (int i = 0; i < children.size(); i++) {
            LetterElement keep = (LetterElement)children.get(i);
            if (keep.type().equals(Conts.HTML_ELEMENT)) {
                result.addAll(((HtmlElement)keep).getSpecificElements(element));
            }
            if (keep.kindOfElement.equals(element)) {
                result.add(keep);
            }
        }
        return result;
    }

    /**
     * This method returns the type of an element. 
     * For the HtmlElement it returns HtmlElement.
     * @return String, the type of the element
     * @shortdesc This method returns the type of an element. 
     * @example HtmlElement_type
     * @related HtmlElement
     * @related addAttribute ( )
     * @related addChild ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related getSpecificElements ( ) 
     * @related hasAttribute ( )
     * @related hasAttributes ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */

    public String type () {
        return Conts.HTML_ELEMENT;
    }

    /**
     * This method prints the treestrucure of this element and its decendents by using the given dist String.
     * @param dist String, to format the output
     * @example HtmlElement
     * @related HtmlElement
     * @related addAttribute ( )
     * @related addChild ( ) 
     * @related countAllChildren ( )
     * @related getAttribute ( )
     * @related getAttributes ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related getSpecificElements ( ) 
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
     * Prints the tree of this Element with the given distance and start string.
     * @param start String
     * @param dist String
     * @invisible
     */

    public void printElementTree (String start, String dist) {
        super.printElementTree(start, dist);
        for (int i = 0; i < children.size(); i++) {
            ((Element)children.get(i)).printElementTree(start + dist, dist);
        }
        System.out.println(start + "</" + kindOfElement + ">");
    }
}
