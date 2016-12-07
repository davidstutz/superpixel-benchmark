// ****************************************************************************** 
// SEEDS Superpixels
// ******************************************************************************
// Author: Michael Van den Bergh
// Contact: vamichae@vision.ee.ethz.ch
//
// This code implements the superpixel method described in:
// M. Van den Bergh, X. Boix, G. Roig, B. de Capitani and L. Van Gool, 
// "SEEDS: Superpixels Extracted via Energy-Driven Sampling",
// ECCV 2012
// 
// Copyright (c) 2012 Michael Van den Bergh (ETH Zurich). All rights reserved.
// ******************************************************************************

/**
 * Commented by David Stutz, a detailed discussion of the algorithm can be found
 * in [1] rather than in the ECCV paper [2].
 * 
 * [1]  M. Van den Bergh, X. Boix, G. Roig, B. de Capitani and L. Van Gool,
 *      "SEEDS: Superpixels Extracted via Energy-Driven Sampling", arXiv:1309.3848v1
 * [2]  M. Van den Bergh, X. Boix, G. Roig, B. de Capitani and L. Van Gool,
 *      "SEEDS: Superpixels Extracted via Energy-Driven Sampling", ECCV 2012
 */

#include "seeds2.h"
#include "math.h"
#include <cstdio>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>

// Choose color space, LAB is better, HSV is faster (initialization).
#define LAB_COLORSPACE
//#define HSV_COLORSPACE

// Enables means post-processing
// #define MEANS

// Enables 3x3 smoothing prior
// #define PRIOR

// If enabled, each block-updating level will be iterated twice.
// Disable this to speed up the algorithm.
// #define DOUBLE_STEPS

// The minimum confidence needed for block/pixel updates.
// #define REQ_CONF 0.1

#define MINIMUM_NR_SUBLABELS 1

/**
 * Main iteration method. Performs one (or a fixed number) iteration at each level
 * including the pixel level.
 * 
 * The iterative nature of SEEDS described in the paper is "lost" because the user
 * can not simply abort after each iteration without loosing too much quality.
 */
void SEEDS::iterate(int iterations) 
{
	// Begin with block updates at each level.
        // update_blocks moves every block at the current level to the neighbouring
        // superpixel with the highest intersection.
	while (seeds_current_level >= 0)
	{
                for (int iteration = 0; iteration < iterations; ++iteration) {
                        update_blocks(seeds_current_level);
                }
                
		seeds_current_level = go_down_one_level();
	}

        // End with several pixel updates, that is exchange contour pixels between
        // neighbouring superpixels.
	if (means) {
		compute_means();
                
                for (int iteration = 0; iteration < iterations; ++iteration) {
                        update_pixels_means();
                }
	}
        else {
                for (int iteration = 0; iteration < iterations; ++iteration) {
                        update_pixels();
                }
        }
}

/**
 * Constructor.
 * 
 * Arguments define width and height of the image, the number of channels used,
 * the number of bins for the histograms.
 * 
 * @param width
 * @param height
 * @param nr_channels
 * @param nr_bins
 * @param min_size
 */
SEEDS::SEEDS(int width, int height, int nr_channels, int nr_bins, int min_size, float confidence, bool prior, bool means, int color)
{
	this->width = width;
	this->height = height;
	this->nr_channels = nr_channels;
	this->nr_bins = nr_bins;
	this->merge_threshold = merge_threshold;
	this->min_size = min_size;
        this->confidence = confidence;
        this->prior = prior;
        this->means = means;
        this->color = color;
        
	image_bins = new UINT[width*height];
	image_l = new float[width*height];
	image_a = new float[width*height];
	image_b = new float[width*height];
	edge_w = new float[width*height];
	edge_h = new float[width*height];
	forwardbackward = true;
	histogram_size = nr_bins*nr_bins*nr_bins;
	initialized = false;
}

/**
 * Destructor.
 */
SEEDS::~SEEDS()
{
	// from constructor
	delete[] image_bins;
	delete[] image_l;
	delete[] image_a;
	delete[] image_b;
	delete[] edge_w;
	delete[] edge_h;

	if (initialized)
	{
		for (int level=0; level<seeds_nr_levels; level++)
		{
			for (int label=0; label<nr_labels[level]; label++)
			{
				delete[] histogram[level][label];
			}
			delete[]  histogram[level];
			delete[] T[level];
			delete[] labels[level];
			delete[] parent[level];
			delete[] nr_partitions[level];

		}
		delete[] histogram;
		delete[] T;
		delete[] labels;
		delete[] parent;
		delete[] nr_partitions;
		delete[] nr_labels;
		delete[] nr_w;
		delete[] nr_h;
		if (color == 0 || color == 1) delete[] bin_cutoff1;
		if (color == 0 || color == 1) delete[] bin_cutoff2;
		if (color == 0 || color == 1) delete[] bin_cutoff3;

		if (means) {
                        delete[] L_channel;
                        delete[] A_channel;
                        delete[] B_channel;
		}
	}
}

/**
 * Initialize the algorithm.
 * 
 * The initial superpixel size is defined by (seed_w*2^(nr_levels-1) x seed_h*2^(nr_levels-1)),
 * such that nr_levels defines how often the size of the smallest blocks are doubled to
 * get the superpixel size.
 * 
 * @param image
 * @param seeds_w
 * @param seeds_h
 * @param nr_levels
 */
void SEEDS::initialize(UINT* image, int seeds_w, int seeds_h, int nr_levels)
{
	iteration = 0;

	this->seeds_w = seeds_w;
	this->seeds_h = seeds_h;
        
        // Minimum number of levels used for testing on BSDS500 is 4, at least 3
        // levels are necessary not including the pixel level.
	this->seeds_nr_levels = nr_levels;
	this->seeds_top_level = nr_levels - 1;
	this->seeds_current_level = nr_levels - 2;

	// Assign the labels and set up arrays to manage the labels
        // at each level.
	assign_labels();
	
//	#ifdef LAB_COLORSPACE
        if (color == 0)
        {
                rgb_get_histogram_cutoff_values(image);
        }
        else if (color == 1)
        { //Lab
		lab_get_histogram_cutoff_values(image);
        }
//	#endif

	// Convert the image into LAB or
	for (int x=0; x<width; x++)
		for (int y=0; y<height; y++)
		{
			int i = y*width + x;
			int r = (image[i] >> 16) & 0xFF;
			int g = (image[i] >>  8) & 0xFF;
			int b = (image[i]      ) & 0xFF;
			float L;
			float A;
			float B;
                        
                        if (color == 0) // RGB
                        {
                                image_bins[i] = RGB_special(r, g, b, &L, &A, &B);
				image_l[i] = r/255.0;
				image_a[i] = g/255.0;
				image_b[i] = b/255.0;
                        }
                        else if (color == 1) // Lab
                        {
//			#ifdef LAB_COLORSPACE
				image_bins[i] = RGB2LAB_special(r, g, b, &L, &A, &B);
				image_l[i] = L/100.0;
				image_a[i] = (A+128.0)/255.0;
				image_b[i] = (B+128.0)/255.0;
//			#endif
                        }
                        else if (color == 2) // HSV
                        {
//			#ifdef HSV_COLORSPACE
				image_bins[i] = RGB2HSV(r, g, b, &L, &A, &B);
                                image_l[i] = L;
				image_a[i] = A;
				image_b[i] = B;
//			#endif
                        }
                        else
                        {
                            std::cout << "Invalid color space!" << std::endl;
                            return;
                        }
		}

	compute_histograms();

	initialized = true;
}

/**
 * Initialize the algorithm using an OpenCV Matrix.
 * 
 * @param image
 * @param seeds_w
 * @param seeds_h
 * @param nr_levels
 */
void SEEDS::initialize(const cv::Mat &image, int seeds_w, int seeds_h, int nr_levels)
{
	iteration = 0;

	this->seeds_w = seeds_w;
	this->seeds_h = seeds_h;
        
        // Minimum number of levels used for testing on BSDS500 is 4, at least 3
        // levels are necessary not including the pixel level.
	this->seeds_nr_levels = nr_levels;
	this->seeds_top_level = nr_levels - 1;
	this->seeds_current_level = nr_levels - 2;

	// Assign the labels and set up arrays to manage the labels
        // at each level.
	assign_labels();
	
//	#ifdef LAB_COLORSPACE
        if (color == 0)
        {
                rgb_get_histogram_cutoff_values(image);
        }
        else if (color == 1)
        { //Lab
		lab_get_histogram_cutoff_values(image);
        }
//	#endif

	// Convert the image into LAB or
	for (int x=0; x<width; x++)
		for (int y=0; y<height; y++)
		{
			int i = y*width + x;
			int b = image.at<cv::Vec3b>(y, x)[0];
			int g = image.at<cv::Vec3b>(y, x)[0];
			int r = image.at<cv::Vec3b>(y, x)[0];
			float L;
			float A;
			float B;
			if (color == 0) // RGB
                        {
                                image_bins[i] = RGB_special(r, g, b, &L, &A, &B);
				image_l[i] = r/255.0;
				image_a[i] = g/255.0;
				image_b[i] = b/255.0;
                        }
                        else if (color == 1) // Lab
                        {
//			#ifdef LAB_COLORSPACE
				image_bins[i] = RGB2LAB_special(r, g, b, &L, &A, &B);
				image_l[i] = L/100.0;
				image_a[i] = (A+128.0)/255.0;
				image_b[i] = (B+128.0)/255.0;
//			#endif
                        }
                        else if (color == 2) // HSV
                        {
//			#ifdef HSV_COLORSPACE
				image_bins[i] = RGB2HSV(r, g, b, &L, &A, &B);
                                image_l[i] = L;
				image_a[i] = A;
				image_b[i] = B;
//			#endif
                        }
                        else
                        {
                            std::cout << "Invalid color space!" << std::endl;
                            return;
                        }
		}

	compute_histograms();

	initialized = true;
}

