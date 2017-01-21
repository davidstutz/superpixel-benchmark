import java.awt.BorderLayout;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.util.SortedSet;
import java.util.TreeSet;
import java.util.Vector;

import javax.swing.JFrame;

/**
 * A VerticalPathFinder finds vertical paths in an image,
 * using the strongest vertical paths dynamic programming
 * technique.
 * 
 * @author jmac
 */
public class VerticalPathFinder {

	// a filter for detecting vertical edge strengths in the
	// image
	private HorizontalEdgeFilter horizontalEdgeFilter;

	// the image in which strongest vertical paths will be
	// found
	private BufferedImage image;

	// width and height of the image
	private int width;
	private int height;

	// the distance between scanlines, denoted G in the
	// paper, at which eligible seed locations are
	// considered
	private int coarseGridSize;

	// the number of scanlines eligible for seed locations
	private int coarseHeight;

	// What is the minimum horizontal distance from the seed
	// of a new vertical path to an existing one?
	private int gap;

	// when backtracking, should we stop as soon as we run
	// into an existing boundary?
	private boolean stopBacktrackEarly;

	// edgeStrengths[x][y] is the strength of the edge at
	// location (x, y)
	private int[][] edgeStrengths;

	// the "path strength" of a path is the sum of the edge
	// strengths along the locations in the path

	// an "upwards path from (x,y)" begins at (x,y) and
	// includes exactly 1 pixel in each row above row y.
	// x-values of pixels in the path differ by at most 1
	// between adjacent rows. The upwards path from (x,y)
	// includes (x,y).

	// a "downwards path from (x,y)" begins from (x,y) and
	// includes exactly 1 pixel in each row below row y,
	// again with x-values of pixels differing by at most
	// 1 between adjacent rows. To avoid double-counting,
	// the downwards path from (x,y) *does not* include
	// (x,y).

	// pathStrengthsUp[x][y] is the path strength of the
	// best upwards path from (x,y)
	private int[][] pathStrengthsUp;

	// pathStrengthsDown[x][y] is the path strength of the
	// best downwards path from (x,y)
	private int[][] pathStrengthsDown;

	// if pathIndicesUp[x][y]=v then the strongest upwards
	// path beginning at location (x,y) passes through
	// location (v,y-1)
	private int[][] pathIndicesUp;

	// if pathIndicesDown[x][y]=v then the strongest
	// downwards
	// path beginning at location (x,y) passes through
	// location (v,y+1)
	private int[][] pathIndicesDown;

	// pathStrengths[x][y] is the path strength of the
	// best vertical path through (x,y)
	private int[][] pathStrengths;

	// pathMap[x][y] is true if a strong vertical path
	// passes through location (x,y), and false otherwise
	private boolean[][] pathMap;

	// isRemainingMaximum[x][j] is true if:
	// (1) pathStrengths[x][j*coarseGridSize] is a local
	// maximum with respect to x, and
	// (2) no strongest vertical path has yet been created
	// whose horizontal coordinate in row j*coarseGridSize
	// lies in the interval [x-gap,x+gap),
	// where gap is the field of that name
	private boolean[][] isRemainingMaximum;

	/**
	 * @param bi
	 *            The image in which vertical paths will be
	 *            found
	 * @param radius
	 *            The radius of the horizontal edge filter
	 *            used to compute edge strengths
	 * @param coarseGridSize
	 *            The distance between scanlines at which
	 *            eligible seed locations are considered
	 * @param gap
	 *            The minimum horizontal distance from the
	 *            seed of a new vertical path to an existing
	 *            one
	 * @param stopBacktrackEarly
	 *            When backtracking, should we stop as soon
	 *            as we run into an existing boundary?
	 */
	public VerticalPathFinder(BufferedImage bi, int radius,
			int coarseGridSize, int gap,
			boolean stopBacktrackEarly) {
		horizontalEdgeFilter = new HorizontalEdgeFilter(
				radius);
		this.image = bi;
		this.coarseGridSize = coarseGridSize;
		this.gap = gap;
		this.stopBacktrackEarly = stopBacktrackEarly;

		edgeStrengths = horizontalEdgeFilter.filter(bi);
		width = edgeStrengths.length;
		height = edgeStrengths[0].length;
		coarseHeight = (height - 1) / coarseGridSize + 1;
		pathStrengthsUp = new int[width][height];
		pathStrengthsDown = new int[width][height];
		pathStrengths = new int[width][height];
		pathIndicesUp = new int[width][height];
		pathIndicesDown = new int[width][height];
		pathMap = new boolean[width][height];
		// isMaximum = new boolean[width][coarseHeight];
		isRemainingMaximum = new boolean[width][coarseHeight];
	}

