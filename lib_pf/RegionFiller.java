import java.awt.BorderLayout;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.util.SortedSet;
import java.util.TreeSet;
import java.io.PrintWriter;
import java.io.FileNotFoundException;
import java.io.UnsupportedEncodingException;

import javax.swing.JFrame;

/**
 * Given a PathFinder which has computed some horizontal and
 * vertical paths on an image, a RegionFiller can create a
 * new image in which the regions demarcated by the paths
 * are colored uniformly (i.e. pixels from the same region
 * are assigned the same color). A region's color can be
 * chosen randomly, or assigned the average of the region's
 * pixels' colors
 * 
 * @author jmac
 */
public class RegionFiller {

	/**
	 * @author jmac
	 * 
	 * An EdgeMap stores a set of horizontal and vertical
	 * edges in an image
	 */
	public class EdgeMap {

		/**
		 * ver[i][j] is true iff there is a vertical edge
		 * immediately to the left of the pixel at (i,j)
		 */
		public boolean[][] ver;

		/**
		 * hor[i][j] is true iff there is a vertical edge
		 * immediately above the pixel at (i,j)
		 */
		public boolean[][] hor;

		/**
		 * Create a new EdgeMap with the given width and
		 * height, and with no edges in it.
		 */
		public EdgeMap(int width, int height) {
			ver = new boolean[width][height];
			hor = new boolean[width][height];
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					ver[i][j] = false;
					hor[i][j] = false;
				}
			}
		}
	}

	// the width and height of the image and edge map
	private int width, height;

	// map of edges in the image whose regions will be
	// computed
	private EdgeMap edgeMap;

	// region[i][j] = N if location (i,j) is in region N
	// region[i][j] = NO_REGION if location (i,j) has not
	// yet been assigned a region
	private int[][] region;
	private static final int NO_REGION = -1;

	// the number of distinct regions created so far
	private int numRegions;

	public RegionFiller(PathFinder p) {
		this.width = p.getWidth();
		this.height = p.getHeight();
		edgeMap = new EdgeMap(width, height);
		region = new int[width][height];
		p.updateEdgeMap(edgeMap);
	}

	/**
	 * @return the edgeMap
	 */
	public EdgeMap getEdgeMap() {
		return edgeMap;
	}

	/**
	 * @return the numRegions
	 */
	public int getNumRegions() {
		return numRegions;
	}

	/**
	 * Create a new image with a random color assigned to
	 * each region. Regions must have already been computed
	 * before calling this method.
	 * 
	 * @return A new image with a random color assigned to
	 *         each region
	 */
	public BufferedImage makeImageWithRandomRegionColors() {
		int[][] colors = makeRandomRegionColors();
		int[] color;
		BufferedImage image = new BufferedImage(width,
				height, BufferedImage.TYPE_INT_RGB);
		WritableRaster r = image.getRaster();
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int regionID = region[i][j];
				color = colors[regionID];
				r.setPixel(i, j, color);
			}
		}
		return image;
	}

	/**
	 * Create a new image in which the color assigned to
	 * each region is that region's average color in the
	 * original image. Regions must have already been
	 * computed before calling this method.
	 * 
	 * @return A new image with the average color assigned
	 *         to each region
	 */
	public BufferedImage makeImageWithAverageRegionColors(
			BufferedImage original) {
		BufferedImage target = new BufferedImage(width,
				height, BufferedImage.TYPE_INT_RGB);
		int[][] averages = computeRegionAverages(original);
		WritableRaster r = target.getRaster();
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int regionID = region[i][j];
				r.setPixel(i, j, averages[regionID]);
			}
		}
		return target;
	}

	/**
	 * for debugging, create new edge maps restricted to a
	 * small sub window in the top left corner
	 * 
	 * @param x
	 *            width of sub window
	 * @param y
	 *            height of sub window
	 */
	public void restrictToSubWindow(int x, int y) {
		boolean[][] newHor = new boolean[x][y];
		boolean[][] newVer = new boolean[x][y];
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++) {
				newVer[i][j] = edgeMap.ver[i][j];
				newHor[i][j] = edgeMap.hor[i][j];
			}
		}
		width = x;
		height = y;
		edgeMap.ver = newVer;
		edgeMap.hor = newHor;
	}

	/**
	 * print the vertical edge map in human-readable form
	 */
	public void printVerticalEdges() {
		System.out.println("vertical edges:");
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				if (edgeMap.ver[i][j]) {
					System.out.print("1 ");
				} else {
					System.out.print("0 ");
				}
			}
			System.out.println("");
		}
	}

	/**
	 * print the vertical edge map in human-readable form
	 */
	public void printHorizontalEdges() {
		System.out.println("horizontal edges:");
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				if (edgeMap.hor[i][j]) {
					System.out.print("1 ");
				} else {
					System.out.print("0 ");
				}
			}
			System.out.println("");
		}
	}

	/**
	 * print the vertical and horizontal edge maps in
	 * human-readable form
	 */
	public void printEdges() {
		printVerticalEdges();
		printHorizontalEdges();
	}

	/**
	 * Assign each pixel to a region, using a simple
	 * flood-filling approach
	 */
	public void computeRegions() {

		initializeRegions();

		// the "frontier" is the set of not-yet-processed
		// pixels that are adjacent to a
		// previously-processed pixel, initialized to the
		// single location (0,0)
		SortedSet<ImageLocation> frontier = new TreeSet<ImageLocation>();
		frontier.add(new ImageLocation(0, 0));

		// regionID is the ID of the region currently being
		// filled
		int regionID = 0;

		while (!frontier.isEmpty()) {
			// System.out.println("fill: regionID=" +
			// regionID
			// + ", frontier=" + frontier);
			ImageLocation seed = frontier.first();
			frontier.remove(seed);
			fillFrom(seed, regionID, frontier);
			regionID++;
		}
		numRegions = regionID;
	}

        public void saveRegionsToCSV(String fileName) throws FileNotFoundException, UnsupportedEncodingException {
                computeRegions();
                
                PrintWriter csv = new PrintWriter(fileName, "UTF-8");
                for (int i = 0; i < height; i++) {
                        for (int j = 0; j < width; j++) {
                                csv.print(region[j][i]);
                                
                                if (j < width - 1) {
                                        csv.print(",");
                                }
                        }
                        
                        csv.print("\n");
                }
                
                csv.close();
        }
        
	private void initializeRegions() {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				region[i][j] = NO_REGION;
			}
		}
	}

	/**
	 * Flood fill from the given location assigning all
	 * reachable pixels the given region ID. Any pixels
	 * assigned to the region that are also in the given
	 * frontier should be removed from the frontier
	 * 
	 * @param l
	 *            The location from which flood filling
	 *            starts
	 * @param regionID
	 *            The ID of the region we are filling
	 * @param frontier
	 *            The current set of "frontier" pixels --
	 *            any pixels assigned to the current region
	 *            will be removed from this frontier
	 */
	private void fillFrom(ImageLocation l, int regionID,
			SortedSet<ImageLocation> frontier) {

		// regionFrontier is the "frontier" of the current
		// region. It consists of locations that must be
		// probed to see if any neighbors are in the same
		// region. Obviously, since we start filling from
		// location l, it is initialized to l.
		SortedSet<ImageLocation> regionFrontier = new TreeSet<ImageLocation>();
		regionFrontier.add(l);

		while (!regionFrontier.isEmpty()) {
			// STEP 1: retrieve the next location and add it
			// to the current region
			ImageLocation current = regionFrontier.first();
			regionFrontier.remove(current);
			// System.out.println("fillFrom: current="
			// + current + " regionFrontier="
			// + regionFrontier);
			int x = current.x;
			int y = current.y;

			region[x][y] = regionID;

			// STEP 2: Examine all unassigned neighbors
			// (i.e. those that have no region yet) of the
			// current location, adding them to either (i)
			// the regionFrontier if there is no edge
			// separating them from the current location, or
			// (ii) the frontier, if there is a separating
			// edge
			ImageLocation newLoc;

			// up
			if (y > 0 && region[x][y - 1] == NO_REGION) {
				newLoc = new ImageLocation(x, y - 1);
				addToRegionOrFrontier(edgeMap.hor[x][y],
						newLoc, regionFrontier, frontier);
			}
			// down
			if (y < height - 1
					&& region[x][y + 1] == NO_REGION) {
				newLoc = new ImageLocation(x, y + 1);
				addToRegionOrFrontier(
						edgeMap.hor[x][y + 1], newLoc,
						regionFrontier, frontier);
			}
			// left
			if (x > 0 && region[x - 1][y] == NO_REGION) {
				newLoc = new ImageLocation(x - 1, y);
				addToRegionOrFrontier(edgeMap.ver[x][y],
						newLoc, regionFrontier, frontier);
			}

			// right
			if (x < width - 1
					&& region[x + 1][y] == NO_REGION) {
				newLoc = new ImageLocation(x + 1, y);
				addToRegionOrFrontier(
						edgeMap.ver[x + 1][y], newLoc,
						regionFrontier, frontier);
			}
		}
	}

	/**
	 * If isEdge is false, add newLoc to the regionFrontier,
	 * otherwise add it to the frontier
	 */
	private void addToRegionOrFrontier(boolean isEdge,
			ImageLocation newLoc,
			SortedSet<ImageLocation> regionFrontier,
			SortedSet<ImageLocation> frontier) {
		if (!isEdge) {
			addToRegionFrontier(newLoc, regionFrontier,
					frontier);
		} else {
			addToFrontier(newLoc, frontier);
		}
	}

	/**
	 * Add the given location to the regionFrontier, and
	 * remove it from the frontier if it is in there
	 */
	private void addToRegionFrontier(ImageLocation l,
			SortedSet<ImageLocation> regionFrontier,
			SortedSet<ImageLocation> frontier) {
		regionFrontier.add(l);
		frontier.remove(l);
	}

	/**
	 * Add the given location to the frontier, provided it
	 * has not already been assigned a region
	 */
	private void addToFrontier(ImageLocation l,
			SortedSet<ImageLocation> frontier) {
		if (region[l.x][l.y] == NO_REGION)
			frontier.add(l);
	}

	/**
	 * Create and return a numRegions x 3 array of random
	 * values between 0 and 255 to be used as colors
	 */
	private int[][] makeRandomRegionColors() {
		int[][] colors = new int[numRegions][3];
		for (int i = 0; i < numRegions; i++) {
			for (int j = 0; j < 3; j++) {
				int val = (int) Math
						.floor((Math.random() * 256));
				colors[i][j] = val;
			}
		}
		return colors;
	}

	/**
	 * Create and return a numRegions x 3 array of values
	 * between 0 and 255 to be used as colors. Element (i,0)
	 * the returned array is the average value of the red
	 * component for region i. Element (i,1) is the average
	 * for green and (i,2) for blue.
	 */
	private int[][] computeRegionAverages(
			BufferedImage original) {
		int[][] averages = new int[numRegions][3];
		int[][] sums = new int[numRegions][3];
		int[] counts = new int[numRegions];
		int[] rgb = new int[3];
		Raster r = original.getRaster();

		for (int i = 0; i < numRegions; i++)
			counts[i] = 0;

		for (int i = 0; i < numRegions; i++)
			for (int j = 0; j < 3; j++)
				sums[i][j] = 0;

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int regionID = region[i][j];
				counts[regionID]++;
				r.getPixel(i, j, rgb);
				for (int k = 0; k < 3; k++) {
					sums[regionID][k] += rgb[k];
				}
			}
		}

		for (int i = 0; i < numRegions; i++)
			for (int j = 0; j < 3; j++)
				averages[i][j] = sums[i][j] / counts[i];

		return averages;
	}

	private static void createAndShowGUI() {
		final int radius = 3;
		final int gap = 30;
		final int coarseGridSize = 2 * gap;
		// final int coarseGridSize = 1;
		final boolean stopBacktrackEarly = true;
		final String imageName = "C:\\Documents and Settings\\jmac\\Desktop\\test2.jpg";
		// final String imageName = "C:\\Documents and
		// Settings\\jmac\\Desktop\\tsukuba\\tsukuba\\scene1.row3.col5.jpg";
		final String imageResult = "C:\\Documents and Settings\\jmac\\Desktop\\result.png";
		final String imageResult2 = "C:\\Documents and Settings\\jmac\\Desktop\\result2.png";

		BufferedImage original = ImageUtils
				.getImageFromDisk(imageName);

		BufferedImage mutated = ImageUtils
				.copyImage(original);

		PathFinder pathFinder = new PathFinder(original,
				radius, coarseGridSize, gap,
				stopBacktrackEarly);

		pathFinder.computePathStrengths();
		pathFinder.makeBestPaths();
		RegionFiller rf = new RegionFiller(pathFinder);

		long start, stop;
		double duration;
		int num_iterations = 10;
		start = System.nanoTime();
		for (int i = 0; i < num_iterations; i++) {
			rf.computeRegions();
		}
		stop = System.nanoTime();
		duration = (stop - start) * 1e-9 / num_iterations;

		System.out.format("%f seconds per iteration%n",
				duration);

		pathFinder.drawPathMap(mutated);

		// boolean[][] edgeMap.ver = pathFinder
		// .getVerticalEdges();
		// boolean[][] edgeMap.hor = pathFinder
		// .getHorizontalEdges();

		// int x = 480;
		// int y = 360;
		// rf.restrictToSubWindow(x, y);
		// pathFinder.printSomeEdges(x, y);
		// rf.printEdges();

		// BufferedImage regionImage = rf
		// .makeImageWithRandomRegionColors();
		BufferedImage regionImage = rf
				.makeImageWithAverageRegionColors(original);

		System.out.println("numRegions = "
				+ rf.getNumRegions());

		// Save the resulting image
		ImageUtils.savePngImageToDisk(mutated, imageResult);
		ImageUtils.savePngImageToDisk(regionImage,
				imageResult2);

		// Create and set up the window.
		JFrame frame = new JFrame("PathFinder");
		frame.setLayout(new BorderLayout());
		frame
				.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		ImageComponent image1 = new ImageComponent(mutated);
		frame.add(image1, BorderLayout.PAGE_START);

		ImageComponent image2 = new ImageComponent(
				regionImage);
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
