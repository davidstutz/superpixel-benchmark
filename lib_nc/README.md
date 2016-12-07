Code to compute superpixels.
  by Greg Mori http://www.cs.sfu.ca/~mori/research/superpixels


The idea of superpixels was originally developed by Xiaofeng Ren and
Jitendra Malik [1].  This implementation is different, and is a
version of that used in [2],[3].

Basically, it is just a wrapper around Stella Yu's cncut code.  David
Martin and Charless Fowlkes' boundary detector[4] is used to provide an
edge map with which the "intervening contour" grouping cue is
computed.

Instructions for use:

- Run mex -largeArrayDims on *.c in yu_imncut directory
    -largeArrayDims is needed for 64bit architectures (thanks to Richard Lowe for providing fixes for 64bit MATLAB)
- Obtain mfm-pb boundary detector code from 
    http://www.cs.berkeley.edu/projects/vision/grouping/segbench/
- Change path names in sp_demo.m and pbWrapper.m
- Get a fast processor and lots of RAM
- Run sp_demo.m


References:

[1] X. Ren and J. Malik. Learning a classification model for
segmentation. In Proc. 9th Int. Conf. Computer Vision, volume 1, pages
10-17, 2003.

[2] G. Mori, X. Ren, A. Efros, and J. Malik, Recovering Human Body
Configurations: Combining Segmentation and Recognition, IEEE Computer
Vision and Pattern Recognition, 2004.

[3] G. Mori, Guiding Model Search Using Segmentation, IEEE
International Conference on Computer Vision, 2005.

[4] D. Martin, C. Fowlkes, and J. Malik, Learning to find brightness
and texture boundaries in natural images, NIPS, 2002.