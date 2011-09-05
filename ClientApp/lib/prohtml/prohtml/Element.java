package prohtml;

import java.util.*;

/**
 * Basic Description of the Methods that all Kinds of Elements have to supply. Every 
 * HtmlElement, StandAloneElement, TextElement or LetterElement is also an Element.
 * @invisible
 */

public interface Element {

    /**
     * Gives back the Children of the Element.
     * @return List, with the children Elements
     */

    public List getChildren ();

    /**
     * Returns true if this Element has Children
     * @return boolean
     */

    public boolean hasChildren ();

    /**
     * Returns the maximum depth of the Element
     * @return int
     */

    public int getDepth ();

    /**
     * Counts the Number of all decendents of this Element
     * @return int
     */

    public int countAllChildren ();

    /**
     * Returns the type of this Element.
     * @return String
     */

    public String type ();

    /**
     * Returns the content of the Element as String
     * @return String
     */

    public String toString ();

    /**
     * Prints the treestrucure of this element and its decendents by using the given dist String
     * @param dist String
     */

    public void printElementTree (String dist);

    /**
     * Prints the treestrucure of this element and its decendents by using the given dist String and start String
     * @param start String
     * @param dist String
     */

    public void printElementTree (String start, String dist);

}
