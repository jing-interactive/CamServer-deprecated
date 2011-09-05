package prohtml;

/**
 * This class is for access to different constants. 
 * You can use these constants to check which type of element 
 * you have.
 * @example Conts_fields
 * @related LETTER_ELEMENT
 * @related TEXT_ELEMENT
 * @related STAND_ALONE_ELEMENT
 * @related HTML_ELEMENT 
 */
public class Conts {
    /**
     * LETTER_ELEMENT is simply a static field containing a string. 
     * The sense of LETTER_ELEMENT is to check if the type of an element 
     * of the pageTree is LETTER_ELEMENT. There are different other types 
     * of elements: TEXT_ELEMENT, STAND_ALONE_ELEMENT and HTML_ELEMENT. 
     * Use these constants to make shure you take the correct methods of an element. 
     * 
     * @shortdesc Represents the type LetterElement as String.
     * @example Conts_fields
	 * @related STAND_ALONE_ELEMENT
	 * @related HTML_ELEMENT
	 * @related TextElement
	 * @related Conts
     */
    public static final String LETTER_ELEMENT = "LetterElement";

    /**
	 * TEXT_ELEMENT is simply a static field containing a string. The sense of
	 * TEXT_ELEMENT is to check if the type of an element of the pageTree is
	 * TEXT_ELEMENT. There are different other types of elements:
	 * LETTER_ELEMENT, STAND_ALONE_ELEMENT and HTML_ELEMENT. Use these constants
	 * to make shure you take the correct methods of an element.
	 * 
	 * @shortdesc Represents the type TextElement as String.
	 * @example Conts_fields
	 * @related STAND_ALONE_ELEMENT
	 * @related HTML_ELEMENT
     * @related LETTER_ELEMENT
	 * @related Conts
	 */
    public static final String TEXT_ELEMENT = "TextElement";

    /**
     * STAND_ALONE_ELEMENT is simply a static field containing a string. 
     * The sense of STAND_ALONE_ELEMENT is to check if the type of an element 
     * of the pageTree is STAND_ALONE_ELEMENT. There are different other types 
     * of elements: TEXT_ELEMENT, LETTER_ELEMENT and HTML_ELEMENT. 
     * Use these constants to make shure you take the correct methods of an element. 
     * 
     * @shortdesc Represents the type StandAloneElement as String.
     * @example Conts_fields
     * @related LETTER_ELEMENT
	 * @related TextElement
	 * @related HTML_ELEMENT
	 * @related Conts
     */
    public static final String STAND_ALONE_ELEMENT = "StandAloneElement";

    /**
     * HTML_ELEMENT is simply a static field containing a string. 
     * The sense of HTML_ELEMENT is to check if the type of an element 
     * of the pageTree is HTML_ELEMENT. 
     * There are different other types of elements: TEXT_ELEMENT, LETTER_ELEMENT 
     * and STAND_ALONE_ELEMENT. Use these constants to make shure you take 
     * the correct methods of an element. 
     * 
     * @shortdesc Represents the type HtmlElement as String.
     * @example Conts_fields
     * @related LETTER_ELEMENT
	 * @related STAND_ALONE_ELEMENT
	 * @related TextElement
	 * @related Conts
     */
    public static final String HTML_ELEMENT = "HtmlElement";

}