/**
 * Called in initialize, the method initializes all arrays needed to managing the labels 
 * on all levels:
 * 
 * - labels: contains the labels for each pixel at each level.
 * - parent: contains the superpixel label for each pixel, this is updated by add_block and delete_block.
 * - nr_partitions: managed by add_block and delete_block, as well, this array keeps
 * track of the number of blocks per superpixel at each level.
 * - nr_labels: contains the number of labels for each level.
 * - nr_w and nr_h: contain the number of blocks at each level in x and y direction
 */
void SEEDS::assign_labels()
{
        // Two dimensional array to assign each pixel a label at each level: labels[seeds_nr_levels][width*height].
	labels = new UINT*[seeds_nr_levels];
        
        // Two dimensional array containing the superpixel label for each pixel on the current level.
	parent = new UINT*[seeds_nr_levels];
        
        // Counts the number of subblocks a label at level l can be divided into.
        // This is managed in add_block and delete_block.
	nr_partitions = new UINT*[seeds_nr_levels];
        
        // Contains the total number of labels at each level.
	nr_labels = new UINT[seeds_nr_levels];
        
        // Arrays to contain the number of blocks in x and y direction, respectively.
	nr_w = new int[seeds_nr_levels];
	nr_h = new int[seeds_nr_levels];

	// Base level: 0.
	int level = 0;
	int nr_seeds_w = floor(width/seeds_w);
	int nr_seeds_h = floor(height/seeds_h);
        
        // Blocks have size seeds_w x seeds_h.
	int step_w = seeds_w;
	int step_h = seeds_h;
	int nr_seeds = nr_seeds_w*nr_seeds_h;
        
	nr_labels[level] = nr_seeds;
	nr_w[level] = nr_seeds_w;
	nr_h[level] = nr_seeds_h;
	labels[level] = new UINT[width*height];
	parent[level] = new UINT[nr_seeds];
	nr_partitions[level] = new UINT[nr_seeds];
        
        // At base level there is no further partitioning, each pixel
        // contains exactly one pixel.
	for (int i=0; i<nr_seeds; i++) nr_partitions[level][i] = 1;
        
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
                        // Note the handling of border effects if the image cannot
                        // be subdivided into blocks without leaving some
                        // rest pixels at the borders: the blocks at the borders
                        // may be larger than block in the middle.
			int label_x = floor((float)x/step_w);
			if (label_x >= nr_seeds_w) label_x = nr_seeds_w-1;
			int label_y = floor((float)y/step_h);
			if (label_y >= nr_seeds_h) label_y = nr_seeds_h-1;
			labels[level][y*width+x] = label_y*nr_seeds_w + label_x;
			// parents will be set in the next loop
		}

	for (int level = 1; level < seeds_nr_levels; level++)
	{
                // In each level, the size of the blocks is doubled (both width and height)
                // such that a block at level l is subdivided into 4 block at level l-1.
		nr_seeds_w /= 2;
		nr_seeds_h /= 2;
		step_w *= 2;
		step_h *= 2;
		nr_seeds = nr_seeds_w*nr_seeds_h;
                
		nr_labels[level] = nr_seeds;
		nr_w[level] = nr_seeds_w;
		nr_h[level] = nr_seeds_h;
		labels[level] = new UINT[width*height];
		parent[level] = new UINT[nr_seeds];
		nr_partitions[level] = new UINT[nr_seeds];
                
                // nr_partitions is managed in add_block and delete_block, so
                // just initialize with zero.
		for (int i=0; i<nr_seeds; i++) nr_partitions[level][i] = 0;
                
		for (int y=0; y<height; y++)
			for (int x=0; x<width; x++)
			{
				int label_x = floor((float)x/step_w);
				if (label_x >= nr_seeds_w) label_x = nr_seeds_w-1;
				int label_y = floor((float)y/step_h);
				if (label_y >= nr_seeds_h) label_y = nr_seeds_h-1;
				labels[level][y*width+x] = label_y*nr_seeds_w + label_x;
                                
                                // Set the superpixel label for each pixel in this level.
				parent[level-1][labels[level-1][y*width+x]] = labels[level][y*width+x];
			}
	}

	seeds_top_level = seeds_nr_levels - 1;
}

void SEEDS::lab_get_histogram_cutoff_values(UINT* image)
{
	// get image lists and histogram cutoff values
	vector<float> list_channel1;
	vector<float> list_channel2;
	vector<float> list_channel3;
	vector<float>::iterator it;
	int samp = 5;
	int ctr = 0;
	for (int x=0; x<width; x+=samp)
		for (int y=0; y<height; y+=samp)
		{
			int i = y*width +x;
			int r = (image[i] >> 16) & 0xFF;
			int g = (image[i] >>  8) & 0xFF;
			int b = (image[i]      ) & 0xFF;
			float L = 0;
			float A = 0;
			float B = 0;
			RGB2LAB(r, g, b, &L, &A, &B);
			list_channel1.push_back(L);
			list_channel2.push_back(A);
			list_channel3.push_back(B);
			ctr++;
		}
	bin_cutoff1 = new float[nr_bins];
	bin_cutoff2 = new float[nr_bins];
	bin_cutoff3 = new float[nr_bins];
	for (int i=1; i<nr_bins; i++)
	{
		int N = (int) floor((float) (i*ctr)/ (float)nr_bins);
		nth_element(list_channel1.begin(), list_channel1.begin()+N, list_channel1.end());
		it = list_channel1.begin()+N;
		bin_cutoff1[i-1] = *it;
		nth_element(list_channel2.begin(), list_channel2.begin()+N, list_channel2.end());
		it = list_channel2.begin()+N;
		bin_cutoff2[i-1] = *it;
		nth_element(list_channel3.begin(), list_channel3.begin()+N, list_channel3.end());
		it = list_channel3.begin()+N;
		bin_cutoff3[i-1] = *it;
	}
	bin_cutoff1[nr_bins-1] = 300;
	bin_cutoff2[nr_bins-1] = 300;
	bin_cutoff3[nr_bins-1] = 300;
}

void SEEDS::lab_get_histogram_cutoff_values(const cv::Mat &image)
{
	// get image lists and histogram cutoff values
	vector<float> list_channel1;
	vector<float> list_channel2;
	vector<float> list_channel3;
	vector<float>::iterator it;
	int samp = 5;
	int ctr = 0;
	for (int x=0; x<width; x+=samp)
		for (int y=0; y<height; y+=samp)
		{
			int i = y*width +x;
                        int b = image.at<cv::Vec3b>(y, x)[0];
                        int g = image.at<cv::Vec3b>(y, x)[1];
                        int r = image.at<cv::Vec3b>(y, x)[2];
			float L = 0;
			float A = 0;
			float B = 0;
			RGB2LAB(r, g, b, &L, &A, &B);
			list_channel1.push_back(L);
			list_channel2.push_back(A);
			list_channel3.push_back(B);
			ctr++;
		}
	bin_cutoff1 = new float[nr_bins];
	bin_cutoff2 = new float[nr_bins];
	bin_cutoff3 = new float[nr_bins];
	for (int i=1; i<nr_bins; i++)
	{
		int N = (int) floor((float) (i*ctr)/ (float)nr_bins);
		nth_element(list_channel1.begin(), list_channel1.begin()+N, list_channel1.end());
		it = list_channel1.begin()+N;
		bin_cutoff1[i-1] = *it;
		nth_element(list_channel2.begin(), list_channel2.begin()+N, list_channel2.end());
		it = list_channel2.begin()+N;
		bin_cutoff2[i-1] = *it;
		nth_element(list_channel3.begin(), list_channel3.begin()+N, list_channel3.end());
		it = list_channel3.begin()+N;
		bin_cutoff3[i-1] = *it;
	}
	bin_cutoff1[nr_bins-1] = 300;
	bin_cutoff2[nr_bins-1] = 300;
	bin_cutoff3[nr_bins-1] = 300;
}

