package prohtml;

/**
 * <p>Title: InvalidColorNameException</p>
 *
 * <p>Description: Thrown when an invalid colorname was parsed</p>
 *
 * <p>Copyright: Copyright (c) 2004</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 * @invisible
 */
public class InvalidColorNameException extends IllegalArgumentException{
	static final long serialVersionUID = 0;
    public InvalidColorNameException () {
        super("This is not a valid colorname");
    }

    public InvalidColorNameException (String colorname) {
        super(colorname+" is not a parsable URL");
    }

}
