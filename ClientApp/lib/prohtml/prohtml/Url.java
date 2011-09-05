package prohtml;

/**
 * This class is to keep an url. It stores if an url is absolute or relative.
 * It also stores the parentpart of an url, so it is possible to get the complete Part.
 *
 * @example Url
 * @related compareTo ( )
 * @related equals ( ) 
 * @related getCompleteUrl ( )
 * @related getDomain ( )
 * @related getParentPart ( )
 * @related getUrl ( )
 * @related isAbsolute ( )
 * @related toString ( )
 */

public class Url implements Comparable {
    String url;
    String parentPart;
    String domain;

    boolean absoluteUrl;

    /**
     * Initializes a new Url Object from a relative url part and a parent url part.
     * @param url String, part that is given from a link in a html page 
     * @param parentPart String, part that is added by proHTML for the case the url is relative
     */

    public Url(String url, String parentPart) {
        this.url = url;
        this.parentPart = parentPart;
        this.absoluteUrl = parentPart.equals("");
        this.domain = parentPart;
    }

    /**
     * Initializes a new Url Object from a relative url part, a parent url part and a domain.
     * @param url String, part that is given from a link in a html page 
     * @param parentPart String, part that is added by proHTML for the case the url is relative
     * @param domain String, the domain part of an url
     */
    public Url(String url, String parentPart,String domain){
        this(url,parentPart);
        this.domain = domain;
    }

    /**
     * Gives back true if the url is absolute, false if it is realtive
     * @example Url
     * @return boolean, true if the url is absolute
     * @related Url
     * @related compareTo ( ) 
     * @related equals ( )
     * @related getCompleteUrl ( )
     * @related getDomain ( )
     * @related getParentPart ( )
     * @related getUrl ( )
     * @related toString ( ) 
     */

    public boolean isAbsolute() {
        return absoluteUrl;
    }

    /**
     * Gives back the url stored in this object. If the domain is absolute you get the complete url, if it is relative
     * you get the relative part
     * @example Url
     * @return String, url as it was given as link in the html page
     * @related Url
     * @related compareTo ( ) 
     * @related equals ( )
     * @related getCompleteUrl ( )
     * @related getDomain ( )
     * @related getParentPart ( )
     * @related isAbsolute ( )
     * @related toString ( ) 
     */

    public String getUrl() {
        return url;
    }

    /**
     * Gives back the domain of the site.
     * @example Url
     * @return String, the domainpart of the url
     * @related Url
     * @related compareTo ( ) 
     * @related equals ( )
     * @related getCompleteUrl ( )
     * @related getParentPart ( )
     * @related getUrl ( )
     * @related isAbsolute ( )
     * @related toString ( ) 
     */
    public String getDomain(){
        return domain;
    }

    /**
     * Gives back the parent url part. If the url is absolute it just
     * returns an empty String ("").
     * @example Url
     * @return String, parent part of a relative url
     * @related Url
     * @related compareTo ( ) 
     * @related equals ( )
     * @related getCompleteUrl ( )
     * @related getDomain ( )
     * @related getUrl ( )
     * @related isAbsolute ( )
     * @related toString ( ) 
     */

    public String getParentPart() {
        return parentPart;
    }

    /**
     * Gives back the complete url, no matter if it is absolute or relative.
     * @example Url
     * @return String, the complete url
     * @related Url
     * @related compareTo ( ) 
     * @related equals ( )
     * @related getDomain ( )
     * @related getParentPart ( )
     * @related getUrl ( )
     * @related isAbsolute ( )
     * @related toString ( ) 
     */

    public String getCompleteUrl() {
        return parentPart + url;
    }

    /**
     * Overwrites the equals function to make sure when two Urls are equal.
     * @param toCompare Url to compare
     * @return boolean, true if the given Url is the same
     * @related Url
     * @related compareTo ( ) 
     * @related getCompleteUrl ( )
     * @related getDomain ( )
     * @related getParentPart ( )
     * @related getUrl ( )
     * @related isAbsolute ( )
     * @related toString ( ) 
     */

    public boolean equals(Url toCompare) {
        if (this.url.equals(toCompare.url) && this.parentPart.equals(toCompare.parentPart)) {
            return true;
        }
        return false;
    }

    /**
     * Compares two url. Returns 0 if the two urls are equal.
     * @param toCompare Object, url to compare
     * @return int
     * @related Url
     * @related equals ( ) 
     * @related getCompleteUrl ( )
     * @related getDomain ( )
     * @related getParentPart ( )
     * @related getUrl ( )
     * @related isAbsolute ( )
     * @related toString ( ) 
     * @throws ClassCastException
     */

    public int compareTo(Object toCompare) throws ClassCastException {
        if (!(toCompare instanceof Url)) {
            throw new ClassCastException();
        }

        Url urlToCompare = (Url)toCompare;

        if (urlToCompare.equals(this)) {
            return 0;
        }

        if (this.parentPart.equals(urlToCompare.parentPart)) {
            return this.url.compareTo(urlToCompare.url);
        } else {
            return this.parentPart.compareTo(urlToCompare.parentPart);
        }
    }

    /**
     * Gives back the seperated parentPart and url part. Use getCompleteUrl to get the real url.
     * @example Url
     * @return String
     * @related Url
     * @related compareTo ( ) 
     * @related equals ( )
     * @related getCompleteUrl ( )
     * @related getDomain ( )
     * @related getParentPart ( )
     * @related getUrl ( )
     * @related isAbsolute ( ) 
     */

    public String toString() {
        return ("parentPart: " + parentPart + " url: " + url);
    }

}