void SEEDS::rgb_get_histogram_cutoff_values(UINT* image)
{
	// get image lists and histogram cutoff values
	vector<float> list_channel1;
	vector<float> list_channel2;
	vector<float> list_channel3;
	vector<float>::iterator it;
	int samp = 5;
	int ctr = 0;
	for (int x=0; x<width; x+=samp)
		for (int y=0; y<height; y+=samp)
		{
			int i = y*width +x;
                        int r = (image[i] >> 16) & 0xFF;
			int g = (image[i] >>  8) & 0xFF;
			int b = (image[i]      ) & 0xFF;
			list_channel1.push_back(b);
			list_channel2.push_back(g);
			list_channel3.push_back(r);
			ctr++;
		}
	bin_cutoff1 = new float[nr_bins];
	bin_cutoff2 = new float[nr_bins];
	bin_cutoff3 = new float[nr_bins];
	for (int i=1; i<nr_bins; i++)
	{
		int N = (int) floor((float) (i*ctr)/ (float)nr_bins);
		nth_element(list_channel1.begin(), list_channel1.begin()+N, list_channel1.end());
		it = list_channel1.begin()+N;
		bin_cutoff1[i-1] = *it;
		nth_element(list_channel2.begin(), list_channel2.begin()+N, list_channel2.end());
		it = list_channel2.begin()+N;
		bin_cutoff2[i-1] = *it;
		nth_element(list_channel3.begin(), list_channel3.begin()+N, list_channel3.end());
		it = list_channel3.begin()+N;
		bin_cutoff3[i-1] = *it;
	}
	bin_cutoff1[nr_bins-1] = 300;
	bin_cutoff2[nr_bins-1] = 300;
	bin_cutoff3[nr_bins-1] = 300;
}

void SEEDS::rgb_get_histogram_cutoff_values(const cv::Mat &image)
{
	// get image lists and histogram cutoff values
	vector<float> list_channel1;
	vector<float> list_channel2;
	vector<float> list_channel3;
	vector<float>::iterator it;
	int samp = 5;
	int ctr = 0;
	for (int x=0; x<width; x+=samp)
		for (int y=0; y<height; y+=samp)
		{
			int i = y*width +x;
                        int b = image.at<cv::Vec3b>(y, x)[0];
                        int g = image.at<cv::Vec3b>(y, x)[1];
                        int r = image.at<cv::Vec3b>(y, x)[2];
			list_channel1.push_back(b);
			list_channel2.push_back(g);
			list_channel3.push_back(r);
			ctr++;
		}
	bin_cutoff1 = new float[nr_bins];
	bin_cutoff2 = new float[nr_bins];
	bin_cutoff3 = new float[nr_bins];
	for (int i=1; i<nr_bins; i++)
	{
		int N = (int) floor((float) (i*ctr)/ (float)nr_bins);
		nth_element(list_channel1.begin(), list_channel1.begin()+N, list_channel1.end());
		it = list_channel1.begin()+N;
		bin_cutoff1[i-1] = *it;
		nth_element(list_channel2.begin(), list_channel2.begin()+N, list_channel2.end());
		it = list_channel2.begin()+N;
		bin_cutoff2[i-1] = *it;
		nth_element(list_channel3.begin(), list_channel3.begin()+N, list_channel3.end());
		it = list_channel3.begin()+N;
		bin_cutoff3[i-1] = *it;
	}
	bin_cutoff1[nr_bins-1] = 300;
	bin_cutoff2[nr_bins-1] = 300;
	bin_cutoff3[nr_bins-1] = 300;
}

/**
 * As described in [2], it is beneficial to do the last pixel updates meand-based
 * as also used by SLIC superpixels.
 * 
 * This method computes the means of the superpixels necessary for the updates.
 */
void SEEDS::compute_means()
{
	L_channel = new float[nr_labels[seeds_top_level]];
	A_channel = new float[nr_labels[seeds_top_level]];
	B_channel = new float[nr_labels[seeds_top_level]];

	// clear counted LAB values
	for (int label=0; label<nr_labels[seeds_top_level]; label++)
	{
		L_channel[label] = 0;
		A_channel[label] = 0;
		B_channel[label] = 0;
	}

	// sweep image
	for (int i=0; i<width*height; i++)
	{
		int label = labels[seeds_top_level][i];
		// add LAB values to total
		L_channel[label] += image_l[i];
		A_channel[label] += image_a[i];
		B_channel[label] += image_b[i];
	}
}

/**
 * Histograms are initialized in the first iteration and built up layer wise,
 * beginning with the first block level.
 * 
 * The histograms are built using add_pixel and add_block as they automatically
 * update the histograms.
 * 
 * In initialization the nr_partitions array is set using add_block.
 * 
 * @param until_level
 */
void SEEDS::compute_histograms(int until_level)
{
	if (until_level == -1) until_level = seeds_nr_levels - 1;
	until_level++;

	// Initialize the histrograms in the first iteration.
	if (iteration == 0)
	{
                // Histograms are initialized for each label in each level: histogram[level][label][bin].
		histogram = new int**[seeds_nr_levels];
                
                
		T = new int*[seeds_nr_levels]; // block sizes are kept at each level [level][label]
		for (int level=0; level<seeds_nr_levels; level++)
		{
			histogram[level] = new int*[nr_labels[level]];
                        
                        // Block sizes are kept at each level: T[level][label].
			T[level] = new int[nr_labels[level]]; 
			for (int label=0; label<nr_labels[level]; label++)
			{
				histogram[level][label] = new int[histogram_size]; // histogram bins
			}
		}
	}

	// Initialize empty histograms - this could also be done in the for loop before.
	for (int level=0; level<seeds_nr_levels; level++)
		for (int label=0; label<nr_labels[level]; label++)
		{
			for (int j=0; j<histogram_size; j++)
				histogram[level][label][j] = 0;
			T[level][label] = 0;
		}

	// Histograms are built in a level-wise manner, that is first the histograms
        // for the first level are built using the pixels, then the histograms
        // can simply be added to for the histogram of blocks at the next level.
        // The histograms are built using the add_pxiel and add_block methods
        // which automatically update the histograms.
	for (int x=0; x<width; x++)
		for (int y=0; y<height; y++)
		{					
			int i = y*width +x;
			add_pixel(0, labels[0][i], x, y);
		}

	for (int level=1; level<until_level; level++)
	{
		for (int label=0; label<nr_labels[level-1]; label++)
		{
			add_block(level, parent[level-1][label], level-1, label);
		}
	}
}

/**
 * Compute the histograms explicitely, that is without using add_pixel or
 * add_block as in compute_histograms.
 */
void SEEDS::compute_histograms_ex()
{
	// clear histograms
	for (int level=0; level<seeds_nr_levels; level++)
		for (int label=0; label<nr_labels[level]; label++)
		{
			for (int j=0; j<histogram_size; j++)
				histogram[level][label][j] = 0;
			T[level][label] = 0;
		}

	for (int level=0; level<seeds_nr_levels; level++)
		for (int x=0; x<width; x++)
			for (int y=0; y<height; y++)
			{					
				int i = y*width +x;
				//add_pixel(level, labels[level][i], x, y);
				histogram[level][labels[level][i]][image_bins[y*width+x]]++;
				T[level][labels[level][i]]++;
			}

}

void SEEDS::compute_edges()
{
	// compute edges
	/*for (int x=0;x<width-1; x++)
		for (int y=0; y<height; y++)
		{
			float Ex = image_l[y*width+x] - image_l[y*width+(x+1)];
			#ifdef HSV_COLORSPACE
				if (Ex > 0.5) Ex = 1.0 - Ex;
			#endif
			float Elx = image_a[y*width+x] - image_a[y*width+(x+1)];
			float Ellx = image_b[y*width+x] - image_b[y*width+(x+1)];
			edge_w[y*width+x] = sqrt(Ex*Ex + Elx*Elx + Ellx*Ellx);
		}
	for (int y=0;y<height-1; y++)
		for (int x=0; x<width; x++)
		{
			float Ey = image_l[y*width+x] - image_l[(y+1)*width+x];
			#ifdef HSV_COLORSPACE
				if (Ey > 0.5) Ey = 1.0 - Ey;
			#endif
			float Ely = image_a[y*width+x] - image_a[(y+1)*width+x];
			float Elly = image_b[y*width+x] - image_b[(y+1)*width+x];
			edge_h[y*width+x] = sqrt(Ey*Ey + Ely*Ely + Elly*Elly);
		}*/
}

/**
 * Performs greedy exchanging of blocks. All blocks at the current level are checked.
 * If there is no exchange in horizontal direction, vertical direction is checked as well.
 * The block is assigned to the label with higher intersection which is checked first.
 * 
 * The only exception to the greedy exchanging of blocks is req_confidence. A block is
 * not moved to a superpixel with intersection confidence not greater than req_confidence.
 * 
 * @param level
 * @param req_confidence
 */
