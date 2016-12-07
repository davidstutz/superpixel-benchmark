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

#if !defined(_SEEDS_H_INCLUDED_)
#define _SEEDS_H_INCLUDED_

#include <string>
#include <opencv2/opencv.hpp>

using namespace std;

typedef unsigned int UINT;


class SEEDS  
{
public:
	SEEDS(int width, int height, int nr_channels, int nr_bins, int min_size, float confidence, bool prior, bool means, int color);
	~SEEDS();

	// initialize with an image
	void initialize(UINT* image, int seeds_w, int seeds_h, int nr_levels);
	void initialize(const cv::Mat &image, int seeds_w, int seeds_h, int nr_levels);
        
	// go through iterations
	void iterate(int iterations);

	// output labels
	UINT** labels;	 

	// evaluation
	int count_superpixels();
	void SaveLabels_Text(string filename);
        void SaveLabels_CSV(string filename);
        
	// mean colors
//	void compute_mean_map();
//	UINT* means;

private:

	int current_nr_superpixels;
	bool initialized;

	// seeds	
	int seeds_w;
	int seeds_h;
	int seeds_nr_levels;
	int seeds_top_level;
	int seeds_current_level;

   	// image processing
	float* image_l;
	float* image_a;
	float* image_b;
	UINT* image_bins;
	float* edge_h;
	float* edge_w;
	float* bin_cutoff1;
	float* bin_cutoff2;
	float* bin_cutoff3;

	// keep one labeling for each level
	UINT* nr_labels;
	UINT** parent;
	UINT** nr_partitions;
	int** T;
	int go_down_one_level();

	// initialization
	void assign_labels();
	void compute_histograms(int until_level = -1);
	void compute_histograms_ex();
	void compute_means();
	void compute_edges();
	void lab_get_histogram_cutoff_values(UINT* image);
	void lab_get_histogram_cutoff_values(const cv::Mat &image);
        void rgb_get_histogram_cutoff_values(UINT* image);
	void rgb_get_histogram_cutoff_values(const cv::Mat &image);
        
	// color conversion and histograms
	int RGB2HSV(const int& r, const int& g, const int& b, float* hval, float* sval, float* vval);
	int RGB2LAB(const int& r, const int& g, const int& b, float* lval, float* aval, float* bval);
	int LAB2bin(float l, float a, float b);
	int RGB_special(int r, int g, int b, float* lval, float* aval, float* bval);
	int RGB2LAB_special(int r, int g, int b, float* lval, float* aval, float* bval);
	int RGB2LAB_special(int r, int g, int b, int* bin_l, int* bin_a, int* bin_b);
	void LAB2RGB(float L, float a, float b, int* R, int* G, int* B);

	int histogram_size;
	int*** histogram;
	//int** subhistogram;
	

    void update(int level, int label_new, int x, int y);
	void add_pixel(int level, int label, int x, int y);
	void add_pixel_m(int level, int label, int x, int y);
	void delete_pixel(int level, int label, int x, int y);
	void delete_pixel_m(int level, int label, int x, int y);
	void add_block(int level, int label, int sublevel, int sublabel);
	void delete_block(int level, int label, int sublevel, int sublabel);
	void update_labels(int level);


	// probability computation
	bool probability(int color, int label1, int label2, int prior1, int prior2, float edge1, float edge2);
	bool probability_means(float L, float a, float b, int label1, int label2, int prior1, int prior2, float edge1, float edge2);

	int threebythree(int x, int y, int label);
	int threebyfour(int x, int y, int label);
	int fourbythree(int x, int y, int lacompute_mean_mapbel);


	// block updating
	void update_blocks(int level, float req_confidence = 0.0);
	float merge_threshold;
	float intersection(int level1, int label1, int level2, int label2);
	float geometric_distance(int label1, int label2);
	int min_size;

	// border updating
	void update_pixels();
	void update_pixels_means();
	bool forwardbackward;
	int threebythree_upperbound;
	int threebythree_lowerbound;

	bool check_split(int a11, int a12, int a13, int a21, int a22, int a23, int a31, int a32, int a33, bool horizontal, bool forward);

	int nr_comp;
	float step_h;
	float step_w;
	float substep_h;
	float substep_w;
	int nr_label_h;
	int nr_label_w;
	int* nr_w;
	int* nr_h;

	int* x_position;
	int* y_position;
	float* L_channel;
	float* A_channel;
	float* B_channel;
	float* L_subchannel;
	float* A_subchannel;
	float* B_subchannel;
	int width, height, nr_channels, nr_bins;
        float confidence;
        bool prior;
        bool means;
        int color;
        
	int iteration;
	int step;
};




#endif // !defined(_SEEDS_H_INCLUDED_)
