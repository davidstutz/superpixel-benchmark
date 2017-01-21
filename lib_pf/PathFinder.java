import java.awt.BorderLayout;
import java.awt.image.BufferedImage;
import java.util.Vector;
import java.io.FileNotFoundException;
import java.io.UnsupportedEncodingException;
import java.io.File;
import java.lang.Math;
import java.nio.file.Paths;
import java.nio.file.Files;
import java.io.IOException;
import java.nio.file.StandardOpenOption;
import java.io.PrintWriter;

import javax.swing.JFrame;

/**
 * A PathFinder can compute and display strongest vertical
 * and horizontal paths in an image.
 * 
 * @author jmac
 */
public class PathFinder {

	// pathFinders for computing vertical and horizontal
	// paths respectively.
	private VerticalPathFinder pathFinder, pathFinderT;

	// seed locations for vertical paths are only chosen on
	// scanlines whose vertical coordinate is a multiple of
	// coarseGridSize
	private int coarseGridSize;

	/**
	 * Create a new PathFinder
	 * 
	 * @param image
	 *            The image in which strongest paths will be
	 *            found
	 * @param radius
	 *            The radius of the edge filter (in pixels)
	 *            used to compute vertical edge strengths
	 * @param coarseGridSize
	 *            The distance between scanlines, denoted G
	 *            in the paper, at which eligible seed
	 *            locations are considered
	 * @param gap
	 *            The minimum horizontal distance from the
	 *            seed of a new vertical path to an existing
	 *            one
	 * @param stopBacktrackEarly
	 *            When backtracking, should we stop as soon
	 *            as we run into an existing boundary?
	 */
	public PathFinder(BufferedImage image, int radius,
			int coarseGridSize, int gap,
			boolean stopBacktrackEarly) {
		this.coarseGridSize = coarseGridSize;
		BufferedImage imageT = ImageUtils
				.transposeImage(image);
		pathFinder = new VerticalPathFinder(image, radius,
				coarseGridSize, gap, stopBacktrackEarly);
		pathFinderT = new VerticalPathFinder(imageT,
				radius, coarseGridSize, gap,
				stopBacktrackEarly);
	}

	/**
	 * @return the width
	 */
	public int getWidth() {
		return pathFinder.getWidth();
	}

	/**
	 * @return the height
	 */
	public int getHeight() {
		return pathFinder.getHeight();
	}

	/**
	 * Create the set of strongest paths by choosing seed
	 * locations and backtracking from them. Seed locations
	 * are chosen according to the strength of their paths,
	 * subject to certain heuristics.
	 */
	public void makeBestPaths() {
		pathFinder.makeBestPaths();
		pathFinderT.makeBestPaths();
	}

	/**
	 * Create a set of strongest paths by choosing seed
	 * locations and backtracking from them. Seed locations
	 * are chosen according to the strength of their paths,
	 * subject to certain heuristics.
	 * 
	 * @param numPaths
	 *            The number of vertical and horizontal
	 *            paths to create
	 * @return A vector containing the chosen seed locations
	 */
	public Vector<ImageLocation> makeSomeBestPaths(
			int numPaths) {
		Vector<SeedLocation2D> vert;
		Vector<SeedLocation2D> hor;
		Vector<ImageLocation> both;
		// System.out.println("vertical:");
		vert = pathFinder.makeSomeBestPaths(numPaths);
		// System.out.println("horizontal:");
		hor = pathFinderT.makeSomeBestPaths(numPaths);
		both = new Vector<ImageLocation>();
//		System.out.println("vertical locations:");
		for (SeedLocation2D m : vert) {
			both.add(new ImageLocation(m.i, m.j
					* coarseGridSize));
		}
//		System.out.println("horizontal locations:");
		for (SeedLocation2D m : hor) {
			both.add(new ImageLocation(
					m.j * coarseGridSize, m.i));
		}
		return both;
	}

	/**
	 * make the best vertical path from the given seed
	 * location
	 * 
	 * @param x
	 *            horizontal coordinate of seed location
	 * @param y
	 *            vertical coordinate of seed location
	 */
	public void makeVerticalPath(int x, int y) {
		pathFinder.backtrack(x, y);
	}

	/**
	 * make the best vertical path from the given set of
	 * seed locations, which consists of a contiguous row of
	 * horizontal neighbors
	 * 
	 * @param xMin
	 *            left coordinate of seed locations
	 * @param xMax
	 *            right coordinate of seed locations, plus 1
	 * @param y
	 *            vertical coordinate of seed locations
	 */
	public void makeVerticalPath(int xMin, int xMax, int y) {
		pathFinder.backtrack(xMin, xMax, y);
	}