void SEEDS::update_blocks(int level, float req_confidence)
{
	int labelA;
	int labelB;
	int sublabel;
	int priorA;
	int priorB;
        
        // If at least one block has been exchanged, this will be set to true.
        // The algorithm will search for blocks to exchange until one is exchanged.
	bool done;

	int step = nr_w[level];
	
	// Horizontal exchanging of blocks.
        // If the chosen block will not be exchanged with the the superpixel on
        // the one side, try the other side (bidirectional).
	for (int y=1; y<nr_h[level]-1; y++)
		for (int x=1; x<nr_w[level]-1; x++) 
		{
			// choose a label at the current level
			sublabel = y*step+x; 
			// get the label at the top level (= superpixel label)
			labelA = parent[level][y*step+x]; 
			// get the neighboring label at the top level (= superpixel label)
			labelB = parent[level][y*step+x+1]; 

			if (labelA != labelB)
			{
				// get the surrounding labels at the top level, to check for splitting 
				int a11 = parent[level][(y-1)*step+(x-1)]; 
				int a12 = parent[level][(y-1)*step+(x)];
				int a13 = parent[level][(y-1)*step+(x+1)];
				int a14 = parent[level][(y-1)*step+(x+2)];
				int a21 = parent[level][(y)*step+(x-1)];
				int a22 = parent[level][(y)*step+(x)];
				int a23 = parent[level][(y)*step+(x+1)];
				int a24 = parent[level][(y)*step+(x+2)];
				int a31 = parent[level][(y+1)*step+(x-1)];
				int a32 = parent[level][(y+1)*step+(x)];
				int a33 = parent[level][(y+1)*step+(x+1)];
				int a34 = parent[level][(y+1)*step+(x+2)];

				done = false;

                                // At level l, each block has a minimum size of 
                                // MINIMUM_NR_SUBLABELS block at level l-1.
				if (nr_partitions[seeds_top_level][labelA] > MINIMUM_NR_SUBLABELS)
				{
					// If the label can only be subdivided into 2 blocks.
					if (nr_partitions[seeds_top_level][labelA] <= 2)
					{
						// Delete the block from label A and compute the intersection of 
                                                // the sublabel with both label A and B.
						delete_block(seeds_top_level, labelA, level, sublabel);
						float intA = intersection(seeds_top_level, labelA, level, sublabel);
						float intB = intersection(seeds_top_level, labelB, level, sublabel);
						float confidence = fabs(intA - intB);
						
                                                // Add the block to the label with the highest intersection.
						if ((intB > intA) && (confidence > req_confidence))
						{
							add_block(seeds_top_level, labelB, level, sublabel);
							done = true;
						}
						else
						{
							add_block(seeds_top_level, labelA, level, sublabel);
						}
					}
                                        // If there are more than 2 subblocks of label A.
					else if (nr_partitions[seeds_top_level][labelA] > 2)
					{
                                                // Check whether exchanging the chosen sublabel would split a superpixel,
                                                // then the resulting segmentation would be invalid.
						if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, true, true))
						{
							// As with only 2 partitions, delete block from label A
                                                        // and check the intersection with label A and B.
							delete_block(seeds_top_level, labelA, level, sublabel);
							float intA = intersection(seeds_top_level, labelA, level, sublabel);
							float intB = intersection(seeds_top_level, labelB, level, sublabel);
							float confidence = fabs(intA - intB);
							
                                                        // Assign to label with higher intersection.
							if ((intB > intA) && (confidence > req_confidence))
							{
								add_block(seeds_top_level, labelB, level, sublabel);
								done = true;
							}
							else
							{
								add_block(seeds_top_level, labelA, level, sublabel);
							}
						}
					}
				}

				if ((!done) && (nr_partitions[seeds_top_level][labelB] > MINIMUM_NR_SUBLABELS))
				{
					// try opposite direction
					sublabel = y*step+x+1;
                                        
                                        // As above, put the block to the label with higher intersection,
                                        // if there are more than 3 subblocks, check whether this would split
                                        // a superpixel first.
					if (nr_partitions[seeds_top_level][labelB] <= 2) // == 2
					{
						delete_block(seeds_top_level, labelB, level, sublabel);
						float intA = intersection(seeds_top_level, labelA, level, sublabel);
						float intB = intersection(seeds_top_level, labelB, level, sublabel);
						float confidence = fabs(intA - intB);
						if ((intA > intB) && (confidence > req_confidence))
						{
							add_block(seeds_top_level, labelA, level, sublabel);
							x++;
						}
						else
						{
							add_block(seeds_top_level, labelB, level, sublabel);
						}
					}
					else if (nr_partitions[seeds_top_level][labelB] > 2)
					{
						if (!check_split(a12, a13, a14, a22, a23, a24, a32, a33, a34, true, false))
						{
							delete_block(seeds_top_level, labelB, level, sublabel);
							float intA = intersection(seeds_top_level, labelA, level, sublabel);
							float intB = intersection(seeds_top_level, labelB, level, sublabel);
							float confidence = fabs(intA - intB);
							if ((intA > intB) && (confidence > req_confidence))
							{
								add_block(seeds_top_level, labelA, level, sublabel);
								x++;
							}
							else
							{
								add_block(seeds_top_level, labelB, level, sublabel);
							}
						}
					}
				}											
			}
		}
        
		update_labels(level);

	// Now, try vertical exchanging of the block.
        // This is mainly the same as with horizontal exchanging of blocks.
	for (int x=1; x<nr_w[level]-1; x++)
		for (int y=1; y<nr_h[level]-1; y++)
		{
			// choose a label at the current level
			sublabel = y*step+x; 
			// get the label at the top level (= superpixel label)
			labelA = parent[level][y*step+x]; 
			// get the neighboring label at the top level (= superpixel label)
			labelB = parent[level][(y+1)*step+x]; 

			if (labelA != labelB)
			{
				int a11 = parent[level][(y-1)*step+(x-1)];
				int a12 = parent[level][(y-1)*step+(x)];
				int a13 = parent[level][(y-1)*step+(x+1)];
				int a21 = parent[level][(y)*step+(x-1)];
				int a22 = parent[level][(y)*step+(x)];
				int a23 = parent[level][(y)*step+(x+1)];
				int a31 = parent[level][(y+1)*step+(x-1)];
				int a32 = parent[level][(y+1)*step+(x)];
				int a33 = parent[level][(y+1)*step+(x+1)];
				int a41 = parent[level][(y+2)*step+(x-1)];
				int a42 = parent[level][(y+2)*step+(x)];
				int a43 = parent[level][(y+2)*step+(x+1)];

				done = false;
				if (nr_partitions[seeds_top_level][labelA] > MINIMUM_NR_SUBLABELS)
				{
					if (nr_partitions[seeds_top_level][labelA] <= 2)
					{
						delete_block(seeds_top_level, labelA, level, sublabel);
						float intA = intersection(seeds_top_level, labelA, level, sublabel);
						float intB = intersection(seeds_top_level, labelB, level, sublabel);
						float confidence = fabs(intA - intB);
                                                
						if ((intB > intA) && (confidence > req_confidence))
						{
							add_block(seeds_top_level, labelB, level, sublabel);
							//y++;
							done = true;
						}
						else
						{
							add_block(seeds_top_level, labelA, level, sublabel);
						}
					}
					else if (nr_partitions[seeds_top_level][labelA] > 2) // 3 or more partitions
					{
						if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, false, true))
						{
							delete_block(seeds_top_level, labelA, level, sublabel);
							float intA = intersection(seeds_top_level, labelA, level, sublabel);
							float intB = intersection(seeds_top_level, labelB, level, sublabel);
							float confidence = fabs(intA - intB);
                                                        
							if ((intB > intA) && (confidence > req_confidence))
							{
								add_block(seeds_top_level, labelB, level, sublabel);
								//y++;
								done = true;
							}
							else
							{
								add_block(seeds_top_level, labelA, level, sublabel);
							}
						}
					}
				}

                                // Try the opposite side if their was no success, before.
				if ((!done) && (nr_partitions[seeds_top_level][labelB] > MINIMUM_NR_SUBLABELS))
				{
					sublabel = (y+1)*step+x;
					if (nr_partitions[seeds_top_level][labelB] <= 2) // == 2
					{
						delete_block(seeds_top_level, labelB, level, sublabel);
						float intA = intersection(seeds_top_level, labelA, level, sublabel);
						float intB = intersection(seeds_top_level, labelB, level, sublabel);
						float confidence = fabs(intA - intB);
                                                
						if ((intA > intB) && (confidence > req_confidence))
						{
							add_block(seeds_top_level, labelA, level, sublabel);
							y++;
						}
						else
						{
							add_block(seeds_top_level, labelB, level, sublabel);
						}
					}
					else if (nr_partitions[seeds_top_level][labelB] > 2)
					{
						if (!check_split(a21, a22, a23, a31, a32, a33, a41, a42, a43, false, false))
						{
							delete_block(seeds_top_level, labelB, level, sublabel);
							float intA = intersection(seeds_top_level, labelA, level, sublabel);
							float intB = intersection(seeds_top_level, labelB, level, sublabel);
							float confidence = fabs(intA - intB);
                                                        
							if ((intA > intB) && (confidence > req_confidence))
							{
								add_block(seeds_top_level, labelA, level, sublabel);
								y++;
							}
							else
							{
								add_block(seeds_top_level, labelB, level, sublabel);
							}
						}
					}
				}											
			}
		}
		update_labels(level);
}

/**
 * After performing block updates at the current level, the level is decreased.
 * This method updates the parent and nr_partitions array accordingly as used in
 * update_blocks.
 * 
 * @return 
 */
int SEEDS::go_down_one_level()
{
	int old_level = seeds_current_level;
	int new_level = seeds_current_level - 1;

	if (new_level < 0) return -1;

	// go through labels of top level
	for (int x=0; x<nr_w[seeds_top_level]; x++)
		for (int y=0; y<nr_h[seeds_top_level]; y++)
		{
			// reset nr_partitions
			nr_partitions[seeds_top_level][y*nr_w[seeds_top_level]+x] = 0;
		}

	// go through labels of new_level
	for (int x=0; x<nr_w[new_level]; x++)
		for (int y=0; y<nr_h[new_level]; y++)
		{
			// assign parent = parent of old_label
			int p = parent[old_level][parent[new_level][y*nr_w[new_level]+x]];
			parent[new_level][y*nr_w[new_level]+x] = p;
			// add nr_partitions[label] to parent
			nr_partitions[seeds_top_level][p] += nr_partitions[new_level][y*nr_w[new_level]+x];
		}

	return new_level;
}