	/**
	 * @return the image
	 */
	public BufferedImage getImage() {
		return image;
	}

	/**
	 * @return the pathMap
	 */
	public boolean[][] getPathMap() {
		return pathMap;
	}

	/**
	 * @return the transpose of the current pathMap
	 */
	public boolean[][] getPathMapTranspose() {
		boolean[][] pathMapT = new boolean[height][width];
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				pathMapT[j][i] = pathMap[i][j];
			}
		}
		return pathMapT;
	}

	/**
	 * @return the width of the image being processed
	 */
	public int getWidth() {
		return width;
	}

	/**
	 * @return the height of the image being processed
	 */
	public int getHeight() {
		return height;
	}

	public int getNumMaxima() {
		int count = 0;
		for (int i = 0; i < width; i++)
			for (int j = 0; j < coarseHeight; j++)
				if (isRemainingMaximum[i][j])
					count++;
		return count;
	}

	private void initializeMaps() {
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				pathMap[i][j] = false;
		for (int i = 0; i < width; i++)
			for (int j = 0; j < coarseHeight; j++)
				isRemainingMaximum[i][j] = false;
	}

	/**
	 * Create the set of strongest vertical paths by
	 * choosing seed locations and backtracking from them.
	 * Seed locations are chosen according to the strength
	 * of their paths, subject to certain heuristics.
	 */
	public void makeBestPaths() {
		makeSomeBestPaths(Integer.MAX_VALUE);
	}

	/**
	 * Create a set of strongest vertical paths by choosing
	 * seed locations and backtracking from them. Seed
	 * locations are chosen according to the strength of
	 * their paths, subject to certain heuristics.
	 * 
	 * @param numPaths
	 *            The number of vertical paths to create
	 * @return A vector containing the chosen seed
	 *         locations, in the order in which they were
	 *         chosen
	 */
	public Vector<SeedLocation2D> makeSomeBestPaths(
			int numPaths) {
		// run through all eligible locations (i.e. restrict
		// to scanlines on the coarse grid, and restrict to
		// local maxima wrt horizontal coordinate), and sort
		// all these locations by the strength of the
		// strongest path from that location
		SortedSet<SeedLocation2D> sortedEligibleMaxima = new TreeSet<SeedLocation2D>();
		for (int j = 1; j < coarseHeight; j++) {
			int y = j * coarseGridSize;
			for (int i = 0; i < width; i++) {
				// at this point, isRemainingMaximum [i][j]
				// should be true if and only if the
				// location is a local maximum with respect
				// to the horizontal coordinate
				if (isRemainingMaximum[i][j]) {
					SeedLocation2D m = new SeedLocation2D(
							i, j, pathStrengths[i][y]);
					sortedEligibleMaxima.add(m);
				}
			}
		}

		// Run through the eligible maxima starting with the
		// strongest, and adding the path from each of those
		// locations. Each time a path is added, it might
		// make some of the remaining maxima ineligible, so
		// we check for that.
		Vector<SeedLocation2D> chosenMaxima = new Vector<SeedLocation2D>();
		int counter = 0;
		for (SeedLocation2D m : sortedEligibleMaxima) {
			if (isRemainingMaximum[m.i][m.j]) {
				if (counter >= numPaths)
					break;
				counter++;
				chosenMaxima.add(m);
				// System.out.format("i %d, j %d, val %d\n",
				// m.i, m.j, m.val);
				int y = m.j * coarseGridSize;
				backtrack(m.i, y);
			}
		}
		return chosenMaxima;
	}

	/**
	 * Draw the currently computed path map onto the
	 * specified image
	 * 
	 * @param image
	 *            The image on which the paths will be drawn
	 * @param transpose
	 *            Should we transpose the paths before
	 *            drawing them onto the image?
	 */
	private void drawPathMap(BufferedImage image,
			boolean transpose) {
		final int thickness = 2;
		final int offset = thickness / 2;
		WritableRaster r = image.getRaster();
		// vertical paths will be yellow, horizontal paths
		// will be white
		final int[] vals = { 255, 255, 0 };
		final int[] valsT = { 255, 255, 255 };
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (pathMap[i][j]) {
					for (int k = 0; k < thickness; k++) {
						if (!transpose) {
							int x = Math.max(0, Math.min(
									width - 1, i + k
											- offset));
							r.setPixel(x, j, vals);
						} else {
							int y = Math.max(0, Math.min(
									width - 1, i + k
											- offset));
							r.setPixel(j, y, valsT);
						}
					}
				}
			}
		}
	}

	/**
	 * Draw the currently computed path map onto the
	 * specified image
	 * 
	 * @param image
	 *            The image on which the paths will be drawn
	 */
	public void drawPathMap(BufferedImage image) {
		drawPathMap(image, false);
	}

	/**
	 * Draw the transpose of the currently computed path map
	 * onto the specified image
	 * 
	 * @param image
	 *            The image on which the paths will be drawn
	 */
	public void drawPathMapT(BufferedImage image) {
		drawPathMap(image, true);
	}

	/**
	 * Draw the detected maxima on the given image
	 */
	public void drawMaxima(BufferedImage image) {
		for (int j = 0; j < coarseHeight; j++) {
			int y = j * coarseGridSize;
			for (int i = 1; i < width - 1; i++) {
				if (isRemainingMaximum[i][j]) {
					// int offset = 5;
					// int y1 = Math.max(0, y - offset);
					// int y2 = Math.min(height - 1, y
					// + offset);
					// int x1 = Math.max(0, i - offset);
					// int x2 = Math
					// .min(width - 1, i + offset);
					ImageUtils.makeRedCross(image, i, y,
							10, 2);
					// ImageUtils.makeGreenRectangle(image,
					// i,
					// i + 1, y1, y2);
				}
			}
		}

	}

	/**
	 * Compute the path strengths of vertical paths from
	 * every point in the image, and the local maxima in
	 * these strengths with respect to the horizontal
	 * coordinate
	 */
	public void computePathStrengths() {
		initializeMaps();
		computePathStrengthsUp();
		computePathStrengthsDown();
		computeVerticalPathStrengths();
		computeMaxima();
	}

	/**
	 * Compute upwards path strengths from every pixel,
	 * using a simple dynamic programming approach
	 */
	private void computePathStrengthsUp() {
		int max, argmax, i, j, k, val;
		argmax = -1;
		for (i = 0; i < width; i++) {
			pathStrengthsUp[i][0] = edgeStrengths[i][0];
		}

		for (j = 1; j < height; j++) {
			// process leftmost pixel
			max = -1;
			for (k = 0; k <= 1; k++) {
				val = pathStrengthsUp[k][j - 1];
				if (max < val) {
					max = val;
					argmax = k;
				}
			}
			pathIndicesUp[0][j] = argmax;
			pathStrengthsUp[0][j] = max
					+ edgeStrengths[0][j];

			// process interior pixels
			for (i = 1; i < width - 1; i++) {
				max = -1;
				for (k = i - 1; k <= i + 1; k++) {
					val = pathStrengthsUp[k][j - 1];
					// second clause here prioritizes
					// straight paths
					if (max < val || (max == val && k == i)) {
						max = val;
						argmax = k;
					}
				}
				pathIndicesUp[i][j] = argmax;
				pathStrengthsUp[i][j] = max
						+ edgeStrengths[i][j];
			}

			// process rightmost pixel
			max = -1;
			for (k = width - 2; k <= width - 1; k++) {
				val = pathStrengthsUp[k][j - 1];
				// the "<=" prioritizes straight paths
				if (max <= val) {
					max = val;
					argmax = k;
				}
			}
			pathIndicesUp[width - 1][j] = argmax;
			pathStrengthsUp[width - 1][j] = max
					+ edgeStrengths[width - 1][j];

		}
	}

	/**
	 * Compute downwards path strengths from every pixel,
	 * using a simple dynamic programming approach
	 */
	private void computePathStrengthsDown() {
		int max, argmax, i, j, k, val;
		argmax = -1;
		for (i = 0; i < width; i++) {
			pathStrengthsDown[i][height - 1] = 0;
		}

		for (j = height - 2; j > 0; j--) {
			// process leftmost pixel
			max = -1;
			for (k = 0; k <= 1; k++) {
				val = pathStrengthsDown[k][j + 1];
				if (max < val) {
					max = val;
					argmax = k;
				}
			}
			pathIndicesDown[0][j] = argmax;
			pathStrengthsDown[0][j] = max
					+ edgeStrengths[argmax][j + 1];

			// process interior pixels
			for (i = 1; i < width - 1; i++) {
				max = -1;
				for (k = i - 1; k <= i + 1; k++) {
					val = pathStrengthsDown[k][j + 1];
					// second clause here prioritizes
					// straight paths
					if (max < val || (max == val && k == i)) {
						max = val;
						argmax = k;
					}
				}
				pathIndicesDown[i][j] = argmax;
				pathStrengthsDown[i][j] = max
						+ edgeStrengths[argmax][j + 1];
			}

			// process rightmost pixel
			max = -1;
			for (k = width - 2; k <= width - 1; k++) {
				val = pathStrengthsDown[k][j + 1];
				// the "<=" prioritizes straight paths
				if (max <= val) {
					max = val;
					argmax = k;
				}
			}
			pathIndicesDown[width - 1][j] = argmax;
			pathStrengthsDown[width - 1][j] = max
					+ edgeStrengths[argmax][j + 1];

		}
	}

	/**
	 * Compute vertical path strengths from every pixel by
	 * adding upwards and downwards path strengths
	 */
	private void computeVerticalPathStrengths() {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				pathStrengths[i][j] = pathStrengthsUp[i][j]
						+ pathStrengthsDown[i][j];
			}
		}
	}

	/**
	 * For each point on an eligible scanline, compute
	 * whether or not it is a local maximum with respect to
	 * the horizontal coordinate
	 */
	private void computeMaxima() {
		// the first scanline is not eligible, so start from
		// j=1
		for (int j = 1; j < coarseHeight; j++) {
			int y = j * coarseGridSize;
			// leftmost pixel
			if (pathStrengths[0][y] >= pathStrengths[1][y])
				isRemainingMaximum[0][j] = true;
			// interior pixels
			for (int i = 1; i < width - 1; i++) {
				if ((pathStrengths[i][y] > pathStrengths[i - 1][y])
						&& (pathStrengths[i][y] >= pathStrengths[i + 1][y]))
					isRemainingMaximum[i][j] = true;
			}
			// rightmost pixel
			if (pathStrengths[width - 1][y] > pathStrengths[width - 2][y])
				isRemainingMaximum[width - 1][j] = true;
		}

	}

	/**
	 * Compute and store the strongest vertical path from
	 * the given the seed location, using a simple
	 * backtracking technique.
	 * 
	 * @param i
	 *            Horizontal coordinate of seed location
	 * @param j
	 *            Vertical coordinate of seed location
	 */
	public void backtrack(int i, int j) {
		backtrack(i, i + 1, j);
	}

	/**
	 * Compute and store the strongest vertical path from
	 * the and of the given seed locations, using a simple
	 * backtracking technique. Eligible seed locations lie
	 * between (iMin,j) and (iMax-1,j) inclusive
	 * 
	 * @param iMin
	 *            minimum horizontal coordinate of seed
	 *            location
	 * @param iMax
	 *            maximum horizontal coordinate of seed
	 *            location, plus one
	 * @param j
	 *            Vertical coordinate of seed location
	 */
	public void backtrack(int iMin, int iMax, int j) {
		int max, argmax, i, val;
		argmax = -1;
		max = -1;
		for (i = iMin; i < iMax; i++) {
			val = pathStrengths[i][j];
			if (val > max) {
				max = val;
				argmax = i;
			}
		}
		extendPath(argmax, j);
		backtrackUp(argmax, j);
		backtrackDown(argmax, j);
	}

	/**
	 * Compute and store the strongest upwards vertical path
	 * from the given the seed location, using a simple
	 * backtracking technique.
	 * 
	 * @param i
	 *            Horizontal coordinate of seed location
	 * @param j
	 *            Vertical coordinate of seed location
	 */
	private void backtrackUp(int i, int j) {
		int prevBest = i;
		int nextBest;
		for (; j > 0; j--) {
			nextBest = pathIndicesUp[prevBest][j];
			assert (Math.abs(nextBest - prevBest) <= 1);
			if (!extendPath(nextBest, j - 1)
					&& stopBacktrackEarly)
				break;
			prevBest = nextBest;
		}
	}

	/**
	 * Compute and store the strongest downwards vertical
	 * path from the given the seed location, using a simple
	 * backtracking technique.
	 * 
	 * @param i
	 *            Horizontal coordinate of seed location
	 * @param j
	 *            Vertical coordinate of seed location
	 */
	private void backtrackDown(int i, int j) {
		int prevBest = i;
		int nextBest;
		for (; j < height - 1; j++) {
			nextBest = pathIndicesDown[prevBest][j];
			if (!extendPath(nextBest, j + 1)
					&& stopBacktrackEarly)
				break;
			prevBest = nextBest;
		}
	}

	// return true if the path gets extended, and false if a
	// path is already present at location (x,y)

	/**
	 * Extend the current path map to include the given
	 * location, updating the eligible maxima
	 * 
	 * @param x
	 *            Horizontal location of the extension
	 * @param y
	 *            Vertical location of the extension
	 * @return true if the path gets extended, and false if
	 *         a path is already present at location (x,y)
	 */
	private boolean extendPath(int x, int y) {
		if (pathMap[x][y]) {
			return false;
		} else {
			pathMap[x][y] = true;
			// gridMap[x / coarseGridSize][y /
			// coarseGridSize] = true;
			if (y % coarseGridSize == 0) {
				removeMaxima(x, y);
			}
			return true;
		}
	}

	/**
	 * Remove any eligible maxima within a horizontal gap of
	 * the field "gap" from the given location. Note that
	 * maxima are only ever removed from eligible scanlines,
	 * so this method must be called with
	 * y%coarseGridSize==0.
	 * 
	 * @param x
	 *            Horizontal coordinate of location
	 * @param y
	 *            Vertical coordinate of location
	 */
	private void removeMaxima(int x, int y) {
		// this method is only called when
		// y%coarseGridSize==0,
		// so the following computation of the relevant
		// coarse row makes sense
		int j = y / coarseGridSize;

		int x1 = Math.max(0, x - gap);
		int x2 = Math.min(width, x + gap);
		for (int i = x1; i < x2; i++) {
			isRemainingMaximum[i][j] = false;
		}
	}

	/**
	 * Update the given edge map by adding horizontal and
	 * vertical edges according to the current path map.
	 * 
	 * @param edgeMap
	 *            The EdgeMap to be updated
	 */
	public void updateEdgeMap(RegionFiller.EdgeMap edgeMap) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (pathMap[i][j]) {
					// all locations [i][j] where there is a
					// vertical path cause a vertical edge
					edgeMap.ver[i][j] = true;
					// when the path jumps diagonally left
					// or right, that creates a horizontal
					// edge also
					if (j < height - 1) {
						if (i > 0) {
							// check for horizontal edge to
							// the left
							if (pathMap[i - 1][j + 1]) {
								edgeMap.hor[i - 1][j + 1] = true;
							}
						}
						if (i < width - 1) {
							// check for horizontal edge to
							// the right
							if (pathMap[i + 1][j + 1]) {
								edgeMap.hor[i][j + 1] = true;
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Fill in the given edge map with horizontal and
	 * vertical edges according to the transpose of the
	 * current path map.
	 * 
	 * @param edgeMap
	 *            The EdgeMap to be filled in
	 */
	public void updateEdgeMapT(RegionFiller.EdgeMap edgeMap) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (pathMap[i][j]) {
					// all locations [i][j] where there is a
					// vertical path cause a horizontal edge
					// (when transposed)
					edgeMap.hor[j][i] = true;
					// when the path jumps diagonally left
					// or right, that creates a (transposed)
					// vertical edge also
					if (j < height - 1) {
						if (i > 0) {
							// check for (transposed)
							// vertical edge to the left
							if (pathMap[i - 1][j + 1]) {
								edgeMap.ver[j + 1][i - 1] = true;
							}
						}
						if (i < width - 1) {
							// check for (transposed)
							// vertical edge to the right
							if (pathMap[i + 1][j + 1]) {
								edgeMap.ver[j + 1][i] = true;
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Print out a representation of the current path map,
	 * limited to the top left corner of the path map up to
	 * the given horizontal and vertical coordinates
	 * 
	 * @param x
	 *            horizontal limit
	 * @param y
	 *            vertical limit
	 */
	public void printSomeEdges(int x, int y) {
		System.out.println("edges:");
		for (int j = 0; j < y; j++) {
			for (int i = 0; i < x; i++) {
				if (pathMap[i][j]) {
					System.out.print("1 ");
				} else {
					System.out.print("0 ");
				}
			}
			System.out.println("");
		}
	}

	private static void createAndShowGUI() {
		final int radius = 3;
		final int gap = 30;
		final int coarseGridSize = 2 * gap;
		final boolean stopBacktrackEarly = true;

		final String imageName = "C:\\Documents and Settings\\jmac\\Desktop\\test.jpg";

		BufferedImage original = ImageUtils
				.getImageFromDisk(imageName);
		BufferedImage mutated = ImageUtils
				.getImageFromDisk(imageName);
		VerticalPathFinder pathFinder = new VerticalPathFinder(
				original, radius, coarseGridSize, gap,
				stopBacktrackEarly);

		long start, stop;
		double duration;
		int num_iterations = 100;
		start = System.nanoTime();
		for (int i = 0; i < num_iterations; i++) {
			pathFinder.computePathStrengths();
			pathFinder.makeBestPaths();
		}
		stop = System.nanoTime();
		duration = (stop - start) * 1e-9 / num_iterations;

		System.out
				.format("%f seconds to calculate paths%n",
						duration);

		pathFinder.drawPathMap(mutated);

		// Create and set up the window.
		JFrame frame = new JFrame("PathFinder");
		frame.setLayout(new BorderLayout());
		frame
				.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		ImageComponent image1 = new ImageComponent(original);
		frame.add(image1, BorderLayout.PAGE_START);

		ImageComponent image2 = new ImageComponent(mutated);
		frame.add(image2, BorderLayout.PAGE_END);

		// Display the window.
		frame.pack();
		frame.setVisible(true);
	}

	public static void main(/*String[] args*/) {
		javax.swing.SwingUtilities
				.invokeLater(new Runnable() {
					public void run() {
						createAndShowGUI();
					}
				});

	}

}