	/**
	 * make the best horizontal path from the given seed
	 * location
	 * 
	 * @param x
	 *            horizontal coordinate of seed location
	 * @param y
	 *            vertical coordinate of seed location
	 */
	public void makeHorizontalPath(int x, int y) {
		pathFinderT.backtrack(y, x);
	}

	/**
	 * make the best horizontal path from the given set of
	 * seed locations, which consists of a contiguous row of
	 * vertical neighbors
	 * 
	 * @param x
	 *            horizontal coordinate of seed locations
	 * @param yMin
	 *            top coordinate of seed locations
	 * @param yMax
	 *            bottom coordinate of seed locations, plus
	 *            1
	 */
	public void makeHorizontalPath(int x, int yMin, int yMax) {
		pathFinderT.backtrack(yMin, yMax, x);
	}

	/**
	 * Compute the path strengths of vertical and horizontal
	 * paths from every point in the image, and the local
	 * maxima in these strengths with respect to the
	 * horizontal coordinate
	 */
	public void computePathStrengths() {
		pathFinder.computePathStrengths();
		pathFinderT.computePathStrengths();
	}

	/**
	 * Compute the path strengths of vertical paths from
	 * every point in the image, and the local maxima in
	 * these strengths with respect to the horizontal
	 * coordinate
	 */
	public void computeVerticalPathStrengths() {
		pathFinder.computePathStrengths();
	}

	/**
	 * Draw the currently computed vertical and horizontal
	 * path maps onto the specified image
	 * 
	 * @param image
	 *            The image on which the paths will be drawn
	 */
	public void drawPathMap(BufferedImage image) {
		pathFinder.drawPathMap(image);
		pathFinderT.drawPathMapT(image);
	}

	/**
	 * Draw the detected maxima for vertical paths
	 * 
	 * @param image
	 *            The image on which maxima will be drawn
	 */
	public void drawVerticalMaxima(BufferedImage image) {
		pathFinder.drawMaxima(image);
	}

	/**
	 * @return vertical edges
	 */
	public boolean[][] getVerticalEdges() {
		return pathFinder.getPathMap();
	}

	/**
	 * @return horizontal edges
	 */
	public boolean[][] getHorizontalEdges() {
		return pathFinderT.getPathMapTranspose();
	}

