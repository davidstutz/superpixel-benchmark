import java.awt.BorderLayout;
import java.awt.image.BufferedImage;
import java.awt.image.ConvolveOp;
import java.awt.image.Kernel;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.JFrame;

/**
 * A HorizontalEdgeFilter is capable of applying a simple
 * horizontal edge filter to an image. In other words, it
 * will compute the strength of vertical edges in the image.
 * 
 * @author jmac
 */
public class HorizontalEdgeFilter {
	private ConvolveOp rightSmooth;
	private ConvolveOp leftSmooth;

	/**
	 * Construct a new HorizontalEdgeFilter with the given
	 * radius
	 * 
	 * @param radius
	 *            The radius of the edge filter in pixels
	 */
	public HorizontalEdgeFilter(int radius) {
		int length = radius * 2;
		float[] rightData = new float[length];
		float[] leftData = new float[length];
		for (int i = 0; i < radius; i++) {
			rightData[i] = 0.0f;
			leftData[i] = 1.0f / radius;
		}
		for (int i = radius; i < length; i++) {
			rightData[i] = 1.0f / radius;
			leftData[i] = 0.0f;
		}
		// print kernel values for debugging purposes
		// System.out.println("right kernel values:");
		// for (int i = 0; i < length; i++)
		// System.out.format("%.3f ", rightData[i]);
		// System.out.println();
		// System.out.println("left kernel values:");
		// for (int i = 0; i < length; i++)
		// System.out.format("%.3f ", leftData[i]);
		// System.out.println();

		Kernel rightKernel = new Kernel(length, 1,
				rightData);
		Kernel leftKernel = new Kernel(length, 1, leftData);
		rightSmooth = new ConvolveOp(rightKernel);
		leftSmooth = new ConvolveOp(leftKernel);
	}

	/**
	 * Apply the pre-computed horizontal edge filter to the
	 * given image
	 * 
	 * @param sourceImage
	 *            The image whose edge strengths will be
	 *            computed
	 * @return A 2D array containing the strengths of the
	 *         vertical edges detected by the horizontal
	 *         edge filter. The array has the same width and
	 *         height as the input sourceImage.
	 */
	public int[][] filter(BufferedImage sourceImage) {
		int height = sourceImage.getHeight();
		int width = sourceImage.getWidth();
		Raster sourceRaster = sourceImage.getRaster();
		Raster rightSmoothed = smooth(sourceRaster,
				rightSmooth);
		Raster leftSmoothed = smooth(sourceRaster,
				leftSmooth);
		int[][] result = new int[width][height];
		int[] rightRgb = new int[3];
		int[] leftRgb = new int[3];
		int sum;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				rightSmoothed.getPixel(i, j, rightRgb);
				leftSmoothed.getPixel(i, j, leftRgb);
				sum = 0;
				for (int k = 0; k < 3; k++) {
					sum += Math.abs(rightRgb[k]
							- leftRgb[k]);
				}
				result[i][j] = sum;
			}
		}
		return result;
	}

	/**
	 * Apply the given convolution operation to the given
	 * raster, returning the result in a new raster.
	 */
	private Raster smooth(Raster source, ConvolveOp op) {
		WritableRaster dest = op
				.createCompatibleDestRaster(source);
		op.filter(source, dest);
		return dest;
	}

	private static void createAndShowGUI(
			HorizontalEdgeFilter horizontalEdgeFilter) {
		// Create and set up the window.
		JFrame frame = new JFrame("HorizontalEdgeFilter");
		frame.setLayout(new BorderLayout());
		frame
				.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// add the desired images to the window
		final String imageName = "C:\\Documents and Settings\\jmac\\Desktop\\test.jpg";
		File f = new File(imageName);
		BufferedImage bi1 = null;
		try {
			bi1 = ImageIO.read(f);
		} catch (IOException e) {
			System.out
					.println("exception thrown while reading image "
							+ imageName);
			e.printStackTrace();
		}
		ImageComponent image1 = new ImageComponent(bi1);
		frame.add(image1, BorderLayout.PAGE_START);

		int[][] filtered = horizontalEdgeFilter.filter(bi1);
		BufferedImage filteredImage = ImageUtils
				.transformValuesToImage(filtered);
		// WritableRaster r2 = bi2.getRaster();
		// horizontalEdgeFilter.getConvolveOp().filter(r1,
		// r2);
		ImageComponent image2 = new ImageComponent(
				filteredImage);
		frame.add(image2, BorderLayout.PAGE_END);

		// Display the window.
		frame.pack();
		frame.setVisible(true);

		// horizontalEdgeFilter.compareImageValues(r1, r2,
		// 100, 50, 5);
	}

	// //////////////////////////////////////////////////
	// some debugging routines
	// //////////////////////////////////////////////////

	// private void compareImageValues(Raster r1, Raster r2,
	// int x, int y, int width) {
	// System.out
	// .format(
	// " Comparing image values starting at (%d,%d)%n",
	// x, y);
	// printImageValues(r1, x, y, width);
	// printImageValues(r2, x, y, width);
	// }

	// private void printImageValues(Raster r, int x, int y,
	// int width) {
	// int[] rgb = new int[3];
	// for (int i = 0; i < width; i++) {
	// r.getPixel(x + i, y, rgb);
	// for (int j = 0; j < 3; j++) {
	// System.out.format(" %3d", rgb[j]);
	// }
	// System.out.print(":");
	// }
	// System.out.println();
	// }

	public static void main(/* String[] args */) {
		final int radius = 3;
		javax.swing.SwingUtilities
				.invokeLater(new Runnable() {
					public void run() {
						createAndShowGUI(new HorizontalEdgeFilter(
								radius));
					}
				});
	}

}
