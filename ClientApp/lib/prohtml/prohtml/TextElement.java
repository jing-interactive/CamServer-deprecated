package prohtml;

import java.util.*;

/**
 * Object for keeping a single word of a text of the HTML document.
 * @example TextElement
 * @related LetterElement
 * @related countAllChildren ( )
 * @related getChildren ( )
 * @related getDepth ( )
 * @related hasChildren ( )
 * @related kindOfElement 
 * @related printElementTree ( )
 * @related toString ( )
 * @related type ( ) 
 */

public class TextElement extends LetterElement {

    /**
     * Holds the parent of this Element
     */
	HtmlElement parent;

	/**
	 * Initialises a new TextElement from a single word and a parent Element.
	 * @param kindOfElement String, the word for this textelement
	 * @param parent HtmlElement, the parent Element
	 */
    public TextElement (String kindOfElement, HtmlElement parent) {
        super(kindOfElement);
        this.parent = parent;
    }

    /**
     * Gives back the parent of this Element.
     * @return HtmlElement
     * @related TextElement
     * @related countAllChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related kindOfElement 
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public HtmlElement getParent (){
        return parent;
    }

    /**
     * Method for getting the children of an element. 
     * For a TextElement it gives back a List with the single Letters of the word.
     * @return List, the letters of the word
     * @shortdesc Method for getting the children of an element. 
     * @example TextElement
     * @related TextElement
     * @related countAllChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related kindOfElement 
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public List getChildren () {
        List result = new ArrayList();
        char[] string = kindOfElement.toCharArray();
        for (int i = 0; i < string.length; i++) {
            result.add(new LetterElement(new String(string, i, 1)));
        }
        return result;
    };

    /**
     * This method looks if a element has children. 
     * For a TextElement it returns always true because a word has the letters as children.
     * @return boolean, true because a word has the letters as children.
     * @shortdesc This method looks if a element has children. 
     * @example TextElement
     * @related TextElement
     * @related countAllChildren ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related kindOfElement 
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public boolean hasChildren () {
        return true;
    }

    /**
     * Method for getting the maximum depth of an element to one of its descendants. 
     * For a LetterElement it returns 2 because a word has letters as leaves.
     * @return int, 2 because a word has letters as leaves
     * @shortdesc Method for getting the maximum depth of an element to one of its descendants. 
     * @example TextElement
     * @related TextElement
     * @related countAllChildren ( )
     * @related getChildren ( )
     * @related hasChildren ( )
     * @related kindOfElement 
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public int getDepth () {
        return 2;
    }

    /**
     * Method for counting the descendants of a element. 
     * For a TextElement it returns the length of the word, 
     * because the children of a TextElement are the letters 
     * @return int, the length of the word
     * @shortdesc Method for counting the descendants of a element.
     * @example TextElement
     * @related TextElement
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related kindOfElement 
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public int countAllChildren () {
        return kindOfElement.length();
    }

    /**
     * This method returns the type of an element: For a TextElement it returns TextElement.
     * @return String, type of this element (TextElement)
     * @example TextElement
     * @related TextElement
     * @related countAllChildren ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related printElementTree ( )
     * @related toString ( )
     */
    public String type () {
        return Conts.TEXT_ELEMENT;
    }

    /**
     * This method returns the String representing an element. For a TextElement it returns a single word.
     * @return String, representing an element
     * @shortdesc This method returns the String representing an element.
     * @example TextElement
     * @related TextElement
     * @related countAllChildren ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related kindOfElement 
     * @related printElementTree ( )
     * @related type ( ) 
     */
    public String toString () {
        return kindOfElement;
    }

    /**
     * This method prints the treestrucure of an element and its decendents 
     * by using the given dist String. For a TextElement it simply prints the word. 
     * @param dist String, to format the output
     * @shortdesc This method prints the treestrucure of an element and its decendents.
     * @example TextElement
     * @related TextElement
     * @related countAllChildren ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related kindOfElement 
     * @related toString ( )
     * @related type ( ) 
     */

    public void printElementTree (String dist) {
        System.out.print(this);
    }

    /**
     * Prints the treestrucure of this element and its decendents
     * by using the given dist String and start String. In this case it simply prints the word.
     * @param start String
     * @param dist String
     * @invisible
     */

    public void printElementTree (String start, String dist) {
        System.out.println(start + dist + this);
    }

}
