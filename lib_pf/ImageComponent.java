import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.image.BufferedImage;

import javax.swing.JFrame;

/**
 * 
 * A component for displaying an image in a Swing window
 * 
 * @author jmac
 */
@SuppressWarnings("serial")
public class ImageComponent extends Component {
	BufferedImage bi;

	/**
	 * @param bi
	 *            The image that will be displayed
	 */
	public ImageComponent(BufferedImage bi) {
		this.bi = bi;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.awt.Component#getPreferredSize()
	 */
	public Dimension getPreferredSize() {
		return new Dimension(bi.getWidth(), bi.getHeight());
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.awt.Container#paint(java.awt.Graphics)
	 */
	@Override
	public void paint(Graphics g) {
		g.drawImage(bi, 0, 0, null);
		super.paint(g);
	}

	private static void createAndShowGUI() {
		final String imageName = "C:\\Documents and Settings\\jmac\\Desktop\\test2.jpg";
		BufferedImage original = ImageUtils
				.getImageFromDisk(imageName);
		JFrame frame = new JFrame("ImageComponent");
		frame.setLayout(new BorderLayout());
		frame
				.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		ImageComponent image1 = new ImageComponent(original);
		frame.add(image1, BorderLayout.PAGE_START);
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