	/**
	 * @return array containing the logical OR of the
	 *         vertical and horizontal edges
	 */
	public boolean[][] getEdges() {
		boolean[][] verticalEdges = pathFinder.getPathMap();
		boolean[][] horizontalEdgesT = pathFinderT
				.getPathMap();
		int width = pathFinder.getWidth();
		int height = pathFinder.getHeight();
		boolean[][] edges = new boolean[width][height];
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				edges[i][j] = verticalEdges[i][j]
						|| horizontalEdgesT[j][i];
			}
		}
		return edges;
	}

	/**
	 * Update the given edge map by adding horizontal and
	 * vertical edges according to the current path maps for
	 * both horizontal paths and vertical paths.
	 * 
	 * @param edgeMap
	 *            The EdgeMap to be updated
	 */
	public void updateEdgeMap(RegionFiller.EdgeMap edgeMap) {
		pathFinder.updateEdgeMap(edgeMap);
		pathFinderT.updateEdgeMapT(edgeMap);
	}

	/**
	 * Print out a representation of the current vertical
	 * path map, limited to the top left corner of the path
	 * map up to the given horizontal and vertical
	 * coordinates
	 * 
	 * @param x
	 *            horizontal limit
	 * @param y
	 *            vertical limit
	 */
	public void printSomeEdges(int x, int y) {
		pathFinder.printSomeEdges(x, y);
	}

	/**
	 * draw a couple of specific vertical paths for a figure
	 * in the paper
	 * 
	 * @param image
	 *            The image on which to draw
	 */
	public void bestVerticalExample(BufferedImage image) {
		this.computePathStrengths();

		int x = 38;
		int y = 305;
		this.makeVerticalPath(x, y);

		int xMin = 320;
		int xMax = 460;
		int y2 = 250;
		this.makeVerticalPath(xMin, xMax, y2);

		this.drawPathMap(image);
		ImageUtils.makeRedDot(image, x, y);
		ImageUtils.makeGreenRectangle(image, xMin, xMax,
				y2, y2 + 1);
	}

	/**
	 * draw a several strongest paths for a figure in the
	 * paper
	 * 
	 * @param image
	 *            The image on which to draw
	 */
	public void seededPathExamples(BufferedImage image) {
		this.computePathStrengths();

		int len = 20;
		int width = 4;

		int x1 = 415;
		int y1 = 150;
		this.makeVerticalPath(x1, y1);

		// int x1b=460;
		// int y1b=140;
		// this.makeVerticalPath(x1b, y1b);

		int x2 = 160;
		int y2 = 280;
		this.makeVerticalPath(x2, y2);

		int x3 = 200;
		int y3 = 50;
		this.makeHorizontalPath(x3, y3);

		int x4 = 350;
		int y4 = 250;
		this.makeHorizontalPath(x4, y4);

		this.drawPathMap(image);
		ImageUtils.makeRedCross(image, x1, y1, len, width);
		// ImageUtils.makeRedCross(image, x1b, y1b, len,
		// width);
		ImageUtils.makeRedCross(image, x2, y2, len, width);
		ImageUtils.makeRedCross(image, x3, y3, len, width);
		ImageUtils.makeRedCross(image, x4, y4, len, width);

	}

	/**
	 * Draw a particular path that "runs away", for a figure
	 * in the paper
	 * 
	 * @param image
	 *            The image on which to draw
	 */
	public void verticalRunawayExample(BufferedImage image) {
		this.computePathStrengths();

		// int x = 45;
		// int y = 320;
		int x = 380;
		int y = 140;
		this.makeVerticalPath(x, y);

		this.drawPathMap(image);
		ImageUtils.makeRedRectangle(image, x, x + 2, y,
				y + 2);
	}

	/**
	 * draw a couple of specific horizontal paths for a
	 * figure in the paper
	 * 
	 * @param image
	 *            The image on which to draw
	 */
	public void bestHorizontalExample(BufferedImage image) {
		this.computePathStrengths();

		int x = 200;
		int y = 50;
		this.makeHorizontalPath(x, y);

		int yMin = 150;
		int yMax = 300;
		int x2 = 350;
		this.makeHorizontalPath(x2, yMin, yMax);

		this.drawPathMap(image);
		ImageUtils.makeRedRectangle(image, x - 1, x + 2,
				y - 1, y + 2);
		ImageUtils.makeGreenRectangle(image, x2, x2 + 1,
				yMin, yMax);
	}

	/**
	 * Make the given image black and draw two green
	 * rectangles in it
	 */
	private static void makeBlackWithGreenRectangles(
			BufferedImage image) {
		ImageUtils.makeImageBlack(image);
		ImageUtils.makeGreenRectangle(image, 100, 300, 100,
				250);
		ImageUtils.makeGreenRectangle(image, 150, 250, 275,
				300);
	}

	private static void createAndShowGUI() {
		final int radius = 3;
		final int gap = 20;
		final int coarseGridSize = 2 * gap;
		// final int coarseGridSize = 1;
		final boolean stopBacktrackEarly = true;
		final String imageName = "/home/david/superpixels/workspace/3096.jpg";
		// final String imageName = "C:\\Documents and
		// Settings\\jmac\\My
		// Documents\\research\\superpixels\\experiment-images\\5.jpg";
		// final String imageName = "C:\\Documents and
		// Settings\\jmac\\My
		// Documents\\research\\superpixels\\experiment-images\\BSDS300\\images\\test\\24077.jpg";
		// final String imageName = "C:\\Documents and
		// Settings\\jmac\\Desktop\\tsukuba\\tsukuba\\scene1.row3.col5.jpg";
		final String imageResult = "/home/david/superpixels/workspace/3096_result.jpg";
		// final String imageResult = "C:\\Documents and
		// Settings\\jmac\\My
		// Documents\\research\\superpixels\\experiment-images\\BSDS300\\images\\test\\results\\24077-seg.jpg";

		BufferedImage original = ImageUtils
				.getImageFromDisk(imageName);

		// makeBlackWithGreenRectangles(original);

		BufferedImage mutated = ImageUtils
				.copyImage(original);

		PathFinder pathFinder = new PathFinder(original,
				radius, coarseGridSize, gap,
				stopBacktrackEarly);

		Vector<ImageLocation> locations = null;

		long start, stop;
		double duration;
		int num_iterations = 1;
		start = System.nanoTime();
		for (int i = 0; i < num_iterations; i++) {
			pathFinder.computePathStrengths();
			// pathFinder.makeBestPaths();
			locations = pathFinder.makeSomeBestPaths(50000);
		}
		stop = System.nanoTime();
		duration = (stop - start) * 1e-9 / num_iterations;

		System.out.format(
				"%f seconds to calculate grid paths%n",
				duration);

		// pathFinder.bestVerticalExample(mutated);
		// pathFinder.bestHorizontalExample(mutated);
		// pathFinder.verticalRunawayExample(mutated);

		pathFinder.drawPathMap(mutated);

//		ImageUtils
//				.drawRedCrosses(mutated, 20, 4, locations);

//		pathFinder.drawVerticalMaxima(mutated);

		// Save the resulting image
		ImageUtils.savePngImageToDisk(mutated, imageResult);

		// Create and set up the window.
		JFrame frame = new JFrame("PathFinder");
		frame.setLayout(new BorderLayout());
		frame
				.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		ImageComponent image1 = new ImageComponent(original);
		// frame.add(image1, BorderLayout.PAGE_START);

		ImageComponent image2 = new ImageComponent(mutated);
		frame.add(image2, BorderLayout.PAGE_END);

		// Display the window.
		frame.pack();
		frame.setVisible(true);
	}

        private static double runAndSave(String imageName, String csvResult, int radius, int superpixels) throws FileNotFoundException, UnsupportedEncodingException {
		
		// final int coarseGridSize = 1;
		final boolean stopBacktrackEarly = true;

		BufferedImage original = ImageUtils
				.getImageFromDisk(imageName);
                final int coarseGridSize = (int) (0.5 + Math.sqrt(original.getHeight()*original.getWidth() / (double) superpixels));
                final int gap = 2*coarseGridSize;
                
		// makeBlackWithGreenRectangles(original);

//		BufferedImage mutated = ImageUtils
//				.copyImage(original);

		PathFinder pathFinder = new PathFinder(original,
				radius, coarseGridSize, gap,
				stopBacktrackEarly);

		Vector<ImageLocation> locations = null;

		int num_iterations = 1;
		long start = System.nanoTime();
		for (int i = 0; i < num_iterations; i++) {
			pathFinder.computePathStrengths();
			// pathFinder.makeBestPaths();
			locations = pathFinder.makeSomeBestPaths(50000);
		}
		long stop = System.nanoTime();
		double duration = (stop - start) / 1000000000.0;

//		System.out.format(
//				"%f seconds to calculate grid paths%n",
//				duration);

//		pathFinder.bestVerticalExample(mutated);
//		pathFinder.bestHorizontalExample(mutated);
//		pathFinder.verticalRunawayExample(mutated);

//		pathFinder.drawPathMap(mutated);
//                ImageUtils.savePngImageToDisk(mutated, "test.png");
                
                RegionFiller filler = new RegionFiller(pathFinder);
                filler.saveRegionsToCSV(csvResult);
                
                return duration;
	}
        
	public static void main(String[] args) {
		if (args.length < 4 || args.length > 5) {
                        System.out.println("Usage: PathFinder in_folder out_folder radius superpixels [prefix]");
                        return;
                }
                
                int radius = Integer.parseInt(args[2]);
                int superpixels = Integer.parseInt(args[3]);
                String prefix = "";
                
                if (args.length > 4) {
                        prefix = args[4];
                }
                
                File folder = new File(args[0]);
                File csvFolder = new File(args[1]);
                
                // Create output directory if not existent.
                if (!csvFolder.exists()) {
                        try {
                                csvFolder.mkdirs();
                        }
                        catch (SecurityException e) {
                                System.out.println("Something went wrong:" + e.getMessage());
                                return;
                        }
                }
                
                double total = 0;
                int count = 0;
                
                for (final File fileEntry : folder.listFiles()) {
                        if (fileEntry.isDirectory()) {
                                // Nothing ...
                        }
                        else {
                                String fileName = fileEntry.getName();
                                int i = fileName.lastIndexOf('.');

                                String extension = "";
                                if (i > 0) {
                                        extension = fileName.substring(i + 1);
                                }
                                
                                if (extension.equals("jpg") || extension.equals("png") || extension.equals("jpeg")) {
                                        String csvFileName = csvFolder.getAbsolutePath() 
                                                + "/" + prefix + fileName.substring(0, i) + ".csv";

                                        try {
                                                total += runAndSave(fileEntry.getAbsolutePath(), csvFileName, radius, superpixels);
                                                count++;
                                        }
                                        catch (FileNotFoundException e) {
                                                System.out.println("Something went wrong: " + e.getMessage());
                                        }
                                        catch (UnsupportedEncodingException e) {
                                                System.out.println("Something went wrong: " + e.getMessage());
                                        }
                                }
                        }
                }
                
                System.out.println("Average time: " + Double.toString(total / count));
                
                String runtimeFileName = csvFolder.getAbsolutePath() 
                                                + "/" + prefix + "runtime.txt";
                
                try {
                    PrintWriter out = new PrintWriter(runtimeFileName);
                    out.println(Double.toString(total / count));
                    out.close();
                }
                catch (FileNotFoundException e) {
                    System.out.println("Something went wrong: " + e.getMessage());
                }
                
//                try {
//                        String runtimeFileName = csvFolder.getAbsolutePath() 
//                                + "/" + prefix + "runtime.txt";
//                        Files.write(Paths.get(runtimeFileName), Double.toString(total / count).getBytes(), 
//                                StandardOpenOption.APPEND);
//                }
//                catch (IOException e) {
//                        System.out.println("Something went wrong: " + e.getMessage());
//                }
	}

}
