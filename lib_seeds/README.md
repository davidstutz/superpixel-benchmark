
This is the source code to SEEDS superpixels, as described in M. Van den Bergh, X. Boix, G. Roig, B. de Capitani and L. Van Gool, "SEEDS: Superpixels Extracted via Energy-Driven Sampling", ECCV 2012.

The SEEDS class is defined in seeds.h and seeds2.cpp. The method can be run on an RGB image (stored in an UINT* array) by using the following constructor, initialization and iteration methods:

	SEEDS(int width, int height, int nr_channels, int nr_bins, int min_size);
	void initialize(UINT* image, int seeds_w, int seeds_h, int nr_levels);
	void iterate();

The initial size of the superpixels is independent from the image size, and defined by the following 3 parameters: <seed_width>, <seed_height> and <nr_levels>. The first two define the minimum block size in the SEEDS algorithm. The <nr_levels> defines the number of times this block size is doubled to reach the superpixel size. The superpixel size (width x height) is defined as follows: (seed_width*2^(nr_levels-1) x seed_height*2^(nr_levels-1)).

For example, if (seed_width x seed_height) = (3x4) and nr_levels = 4, then the initial superpixels will be rectangular and of size (3*2^3 x 4*2^3) = (24x32). Provided an image of size (480x320), this results in 20*10 = 200 superpixels.


Two usage examples are provided:


1. Evaluation demo (superpixel_test.cpp)

	This example is intended for use on the BSD300 or BSD500 dataset (http://www.eecs.berkeley.edu/Research/Projects/CS/vision/bsds/).

    Usage: bin/superpixel_test.cpp image_name [number_of_superpixels]

	A bash script is provided to run through a series of images (batch_process).


2. Live demo (demo.cpp)

	This examples takes images from a webcam and segments them independently and in real-time.



Furthermore, a mex file is provided for use in Matlab.


Disclaimer: the author of the code or ETH Zurich are not responsible for any damages that may result from using this code.
Copyright (c) 2012 Michael Van den Bergh (ETH Zurich). All rights reserved.
