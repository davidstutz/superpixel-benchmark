import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.File;
import java.io.IOException;
import java.util.Vector;

import javax.imageio.ImageIO;
import javax.swing.JFrame;

/**
 * 
 * A collection of static methods for operating on images
 * 
 * @author jmac
 */
public class ImageUtils {

	/**
	 * Read an image from the disk and return it as a
	 * BufferedImage
	 * 
	 * @param imageName
	 *            Filename of the image to be read
	 * @return the image read from disk
	 */
	public static BufferedImage getImageFromDisk(
			String imageName) {
		File f = new File(imageName);
		BufferedImage bi = null;
		try {
			bi = ImageIO.read(f);
		} catch (IOException e) {
			System.out
					.println("exception thrown while reading image "
							+ imageName + ": " + e.getMessage());
			e.printStackTrace();
		}

                return convertToGrayScale(bi);
	}

        public static BufferedImage convertToGrayScale(BufferedImage image) {
                int width = image.getWidth();
                int height = image.getHeight();
                
                BufferedImage imageGray = new BufferedImage(width, height, 
                        BufferedImage.TYPE_BYTE_GRAY);
                for (int i = 0; i < height; i++) {
                        for (int j = 0; j < width; j++) {
                            Color c = new Color(image.getRGB(j, i));
                            int red = (int) (c.getRed() * 0.299);
                            int green = (int) (c.getGreen() * 0.587);
                            int blue = (int) (c.getBlue() * 0.114);
                            int sum = red + green + blue;
                            Color newColor = new Color(sum, sum, sum);
                            imageGray.setRGB(j, i, newColor.getRGB());
                        }
                }
                
                return imageGray;
        }
        
	/**
	 * Save the given image to disk in the PNG format
	 * 
	 * @param bi
	 *            the image to be saved
	 * @param imageName
	 *            the filename of the saved image
	 */
	public static void savePngImageToDisk(BufferedImage bi,
			String imageName) {
		try {
			File outputfile = new File(imageName);
			ImageIO.write(bi, "png", outputfile);
		} catch (IOException e) {
			System.out
					.println("exception thrown while writing image "
							+ imageName);
			e.printStackTrace();
		}
	}

