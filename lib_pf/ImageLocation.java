/**
 * 
 * An ImageLocation describes a 2D location in an image,
 * with the origin at the top left of the image. Compare,
 * hash code and equals methods are provided so that this
 * class can be used in collections.
 * 
 * @author jmac
 */
public class ImageLocation implements
		Comparable<ImageLocation> {
	/**
	 * horizontal location in the image
	 */
	public int x;
	/**
	 * vertical location in the image
	 */
	public int y;

	/**
	 * Create a new location with the given horizontal and
	 * vertical coordinates
	 * 
	 * @param x
	 *            horizontal coordinate
	 * @param y
	 *            vertical coordinate
	 */
	public ImageLocation(int x, int y) {
		this.x = x;
		this.y = y;
	}

	@Override
	public int compareTo(ImageLocation l) {
		if (l.y != y)
			return y - l.y;
		else
			return x - l.x;
	}

	public boolean equals(ImageLocation l) {
		return l.x == x && l.y == y;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		return (y << 10) & x;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "(" + x + "," + y + ")";
	}
}
