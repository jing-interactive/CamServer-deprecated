package prohtml;

import java.util.*;

/**
 * Object for keeping the letter of a text element of the HTML document.
 * @example LetterElement
 * @related TextElement
 * @related countAllChildren ( )
 * @related getChildren ( )
 * @related getDepth ( )
 * @related hasChildren ( )
 * @related kindOfElement
 * @related printElementTree ( )
 * @related toString ( )
 * @related type ( ) 
 */

public class LetterElement implements Element {
    /**
     * String holding the kind of the Element. For a LetterElement it keeps a single letter.
     * @example LetterElement_kindOfElement
     * @related LetterElement
     * @related countAllChildren ( ) 
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public String kindOfElement;

    /**
     * Initializes a new LetterElement from a single Letter
     * @param kindOfElement String, the letter for the element
     */

    public LetterElement (String kindOfElement) {
        this.kindOfElement = kindOfElement;
    }

    /**
     * Method for getting the children of an element. 
     * For a LetterElement it gives back an empty ArrayList 
     * because a letter can have no Children. 
     * @return List, empty list
     * @shortdesc Method for getting the children of an element. 
     * @example LetterElement
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
    public List getChildren () {
        return new ArrayList();
    };

    /**
     * This method looks if the element has Children For a LetterElement 
     * it Returns always false because a letter can have no Children. 
     * @return boolean, false because a letter can have no Children
     * @shortdesc This method looks if the element has Children
     * @example LetterElement
     * @related LetterElement
     * @related countAllChildren ( ) 
     * @related getChildren ( )
     * @related getDepth ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public boolean hasChildren () {
        return false;
    }

    /**
     * Method for getting the maximum depth of an Element to one of its descendants. 
     * For a LetterElement it returns 1 because a letter is a leave.
     * @return int, 1 because a letter is a leave
     * @shortdesc Method for getting the maximum depth of an Element
     * @example LetterElement
     * @related LetterElement
     * @related countAllChildren ( ) 
     * @related getChildren ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public int getDepth () {
        return 1;
    }

    /**
     * Method for counting the descendants of the element. 
     * For a LetterElement it returns 0 because a letter is a leave and can have no descendants. 
     * @return int, 0 because a letter is a leave and can have no descendants
     * @shortdesc Method for counting the descendants of the element. 
     * @example LetterElement
     * @related LetterElement
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public int countAllChildren () {
        return 0;
    }

    /**
     * This method returns the type of an element: For a LetterElement it returns LetterElement.
     * @return String, LetterElement
     * @shortdesc Method for counting the descendants of the element. 
     * @example LetterElement
     * @related LetterElement
     * @related countAllChildren ( ) 
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related kindOfElement
     * @related printElementTree ( )
     * @related toString ( )
     */

    public String type () {
        return Conts.LETTER_ELEMENT;
    }

    /**
     * This method returns the String representing an element. 
     * For a LetterElement it returns a single Letter.
     * @return String, a single Letter
     * @shortdesc Method for counting the descendants of the element. 
     * @example LetterElement
     * @related LetterElement
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
     * This method prints the treestrucure of this element and its decendents by 
     * using the given dist String. For the LetterElement it simply prints the letter. 
     * @param dist String, to format the output
     * @shortdesc This method prints the treestrucure of this element and its decendents. 
     * @example LetterElement
     * @related LetterElement
     * @related countAllChildren ( )
     * @related getChildren ( )
     * @related getDepth ( )
     * @related hasChildren ( )
     * @related printElementTree ( )
     * @related toString ( )
     * @related type ( ) 
     */
    public void printElementTree (String dist) {
        System.out.print(this);
    }

    /**
     * Prints the treestrucure of this element and its decendents by using the given dist String and start String
     * @param start String
     * @param dist String
     * @invisible
     */

    public void printElementTree (String start, String dist) {
        System.out.print(kindOfElement);
    }

}