/**
 * This method runs over all pixels and checks whether to move the pixel horizontally
 * or vertically. The decision of whether to move is done using a simple probability
 * computation which may incorporate a prior built by the local neighbourhood.
 * 
 * Update pixels based on the histograms.
 */
void SEEDS::update_pixels()
{
	int labelA, labelA1, labelA2, labelA3;
	int labelB, labelB1, labelB2, labelB3;
	int priorA;
	int priorB;
	float edgeA;
	float edgeB;

	if (forwardbackward)
	{
		forwardbackward = false;
		for (int y=1; y<height-1; y++)
			for (int x=1; x<width-1; x++) 
			{
                                // Get all labels in a three by four neighbourhood.
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a14 = labels[seeds_top_level][(y-1)*width+(x+2)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)]; 
				int a23 = labels[seeds_top_level][(y)*width+(x+1)];
				int a24 = labels[seeds_top_level][(y)*width+(x+2)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)];
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a34 = labels[seeds_top_level][(y+1)*width+(x+2)];

				// Label A is the current label, label B is the
                                // label to move the current pixel to.
				labelA = a22;
				labelB = a23;
				if (labelA != labelB)
				{
					if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, true, true))
					{
                                                // Incorporate a simple prior based on the
                                                // labels of the local neighbourhood of the pixel.
                                                if (prior) {
                                                        priorA = threebyfour(x,y,labelA);
                                                        priorB = threebyfour(x,y,labelB);
                                                }

						if (probability(image_bins[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
						{
							update(seeds_top_level, labelB, x, y);
						}
						else if (!check_split(a12, a13, a14, a22, a23, a24, a32, a33, a34, true, false))
						{
							if (probability(image_bins[y*width+x+1], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
							{
								update(seeds_top_level, labelA, x+1, y);
								x++;
							}
						}
					}
				}
			}

                // Same procedure for vertical movements of the current pixel.
		for (int x=1; x<width-1; x++)
			for (int y=1; y<height-1; y++)
			{
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)]; 
				int a23 = labels[seeds_top_level][(y)*width+(x+1)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)]; 
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a41 = labels[seeds_top_level][(y+2)*width+(x-1)];
				int a42 = labels[seeds_top_level][(y+2)*width+(x)];
				int a43 = labels[seeds_top_level][(y+2)*width+(x+1)];

				labelA = a22;
				labelB = a32;
				if (labelA != labelB)
				{
					if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, false, true))
					{
                                                if (prior) {
                                                        priorA = fourbythree(x,y,labelA);
                                                        priorB =  fourbythree(x,y,labelB);
                                                }

						if (probability(image_bins[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
						{
							update(seeds_top_level, labelB, x, y);
						} 
						else if (!check_split(a21, a22, a23, a31, a32, a33, a41, a42, a43, false, false))
						{
							if (probability(image_bins[(y+1)*width+x], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
							{
								update(seeds_top_level, labelA, x, y+1);
								y++;
							}
						}
					}
				}
			}
	}
	else
	{
		forwardbackward = true;
		for (int y=1; y<height-1; y++)
			for (int x=1; x<width-1; x++) 
			{
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a14 = labels[seeds_top_level][(y-1)*width+(x+2)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)]; 
				int a23 = labels[seeds_top_level][(y)*width+(x+1)];
				int a24 = labels[seeds_top_level][(y)*width+(x+2)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)];
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a34 = labels[seeds_top_level][(y+1)*width+(x+2)];

				// horizontal bidirectional
				labelA = a22;
				labelB = a23;
				if (labelA != labelB)
				{
					if (!check_split(a12, a13, a14, a22, a23, a24, a32, a33, a34, true, false))
					{
                                                if (prior) {
                                                        priorA = threebyfour(x,y,labelA);
                                                        priorB = threebyfour(x,y,labelB);
                                                }

						if (probability(image_bins[y*width+x+1], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
						{
							update(seeds_top_level, labelA, x+1, y);
							x++;
						}
						else if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, true, true))
						{
							if (probability(image_bins[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
							{
								update(seeds_top_level, labelB, x, y);
							}
						}
					}
				}
			}


		for (int x=1; x<width-1; x++)
			for (int y=1; y<height-1; y++)
			{
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)]; 
				int a23 = labels[seeds_top_level][(y)*width+(x+1)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)]; 
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a41 = labels[seeds_top_level][(y+2)*width+(x-1)];
				int a42 = labels[seeds_top_level][(y+2)*width+(x)];
				int a43 = labels[seeds_top_level][(y+2)*width+(x+1)];

				// vertical bidirectional
				labelA = a22;
				labelB = a32;
				if (labelA != labelB)
				{
					if (!check_split(a21, a22, a23, a31, a32, a33, a41, a42, a43, false, false))
					{
						if (prior) {					
                                                        priorA = fourbythree(x,y,labelA);
                                                        priorB =  fourbythree(x,y,labelB);
						}

						if (probability(image_bins[(y+1)*width+x], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
						{
							update(seeds_top_level, labelA, x, y+1);
							y++;
						}
						else if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, false, true))
						{
							if (probability(image_bins[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
							{
								update(seeds_top_level, labelB, x, y);
							}
						}
					}
				}
			}

	}

	// Update the border pixels, here we do not have to check the entire
        // neighbourhood, instead just check right and left or above and below.
	for (int x=0; x<width; x++)
	{
		labelA = labels[seeds_top_level][x];
		labelB = labels[seeds_top_level][width+x];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, x, 0);	
		}
		labelA = labels[seeds_top_level][(height-1)*width + x];
		labelB = labels[seeds_top_level][(height-2)*width + x];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, x, height-1);			
		}
	}
	for (int y=0; y<height; y++)
	{
		labelA = labels[seeds_top_level][y*width];
		labelB = labels[seeds_top_level][y*width+1];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, 0, y);	
		}
		labelA = labels[seeds_top_level][y*width + width - 1];
		labelB = labels[seeds_top_level][y*width + width - 2];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, width-1, y);			
		}
	}
}

/**
 * Update pixels mean-based as described in [2] similar to SLIC.
 */
