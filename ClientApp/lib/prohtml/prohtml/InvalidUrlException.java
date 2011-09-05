package prohtml;

/**
 * <p>Title: InvalidUrlException</p>
 *
 * <p>Description: Thrown when the document behind an url, could not be parsed</p>
 *
 * <p>Copyright: Copyright (c) 2004</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 * @invisible
 */

public class InvalidUrlException extends IllegalArgumentException{
	static final long serialVersionUID = 0;
	public InvalidUrlException(Exception e){
		super("This is not a parsable URL", e);
	}

	public InvalidUrlException(String url, Exception e){
		super(url + " is not a parsable URL", e);
	}

	public InvalidUrlException(){
		super("This is not a parsable URL");
	}

	public InvalidUrlException(String url){
		super(url + " is not a parsable URL");
	}

}
