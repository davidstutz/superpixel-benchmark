Subject: Waterpixels demo in SMIL

Copyright - Armines - Mines ParisTech - 2014
Creation date : December 2014
Contact : vaia.machairas@mines-paristech.fr

Linked to the following paper:
    "Waterpixels". Authors: V. Machairas, M. Faessel, D. Cardenas-Pena, T. Chabardes, T. Walter, E. Decencière.
    Submitted to IEEE Transaction on Image Processing in Dec. 2014.

Image taken from the Berkeley Segmentation database:
"A database of human segmented natural images and its application to evaluating segmentation algorithms and measuring ecological statistics." D. Martin, C. Fowlkes, D. Tal and J. Malik. In Int. Conf. on Computer Vision, vol. 2, p.416-423, July 2001.



You need to download the SMIL library first at: " http://smil.cmm.mines-paristech.fr/doc/index.html ".

To run the program (m-waterpixels, square cells) with ipython:

       run demo_waterpixels_smil_with_parser.py -i "montagne.jpg" -s 25 -w 10 --filter_ori      ## filtering of the original image
or     run demo_waterpixels_smil_with_parser.py -i "montagne.jpg" -s 25 -w 10 --no-filter_ori   ## no filtering of the original image

where:
- "i" is the original image you would like to partition into waterpixels;
- "s" is the grid step (int);
- "w" is the weight of the distance function for regularization (float).

Note that this program has only been tested under Linux. Not all optimizations are implemented in this public version.