void SEEDS::update_pixels_means()
{
	int labelA, labelA1, labelA2, labelA3;
	int labelB, labelB1, labelB2, labelB3;
	int priorA;
	int priorB;
	float edgeA;
	float edgeB;

	if (forwardbackward)
	{
		forwardbackward = false;
		for (int y=1; y<height-1; y++)
			for (int x=1; x<width-1; x++) 
			{
                                // 3 by 4 neighbourhood of current pixel.
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a14 = labels[seeds_top_level][(y-1)*width+(x+2)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)];
				int a23 = labels[seeds_top_level][(y)*width+(x+1)]; 
				int a24 = labels[seeds_top_level][(y)*width+(x+2)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)];
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a34 = labels[seeds_top_level][(y+1)*width+(x+2)];

				// a22 is the label of the current pixel,
                                // a23 is the label of the neighbouring superpixel.
				labelA = a22;
				labelB = a23;
				if (labelA != labelB)
				{
                                        // Only exchange pixel to neighbouring superpixel if the superpixel is
                                        // not going to be split up.
					if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, true, true))
					{
                                                // Compute a simple prior whether the current pixel
                                                // should be exchanged or not.
                                                if (prior) {
                                                    priorA = threebyfour(x,y,labelA); // Label A is the current label of pixel (x,y).
                                                    priorB = threebyfour(x,y,labelB);
						}

                                                // Check whether it is probable that the current pixel belongs to label B.
						if (probability_means(image_l[y*width+x], image_a[y*width+x], image_b[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
						{
							update(seeds_top_level, labelB, x, y);
						}
                                                // At the same point check the next pixel given by x+1 whether to move, as well.
						else if (!check_split(a12, a13, a14, a22, a23, a24, a32, a33, a34, true, false))
						{
							if (probability_means(image_l[y*width+x+1], image_a[y*width+x+1], image_b[y*width+x+1], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
							{
								update(seeds_top_level, labelA, x+1, y);
								x++;
							}
						}
					}
				}
			}

                // Same procedure for vertical moving of pixels.
		for (int x=1; x<width-1; x++)
			for (int y=1; y<height-1; y++)
			{
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)]; 
				int a23 = labels[seeds_top_level][(y)*width+(x+1)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)]; 
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a41 = labels[seeds_top_level][(y+2)*width+(x-1)];
				int a42 = labels[seeds_top_level][(y+2)*width+(x)];
				int a43 = labels[seeds_top_level][(y+2)*width+(x+1)];

				// Label A is the current label of the pixel,
                                // label B the label to move the pixel to.
				labelA = a22;
				labelB = a32;
                                
                                // The approach is the same as with the horizontal exchanging of pixels - 
                                // use the probability estimate to check whether to move the pixel.
				if (labelA != labelB)
				{
					if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, false, true))
					{
                                                if (prior) {
                                                        priorA = fourbythree(x,y,labelA);
                                                        priorB =  fourbythree(x,y,labelB);
                                                }

						if (probability_means(image_l[y*width+x], image_a[y*width+x], image_b[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
						{
							update(seeds_top_level, labelB, x, y);
						} 
						else if (!check_split(a21, a22, a23, a31, a32, a33, a41, a42, a43, false, false))
						{
							if (probability_means(image_l[(y+1)*width+x], image_a[(y+1)*width+x], image_b[(y+1)*width+x], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
							{
								update(seeds_top_level, labelA, x, y+1);
								y++;
							}
						}
					}
				}
			}
	}
	else
	{
		forwardbackward = true;
		for (int y=1; y<height-1; y++)
			for (int x=1; x<width-1; x++) 
			{
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a14 = labels[seeds_top_level][(y-1)*width+(x+2)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)]; 
				int a23 = labels[seeds_top_level][(y)*width+(x+1)]; 
				int a24 = labels[seeds_top_level][(y)*width+(x+2)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)];
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a34 = labels[seeds_top_level][(y+1)*width+(x+2)];

				// horizontal bidirectional
				labelA = a22;
				labelB = a23;
				if (labelA != labelB)
				{
					if (!check_split(a12, a13, a14, a22, a23, a24, a32, a33, a34, true, false))
					{
                                                if (prior) {
                                                        priorA = threebyfour(x,y,labelA);
                                                        priorB = threebyfour(x,y,labelB);
                                                }

						if (probability_means(image_l[y*width+x+1], image_a[y*width+x+1], image_b[y*width+x+1], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
						{
							update(seeds_top_level, labelA, x+1, y);
							x++;
						}
						else if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, true, true))
						{
							if (probability_means(image_l[y*width+x], image_a[y*width+x], image_b[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
							{
								update(seeds_top_level, labelB, x, y);
							}
						}
					}
				}
			}


		for (int x=1; x<width-1; x++)
			for (int y=1; y<height-1; y++)
			{
				int a11 = labels[seeds_top_level][(y-1)*width+(x-1)];
				int a12 = labels[seeds_top_level][(y-1)*width+(x)];
				int a13 = labels[seeds_top_level][(y-1)*width+(x+1)];
				int a21 = labels[seeds_top_level][(y)*width+(x-1)];
				int a22 = labels[seeds_top_level][(y)*width+(x)]; 
				int a23 = labels[seeds_top_level][(y)*width+(x+1)];
				int a31 = labels[seeds_top_level][(y+1)*width+(x-1)];
				int a32 = labels[seeds_top_level][(y+1)*width+(x)]; 
				int a33 = labels[seeds_top_level][(y+1)*width+(x+1)];
				int a41 = labels[seeds_top_level][(y+2)*width+(x-1)];
				int a42 = labels[seeds_top_level][(y+2)*width+(x)];
				int a43 = labels[seeds_top_level][(y+2)*width+(x+1)];

				// vertical bidirectional
				labelA = a22;
				labelB = a32;
				if (labelA != labelB)
				{
					if (!check_split(a21, a22, a23, a31, a32, a33, a41, a42, a43, false, false))
					{
                                                if (prior) {
                                                        priorA = fourbythree(x,y,labelA);
                                                        priorB =  fourbythree(x,y,labelB);
						}

						if (probability_means(image_l[(y+1)*width+x], image_a[(y+1)*width+x], image_b[(y+1)*width+x], labelB, labelA, priorB, priorA, edgeB, edgeA)) 
						{
							update(seeds_top_level, labelA, x, y+1);
							y++;
						}
						else if (!check_split(a11, a12, a13, a21, a22, a23, a31, a32, a33, false, true))
						{
							if (probability_means(image_l[y*width+x], image_a[y*width+x], image_b[y*width+x], labelA, labelB, priorA, priorB, edgeA, edgeB)) 
							{
								update(seeds_top_level, labelB, x, y);
							}
						}
					}
				}
			}

	}

	// For border pixels we have not to check the entire neighbourhood,
        // simply check above and below or left and right.
	for (int x=0; x<width; x++)
	{
		labelA = labels[seeds_top_level][x];
		labelB = labels[seeds_top_level][width+x];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, x, 0);	
		}
		labelA = labels[seeds_top_level][(height-1)*width + x];
		labelB = labels[seeds_top_level][(height-2)*width + x];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, x, height-1);			
		}
	}
	for (int y=0; y<height; y++)
	{
		labelA = labels[seeds_top_level][y*width];
		labelB = labels[seeds_top_level][y*width+1];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, 0, y);	
		}
		labelA = labels[seeds_top_level][y*width + width - 1];
		labelB = labels[seeds_top_level][y*width + width - 2];
		if (labelA != labelB)
		{
				update(seeds_top_level, labelB, width-1, y);			
		}
	}
}

/**
 * Updates the label of a single pixel at the given level.
 * 
 * @param level
 * @param label_new
 * @param x
 * @param y
 */
void SEEDS::update(int level, int label_new, int x, int y)
{
	int label_old = labels[level][y*width+x];
    delete_pixel_m(level, label_old, x, y);
    add_pixel_m(level, label_new, x, y);
	labels[level][y*width+x] = label_new;
}

/**
 * Add a pixel at the given label in the given level.
 * This automatically updates the histograms.
 * 
 * @param level
 * @param label
 * @param x
 * @param y
 */
void SEEDS::add_pixel(int level, int label, int x, int y)
{
	histogram[level][label][image_bins[y*width+x]]++;
	T[level][label]++;
}

/**
 * Add a pixel to the given label in the given level and remember to update both
 * the histograms and the means.
 * 
 * @param level
 * @param label
 * @param x
 * @param y
 */
void SEEDS::add_pixel_m(int level, int label, int x, int y)
{
	histogram[level][label][image_bins[y*width+x]]++;
	T[level][label]++;

	if (means) {
		L_channel[label] += image_l[y*width + x];
		A_channel[label] += image_a[y*width + x];
		B_channel[label] += image_b[y*width + x];
        }
}

/**
 * Delete a pixel from the given label in the given level.
 * Update the histograms.
 * 
 * @param level
 * @param label
 * @param x
 * @param y
 */
void SEEDS::delete_pixel(int level, int label, int x, int y)
{
	histogram[level][label][image_bins[y*width+x]]--;
	T[level][label]--;
}

/**
 * Same as delete_pixel but remember to update the means, as well.
 * @param levelz
 * @param label
 * @param x
 * @param y
 */
void SEEDS::delete_pixel_m(int level, int label, int x, int y)
{
	histogram[level][label][image_bins[y*width+x]]--;
	T[level][label]--;
	
	if (means) {
		L_channel[label] -= image_l[y*width + x];
		A_channel[label] -= image_a[y*width + x];
		B_channel[label] -= image_b[y*width + x];
        }
}

/**
 * Adds the block given by sublabel at level sublevel to the label given by label 
 * at the given level.
 * 
 * @param level
 * @param label
 * @param sublevel
 * @param sublabel
 */
void SEEDS::add_block(int level, int label, int sublevel, int sublabel)
{
	parent[sublevel][sublabel] = label;

	for (int n=0; n<histogram_size; n++)
	{
		histogram[level][label][n] += histogram[sublevel][sublabel][n];
	}
	T[level][label] += T[sublevel][sublabel];

	nr_partitions[level][label]++;
}

/**
 * Delete the block at the given level sublevel from label sublabel and add it
 * to the label given by label and the given level.
 * 
 * @param level
 * @param label
 * @param sublevel
 * @param sublabel
 */
void SEEDS::delete_block(int level, int label, int sublevel, int sublabel)
{
	parent[sublevel][sublabel] = -1;

	for (int n=0; n<histogram_size; n++)
	{
		histogram[level][label][n] -= histogram[sublevel][sublabel][n];
	}
	T[level][label] -= T[sublevel][sublabel];

	nr_partitions[level][label]--;
}

/**
 * Moving blocks using add_block and delete_block only affects the parent array.
 * In a subsequent step, the labels in the labels array need to be updated.
 * 
 * @param level
 */
void SEEDS::update_labels(int level)
{
	for (int i=0; i<width*height; i++)
	{
		labels[seeds_top_level][i] = parent[level][labels[level][i]];
	}
}

/**
 * Computes a probability of whether the pixel with label1 should be moved to label
 * label2 given the prior which is computed using the immediate neighbourhood of the pixel.
 * 
 * Returns a boolean indicating whether to move or not to move.
 * 
 * @param color
 * @param label1
 * @param label2
 * @param prior1
 * @param prior2
 * @param edge1
 * @param edge2
 * @return 
 */
bool SEEDS::probability(int color, int label1, int label2, int prior1, int prior2, float edge1, float edge2)
{
        // T saves the number of pixels for each block/superpixel at each level and 
        // can therefore be used for normalization.
	float P_label1 = (float)histogram[seeds_top_level][label1][color] / (float)T[seeds_top_level][label1];
	float P_label2 = (float)histogram[seeds_top_level][label2][color] / (float)T[seeds_top_level][label2];

	if (prior) {
		P_label1 *= (float) prior1;
		P_label2 *= (float) prior2;
        }
        else {
		P_label1 = (float)histogram[seeds_top_level][label1][color] / (float)T[seeds_top_level][label1];
		P_label2 = (float)histogram[seeds_top_level][label2][color] / (float)T[seeds_top_level][label2];
	}

	return (P_label2 > P_label1);
}

/**
 * In update_pixels_mean, the decision wether to exchange a pixel is based on means
 * rather than histograms.
 * 
 * @param L
 * @param a
 * @param b
 * @param label1
 * @param label2
 * @param prior1
 * @param prior2
 * @param edge1
 * @param edge2
 * @return 
 */
