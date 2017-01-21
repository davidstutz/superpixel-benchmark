/**
 * A SeedLocation2D models a potential seed location for a
 * strongest vertical path. Because we only consider
 * potential seed locations on scanlines that are multiples
 * of some fixed "coarse" parameter G, all vertical
 * coordinates in this class are measured as multiples of G.
 * We call these "coarse coordinates".
 * 
 * @author jmac
 */
public class SeedLocation2D implements
		Comparable<SeedLocation2D> {

	/**
	 * horizontal coordinate of the seed location
	 */
	public int i;
	/**
	 * vertical *coarse* coordinate of the seed location
	 */
	public int j;
	/**
	 * the path strength of the seed location
	 */
	public int val;

	/**
	 * @param i
	 *            horizontal coordinate of the seed location
	 * @param j
	 *            vertical *coarse* coordinate of the seed
	 *            location
	 * @param val
	 *            the path strength of the seed location
	 */
	public SeedLocation2D(int i, int j, int val) {
		this.i = i;
		this.j = j;
		this.val = val;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Comparable#compareTo(java.lang.Object)
	 */
	@Override
	public int compareTo(SeedLocation2D m) {
		if (this.val != m.val) {
			// weaker locations compare higher, so that the
			// strongest locations will be at the head of a
			// sorted list
			return -this.val + m.val;
		} else {
			// break ties arbitrarily -- need to do this for
			// correct insertion into a sorted set
			if (this.j != m.j) {
				return this.j - m.j;
			} else {
				return this.i - m.i;
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "(" + i + "," + j + "," + val + ")";
	}

	/**
	 * return true if the fields of the calling object are
	 * numerically equal to the fields of the given
	 * SeedLocation2D
	 */
	public boolean equals(SeedLocation2D l) {
		return i == l.i && j == l.j && val == l.val;
	}

}