	/**
	 * Return the transpose of the given image
	 * 
	 * @param source
	 *            the image to be transposed
	 * @return the transpose of the given image
	 */
	public static BufferedImage transposeImage(
			BufferedImage source) {
		int width = source.getHeight();
		int height = source.getWidth();
		int imageType = source.getType();
		BufferedImage dest = new BufferedImage(width,
				height, imageType);

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int rgb = source.getRGB(j, i);
				dest.setRGB(i, j, rgb);
			}
		}
		return dest;
	}

	/**
	 * Create a new copy of the given image
	 * 
	 * @param source
	 *            the image to be copied
	 * @return the new copy of the given image
	 */
	public static BufferedImage copyImage(
			BufferedImage source) {
		int height = source.getHeight();
		int width = source.getWidth();
		int imageType = source.getType();
		BufferedImage dest = new BufferedImage(width,
				height, imageType);

		// this could be sped up considerably, but stick
		// with the simple method for now
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int rgb = source.getRGB(i, j);
				dest.setRGB(i, j, rgb);
			}
		}
		return dest;
	}

	/**
	 * Make the given image entirely black
	 * 
	 * @param image
	 *            the image which will be made black
	 */
	public static void makeImageBlack(BufferedImage image) {
		WritableRaster r = image.getRaster();
		int[] vals = { 0, 0, 0 };
		for (int i = 0; i < image.getWidth(); i++) {
			for (int j = 0; j < image.getHeight(); j++) {
				r.setPixel(i, j, vals);
			}
		}
	}

	/**
	 * Draw a solid green rectangle onto the image
	 * 
	 * @param image
	 *            the image on which a green rectangle will
	 *            be drawn
	 * @param x1
	 *            left coordinate of the rectangle
	 * @param x2
	 *            right coordinate of the rectangle
	 * @param y1
	 *            top coordinate of the rectangle
	 * @param y2
	 *            bottom coordinate of the rectangle
	 */
	public static void makeGreenRectangle(
			BufferedImage image, int x1, int x2, int y1,
			int y2) {
		WritableRaster r = image.getRaster();
		int[] vals = { 0, 255, 0 };
		for (int i = x1; i < x2; i++) {
			for (int j = y1; j < y2; j++) {
				r.setPixel(i, j, vals);
			}
		}
	}

	/**
	 * Draw a solid green rectangle onto the image. The
	 * rectangle will be clipped if necessary to fit onto
	 * the image.
	 * 
	 * @param image
	 *            the image on which a green rectangle will
	 *            be drawn
	 * @param x1
	 *            left coordinate of the rectangle
	 * @param x2
	 *            right coordinate of the rectangle
	 * @param y1
	 *            top coordinate of the rectangle
	 * @param y2
	 *            bottom coordinate of the rectangle
	 */
	public static void makeRedRectangle(
			BufferedImage image, int x1, int x2, int y1,
			int y2) {
		WritableRaster r = image.getRaster();
		int[] vals = { 255, 0, 0 };

		// clip if necessary
		int width = image.getWidth();
		int height = image.getHeight();
		if (x1 < 0)
			x1 = 0;
		if (y1 < 0)
			y1 = 0;
		if (x2 >= width)
			x2 = width - 1;
		if (y2 >= height)
			y2 = height - 1;

		for (int i = x1; i < x2; i++) {
			for (int j = y1; j < y2; j++) {
				r.setPixel(i, j, vals);
			}
		}
	}

	/**
	 * Draw a red dot on an image
	 * 
	 * @param image
	 *            the image on which a red dot will be drawn
	 * @param x
	 *            horizontal location of the dot
	 * @param y
	 *            vertical location of the dot
	 */
	public static void makeRedDot(BufferedImage image,
			int x, int y) {
		WritableRaster r = image.getRaster();
		int[] vals = { 255, 0, 0 };
		r.setPixel(x, y, vals);
	}

	/**
	 * Draw a red cross on an image
	 * 
	 * @param image
	 *            the image on which a red cross will be
	 *            drawn
	 * @param x
	 *            horizontal location of the center of the
	 *            cross
	 * @param y
	 *            vertical location of the center of the
	 *            cross
	 * @param len
	 *            the length of the strokes of the cross
	 * @param width
	 *            the width of the strokes of the cross
	 */
	public static void makeRedCross(BufferedImage image,
			int x, int y, int len, int width) {
		makeRedRectangle(image, x - len / 2, x + len / 2, y
				- width / 2, y + width / 2);
		makeRedRectangle(image, x - width / 2, x + width
				/ 2, y - len / 2, y + len / 2);
	}

	/**
	 * Draw red crosses at several locations in an image
	 * 
	 * @param image
	 *            the image on which a red cross will be
	 *            drawn
	 * @param len
	 *            the length of the strokes of the crosses
	 * @param width
	 *            the width of the strokes of the crosses
	 * @param locations
	 *            the locations of the centers of the
	 *            crosses
	 */
	public static void drawRedCrosses(BufferedImage image,
			int len, int width,
			Vector<ImageLocation> locations) {
		for (ImageLocation l : locations) {
			makeRedCross(image, l.x, l.y, len, width);
		}
	}

	/**
	 * Transform a 2D array of nonnegative values into a
	 * grayscale image whose intensity is proportional to
	 * the given values. The values are scaled linearly to
	 * lie between 0 and 255 inclusive.
	 * 
	 * @param values
	 *            a 2D array of values that provide the
	 *            (unscaled) intensities of the resulting
	 *            image.
	 * @return the grayscale image containing the scaled
	 *         values
	 */
	public static BufferedImage transformValuesToImage(
			int[][] values) {
		int width = values.length;
		int height = values[0].length;
		int max = 0;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int value = values[i][j];
				if (max < value) {
					max = value;
				}
			}
		}
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				values[i][j] = values[i][j] * 255 / max;
			}
		}
		BufferedImage result = new BufferedImage(width,
				height, BufferedImage.TYPE_BYTE_GRAY);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int value = values[i][j];
				Color c = new Color(value, value, value);
				int argb = c.getRGB();
				result.setRGB(i, j, argb);
			}
		}
		return result;
	}

	/**
	 * Transform a 2D array of nonnegative values into a
	 * grayscale image whose intensity is proportional to
	 * the given values. The values are scaled linearly to
	 * lie between 0 and 255 inclusive.
	 * 
	 * @param values
	 *            a 2D array of values that provide the
	 *            (unscaled) intensities of the resulting
	 *            image.
	 * @return the grayscale image containing the scaled
	 *         values
	 */
	public static BufferedImage transformDoublesToImage(
			double[][] values) {
		int width = values.length;
		int height = values[0].length;
		double max = 0.0;
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				double value = values[i][j];
				if (max < value) {
					max = value;
				}
			}
		}
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				values[i][j] = values[i][j] * 255.0 / max;
			}
		}
		BufferedImage result = new BufferedImage(width,
				height, BufferedImage.TYPE_BYTE_GRAY);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int value = (int) values[i][j];
				Color c = new Color(value, value, value);
				int argb = c.getRGB();
				result.setRGB(i, j, argb);
			}
		}
		return result;
	}

	public static double[][] computeEdgeStrengths(
			BufferedImage im, int radius) {
		HorizontalEdgeFilter f = new HorizontalEdgeFilter(
				radius);
		int[][] horizontalFiltered = f.filter(im);
		BufferedImage transpose = transposeImage(im);
		int[][] verticalFiltered = f.filter(transpose);
		int width = im.getWidth();
		int height = im.getHeight();
		double[][] edgeStrengths = new double[width][height];
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int h = horizontalFiltered[i][j];
				int v = verticalFiltered[j][i];
				edgeStrengths[i][j] = Math.sqrt(h * h + v
						* v);
			}
		}
		return edgeStrengths;
	}

	private static void showImageInEventThread(
			BufferedImage im) {
		JFrame frame = new JFrame("Image Display");
		frame.setLayout(new BorderLayout());
		frame
				.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		ImageComponent image = new ImageComponent(im);
		frame.add(image, BorderLayout.PAGE_START);

		// Display the window.
		frame.pack();
		frame.setVisible(true);
	}

	public static void showImage(final BufferedImage im) {
		javax.swing.SwingUtilities
				.invokeLater(new Runnable() {
					public void run() {
						showImageInEventThread(im);
					}
				});
	}

	public static void testComputeEdgeStrengths(
			String args[]) {
		if (args.length != 1) {
			System.out
					.println("usage: java ImageUtils imageFilename");
			System.exit(0);
		}
		int radius = 3;
		BufferedImage im = getImageFromDisk(args[0]);
		double[][] edgeStrengths = computeEdgeStrengths(im,
				radius);
		BufferedImage edgeImage = transformDoublesToImage(edgeStrengths);
		showImage(edgeImage);
		System.out.println("done");
	}

	public static void main(String args[]) {
		testComputeEdgeStrengths(args);
	}

}