bool SEEDS::probability_means(float L, float a, float b, int label1, int label2, int prior1, int prior2, float edge1, float edge2)
{
        if (means) {
		float L1 = L_channel[label1] / T[seeds_top_level][label1];
		float a1 = A_channel[label1] / T[seeds_top_level][label1];
		float b1 = B_channel[label1] / T[seeds_top_level][label1];
		float L2 = L_channel[label2] / T[seeds_top_level][label2];
		float a2 = A_channel[label2] / T[seeds_top_level][label2];
		float b2 = B_channel[label2] / T[seeds_top_level][label2];

		float P_label1 = (L-L1)*(L-L1) + (a-a1)*(a-a1) + (b-b1)*(b-b1);
		float P_label2 = (L-L2)*(L-L2) + (a-a2)*(a-a2) + (b-b2)*(b-b2);

		if (prior) {
			P_label1 /= prior1;
			P_label2 /= prior2;
		}

		return (P_label1 > P_label2);
        }

	return false;
}

void SEEDS::LAB2RGB(float L, float a, float b, int* R, int* G, int* B)
{
	float T1 = 0.008856;
	float T2 = 0.206893;

	float fY = pow((L + 16.0) / 116.0, 3);
	bool YT = fY > T1;
	fY = (!YT) * (L / 903.3) + YT * fY;
	float Y = fY;

	// Alter fY slightly for further calculations
	fY = YT * pow(fY,1.0/3.0) + (!YT) * (7.787 * fY + 16.0/116.0);

	float fX = a / 500.0 + fY;
	bool XT = fX > T2;
	float X = (XT * pow(fX, 3) + (!XT) * ((fX - 16.0/116.0) / 7.787));

	float fZ = fY - b / 200.0;
	bool ZT = fZ > T2;
	float Z = (ZT * pow(fZ, 3) + (!ZT) * ((fZ - 16.0/116.0) / 7.787));

	X = X * 0.950456 * 255.0;
	Y = Y * 255.0;
	Z = Z * 1.088754 * 255.0;

	*R = (int) (3.240479*X - 1.537150*Y - 0.498535*Z);
	*G = (int) (-0.969256*X + 1.875992*Y + 0.041556*Z);
	*B = (int) (0.055648*X - 0.204043*Y + 1.057311*Z); 
}

int SEEDS::RGB2LAB(const int& r, const int& g, const int& b, float* lval, float* aval, float* bval)
{
	float xVal = 0.412453 * r + 0.357580 * g + 0.180423 * b;
	float yVal = 0.212671 * r + 0.715160 * g + 0.072169 * b;
	float zVal = 0.019334 * r + 0.119193 * g + 0.950227 * b;

	xVal /= (255.0 * 0.950456);
	yVal /=  255.0;
	zVal /= (255.0 * 1.088754);

	float fY, fZ, fX;
	float lVal, aVal, bVal;
	float T = 0.008856;

	bool XT = (xVal > T);
	bool YT = (yVal > T);
	bool ZT = (zVal > T);

	fX = XT * pow(xVal,1.0/3.0) + (!XT) * (7.787 * xVal + 16.0/116.0);

	// Compute L
	float Y3 = pow(yVal,1.0/3.0); 
	fY = YT*Y3 + (!YT)*(7.787*yVal + 16.0/116.0);
	lVal  = YT * (116 * Y3 - 16.0) + (!YT)*(903.3*yVal);

	fZ = ZT*pow(zVal,1.0/3.0) + (!ZT)*(7.787*zVal + 16.0/116.0);

	// Compute a and b
	aVal = 500 * (fX - fY);
	bVal = 200 * (fY - fZ);

	*lval = lVal;
	*aval = aVal;
	*bval = bVal;

	return 1; //LAB2bin(lVal, aVal, bVal);
}

int SEEDS::RGB2HSV(const int& r, const int& g, const int& b, float* hval, float* sval, float* vval)
{
	float r_ = r / 256.0;
	float g_ = g / 256.0;
	float b_ = b / 256.0;

    float min_rgb = min(r_, min(g_, b_));
    float max_rgb = max(r_, max(g_, b_));
    float V = max_rgb;
	float H = 0.0;
	float S = 0.0;

    float delta = max_rgb - min_rgb;
	
	if ((delta > 0.0) && (max_rgb > 0.0))
	{
		S = delta / max_rgb;
	    if (max_rgb == r_)
	        H = (g_ - b_) / delta;
    	else if (max_rgb == g_)
	        H = 2 + (b_ - r_) / delta;
	    else
	        H = 4 + (r_ - g_) / delta;
	}

    H /= 6;

	/*float V = max(r_,max(g_,b_));
	float S = 0.0;
	if (V != 0.0) S = (V - min(r_, min(g_,b_)))/V;
	float H = 0.0;
	if (S != 0.0)
	{
		if (V == r_)
			H = ((g_ - b_)/6.0)/S;
		else if (V == g_)
			H = 1.0/2.0 + ((b_-r_)/6.0)/S;
		else
			H = 2.0/3.0 + ((r_-g_)/6.0)/S;
	}*/

	if (H<0.0) H += 1.0;

	*hval = H;
	*sval = S;
	*vval = V;

	if ((H<0.0) || (H>= 1.0)) printf("H: %f \n", H);

	int hbin = floor(H * nr_bins);
	int sbin = floor(S * nr_bins);
	int vbin = floor(V * nr_bins);

	//printf("%d %d %d -- %f %f %f -- bins %d %d %d\n", r, g, b, H, S, V, hbin, sbin, vbin);

	return hbin + nr_bins*sbin + nr_bins*nr_bins*vbin;
}

int SEEDS::LAB2bin(float l, float a, float b)
{
	// binning
	int bin_l = floor(l/100.1*nr_bins);

	int bin_a = floor(a/100*(nr_bins-2) + (nr_bins-2)/2 + 1);
	if (bin_a < 0) bin_a = 0;
	if (bin_a >= nr_bins) bin_a = nr_bins-1;

	int bin_b = floor(b/100*(nr_bins-2) + (nr_bins-2)/2 + 1);
	if (bin_b < 0) bin_b = 0;
	if (bin_b >= nr_bins) bin_b = nr_bins-1;

	// encoding
	return bin_l + nr_bins*bin_a + nr_bins*nr_bins*bin_b;
}

int SEEDS::RGB2LAB_special(int r, int g, int b, float* lval, float* aval, float* bval)
{
	float xVal = 0.412453 * r + 0.357580 * g + 0.180423 * b;
	float yVal = 0.212671 * r + 0.715160 * g + 0.072169 * b;
	float zVal = 0.019334 * r + 0.119193 * g + 0.950227 * b;

	xVal /= (255.0 * 0.950456);
	yVal /=  255.0;
	zVal /= (255.0 * 1.088754);

	float fY, fZ, fX;
	float lVal, aVal, bVal;
	float T = 0.008856;

	bool XT = (xVal > T);
	bool YT = (yVal > T);
	bool ZT = (zVal > T);

	fX = XT * pow(xVal,1.0/3.0) + (!XT) * (7.787 * xVal + 16/116);

	// Compute L
	float Y3 = pow(yVal,1.0/3.0); 
	fY = YT*Y3 + (!YT)*(7.787*yVal + 16/116);
	lVal  = YT * (116 * Y3 - 16.0) + (!YT)*(903.3*yVal);

	fZ = ZT*pow(zVal,1.0/3.0) + (!ZT)*(7.787*zVal + 16/116);

	// Compute a and b
	aVal = 500 * (fX - fY);
	bVal = 200 * (fY - fZ);

	*lval = lVal;
	*aval = aVal;
	*bval = bVal;

	int bin1 = 0;
	int bin2 = 0;
	int bin3 = 0;

	while (lVal > bin_cutoff1[bin1]) {
		bin1++;
	}
	while (aVal > bin_cutoff2[bin2]) {
		bin2++;
	}
	while (bVal > bin_cutoff3[bin3]) {
		bin3++;
	}

	return bin1 + nr_bins*bin2 + nr_bins*nr_bins*bin3;
}

int SEEDS::RGB_special(int r, int g, int b, float* lval, float* aval, float* bval)
{
	int bin1 = 0;
	int bin2 = 0;
	int bin3 = 0;

	while (r > bin_cutoff1[bin1]) {
		bin1++;
	}
	while (g > bin_cutoff2[bin2]) {
		bin2++;
	}
	while (b > bin_cutoff3[bin3]) {
		bin3++;
	}

	return bin1 + nr_bins*bin2 + nr_bins*nr_bins*bin3;
}

int SEEDS::RGB2LAB_special(int r, int g, int b, int* bin_l, int* bin_a, int* bin_b)
{
	float xVal = 0.412453 * r + 0.357580 * g + 0.180423 * b;
	float yVal = 0.212671 * r + 0.715160 * g + 0.072169 * b;
	float zVal = 0.019334 * r + 0.119193 * g + 0.950227 * b;

	xVal /= (255.0 * 0.950456);
	yVal /=  255.0;
	zVal /= (255.0 * 1.088754);

	float fY, fZ, fX;
	float lVal, aVal, bVal;
	float T = 0.008856;

	bool XT = (xVal > T);
	bool YT = (yVal > T);
	bool ZT = (zVal > T);

	fX = XT * pow(xVal,1.0/3.0) + (!XT) * (7.787 * xVal + 16/116);

	// Compute L
	float Y3 = pow(yVal,1.0/3.0); 
	fY = YT*Y3 + (!YT)*(7.787*yVal + 16/116);
	lVal  = YT * (116 * Y3 - 16.0) + (!YT)*(903.3*yVal);

	fZ = ZT*pow(zVal,1.0/3.0) + (!ZT)*(7.787*zVal + 16/116);

	// Compute a and b
	aVal = 500 * (fX - fY);

	bVal = 200 * (fY - fZ);

	//*lval = lVal;
	//*aval = aVal;
	//*bval = bVal;

	int bin1 = 0;
	int bin2 = 0;
	int bin3 = 0;

	while (lVal > bin_cutoff1[bin1]) {
		bin1++;
	}
	while (aVal > bin_cutoff2[bin2]) {
		bin2++;
	}
	while (bVal > bin_cutoff3[bin3]) {
		bin3++;
	}

	*bin_l = bin1;
	*bin_a = bin2;
	*bin_b = bin3;

	return bin1 + nr_bins*bin2 + nr_bins*nr_bins*bin3;
}

/**
 * Count the number of pixels with the same label in a three bz three
 * neighbourhood around the given pixel.
 * 
 * This is then used to compute a prior whether the pixel should be
 * moved to a neighbouring superpixel.
 * 
 * @param x
 * @param y
 * @param label
 * @return 
 */
int SEEDS::threebythree(int x, int y, int label)
{
	int count = 0;
	if (labels[seeds_top_level][(y-1)*width+x-1]==label) count++;
	if (labels[seeds_top_level][(y-1)*width+x]==label) count++;
	if (labels[seeds_top_level][(y-1)*width+x+1]==label) count++;

	if (labels[seeds_top_level][y*width+x-1]==label) count++;
	if (labels[seeds_top_level][y*width+x+1]==label) count++;

	if (labels[seeds_top_level][(y+1)*width+x-1]==label) count++;
	if (labels[seeds_top_level][(y+1)*width+x]==label) count++;
	if (labels[seeds_top_level][(y+1)*width+x+1]==label) count++;

	return count;
}

/**
 * Count the number of same labels as the given pixel in a three by four neighbourhood.
 * 
 * @param x
 * @param y
 * @param label
 * @return 
 */
int SEEDS::threebyfour(int x, int y, int label)
{
	int count = 0;
	if (labels[seeds_top_level][(y-1)*width+x-1]==label) count++;
	if (labels[seeds_top_level][(y-1)*width+x]==label) count++;
	if (labels[seeds_top_level][(y-1)*width+x+1]==label) count++;
	if (labels[seeds_top_level][(y-1)*width+x+2]==label) count++;

	if (labels[seeds_top_level][y*width+x-1]==label) count++;
	if (labels[seeds_top_level][y*width+x+2]==label) count++;

	if (labels[seeds_top_level][(y+1)*width+x-1]==label) count++;
	if (labels[seeds_top_level][(y+1)*width+x]==label) count++;
	if (labels[seeds_top_level][(y+1)*width+x+1]==label) count++;
	if (labels[seeds_top_level][(y+1)*width+x+2]==label) count++;

	return count;
}

/**
 * Count the number of same labels as the given pixel in a four by three neighbourhood.
 * 
 * @param x
 * @param y
 * @param label
 * @return 
 */
int SEEDS::fourbythree(int x, int y, int label)
{
	int count = 0;
	if (labels[seeds_top_level][(y-1)*width+x-1]==label) count++;
	if (labels[seeds_top_level][(y-1)*width+x]==label) count++;
	if (labels[seeds_top_level][(y-1)*width+x+1]==label) count++;

	if (labels[seeds_top_level][y*width+x-1]==label) count++;
	if (labels[seeds_top_level][y*width+x+2]==label) count++;

	if (labels[seeds_top_level][(y+1)*width+x-1]==label) count++;
	if (labels[seeds_top_level][(y+1)*width+x+2]==label) count++;

	if (labels[seeds_top_level][(y+2)*width+x-1]==label) count++;
	if (labels[seeds_top_level][(y+2)*width+x]==label) count++;
	if (labels[seeds_top_level][(y+2)*width+x+1]==label) count++;

	return count;
}

float SEEDS::geometric_distance(int label1, int label2)
{
	float dx = ((float)x_position[label1]/T[seeds_top_level][label1] - (float)x_position[label2]/T[seeds_top_level][label2]);
	float dy = ((float)y_position[label1]/T[seeds_top_level][label1] - (float)y_position[label2]/T[seeds_top_level][label2]);
	return sqrt(dx*dx + dy*dy);
}

/**
 * Compute the intersection distance between the histograms of label1 and label2
 * at level1 and level2.
 * 
 * @param level1
 * @param label1
 * @param level2
 * @param label2
 * @return 
 */
float SEEDS::intersection(int level1, int label1, int level2, int label2)
{
    float intersect = 0.0;
	
	for (int n=0; n<histogram_size; n++)
	{
		intersect += min((float)histogram[level1][label1][n]/T[level1][label1], (float)histogram[level2][label2][n]/T[level2][label2]);
	}

	return intersect;
}

/**
 * Check whether moving a given block or pixel would result in a superpixel beign splittet. 
 * 
 * @param a11
 * @param a12
 * @param a13
 * @param a21
 * @param a22
 * @param a23
 * @param a31
 * @param a32
 * @param a33
 * @param horizontal
 * @param forward
 * @return 
 */
bool SEEDS::check_split(int a11, int a12, int a13, int a21, int a22, int a23, int a31, int a32, int a33, bool horizontal, bool forward)
{
        // We consider the 3 x 3 neighborhood of the current block with label l22:
        // ------------- ^
        // |a11|a12|a13| |
        // ------------- |
        // |a21|a22|a23| | vertical
        // ------------- |
        // |a31|a32|a33| |
        // ------------- v
        // <----------->
        //   horizontal
    
	if (horizontal)
	{
		if (forward)
		{
			if ((a22 != a21) && (a22 == a12) && (a22 == a32)) return true;
			if ((a22 != a11) && (a22 == a12) && (a22 == a21)) return true; 
			if ((a22 != a31) && (a22 == a32) && (a22 == a21)) return true;
		}
		else /*backward*/
		{
			if ((a22 != a23) && (a22 == a12) && (a22 == a32)) return true;
			if ((a22 != a13) && (a22 == a12) && (a22 == a23)) return true; 
			if ((a22 != a33) && (a22 == a32) && (a22 == a23)) return true;
		}
	}
	else /*vertical*/
	{
		if (forward)
		{
			if ((a22 != a12) && (a22 == a21) && (a22 == a23)) return true;
			if ((a22 != a11) && (a22 == a21) && (a22 == a12)) return true; 
			if ((a22 != a13) && (a22 == a23) && (a22 == a12)) return true;
		}
		else /*backward*/
		{
			if ((a22 != a32) && (a22 == a21) && (a22 == a23)) return true;
			if ((a22 != a31) && (a22 == a21) && (a22 == a32)) return true; 
			if ((a22 != a33) && (a22 == a23) && (a22 == a32)) return true;
		}
	}

	return false;
}

int SEEDS::count_superpixels()
{
	int* count_labels = new int[nr_labels[seeds_top_level]];
	for (int i=0; i<nr_labels[seeds_top_level]; i++)
		count_labels[i] = 0;
	for (int i=0; i<width*height; i++)
		count_labels[labels[seeds_top_level][i]] = 1;
	int count = 0;
	for (int i=0; i<nr_labels[seeds_top_level]; i++)
		count += count_labels[i];

	delete count_labels;
	return count;
}

void SEEDS::SaveLabels_Text(string filename) 
{
	int sz = width*height;

	ofstream outfile;
	outfile.open(filename.c_str());
        int i = 0;
        for( int h = 0; h < height; h++ )
          {
            for( int w = 0; w < width -1; w++ )
              {
                outfile << labels[seeds_top_level][i] << " ";
                i++;
              }
            outfile << labels[seeds_top_level][i] << endl;
            i++;
          }
	outfile.close();
}

void SEEDS::SaveLabels_CSV(string filename) 
{
	int sz = width*height;

	ofstream outfile;
	outfile.open(filename.c_str());
        int i = 0;
        for( int h = 0; h < height; h++ )
          {
            for( int w = 0; w < width -1; w++ )
              {
                outfile << labels[seeds_top_level][i] << ",";
                i++;
              }
            outfile << labels[seeds_top_level][i] << endl;
            i++;
          }
	outfile.close();
}

//void SEEDS::compute_mean_map()
//{
//	means = new UINT[width*height];
//
//	for (int i=0; i<width*height; i++)
//	{
//		int label = labels[seeds_top_level][i];
//		float L = 100.0 * ((float) L_channel[label]) / T[seeds_top_level][label];
//		float a = 255.0 * ((float) A_channel[label]) / T[seeds_top_level][label] - 128.0;
//		float b = 255.0 * ((float) B_channel[label]) / T[seeds_top_level][label] - 128.0;
//		int R, G, B;
//		LAB2RGB(L, a, b, &R, &G, &B);
//		means[i] = B | (G << 8) | (R << 16);
//	}
//}












