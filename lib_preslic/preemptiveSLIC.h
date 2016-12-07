/*
 * PreemptiveSLIC
 * Copyright (C) 2014  Peer Neubert, peer.neubert@etit.tu-chemnitz.de
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 * ====================================================================== 
 * 
 * This code is based on the original SLIC code by Radhakrishna Achanta [EPFL].
 * He kindly permits the publication of this code under GPL. This is the preamble
 * of his orginal code
 *  
 * // SLIC.h: interface for the SLIC class.
 * //===========================================================================
 * // This code implements the superpixel method described in:
 * //
 * // Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua, and Sabine Susstrunk,
 * // "SLIC Superpixels",
 * // EPFL Technical Report no. 149300, June 2010.
 * //===========================================================================
 * //	Copyright (c) 2012 Radhakrishna Achanta [EPFL]. All rights reserved.
 * //===========================================================================
 * //////////////////////////////////////////////////////////////////////
 * 
 */

#if !defined(_OWN_SLIC_H_INCLUDED_)
#define _OWN_SLIC_H_INCLUDED_


#include <vector>
#include <string>
#include <algorithm>
#include "opencv2/opencv.hpp"

using namespace std;

/**

This class implements the PreemptiveSLIC algorithm.
For details have a look at:

"Compact Watershed and Preemptive SLIC:\\On improving trade-offs of superpixel segmentation algorithms"
Peer Neubert and Peter Protzel, ICPR 2014

The implementation is based on the original SLIC implementation by 
Radhakrishna Achanta (EPFL).
*/
class PreemptiveSLIC  
{
public:
	PreemptiveSLIC();
	virtual ~PreemptiveSLIC();
    
    // currently seed should be an empty cv::Mat --> otherwise the number of changes in the clusters is potentially not computed correctly
    void preemptiveSLIC(const cv::Mat& I_rgb, const int k, const double compactness, int*& klabels, cv::Mat& seeds);
    void preemptiveSLIC(const cv::Mat& I_rgb, const int region_size, const double compactness, bool perturbseeds, int iterations, bool rgb, int*& klabels, cv::Mat& seeds);
    void initSeeds(const cv::Mat& I,const int n, std::vector<double>& kseedsx, std::vector<double>& kseedsy, std::vector<double>& kseedsl, std::vector<double>& kseedsa,  std::vector<double>& kseedsb, int* klabels, bool init_labels_flag, cv::Mat& seeds);
    void initSeedsStep(const cv::Mat& I,const int region_size, std::vector<double>& kseedsx, std::vector<double>& kseedsy, std::vector<double>& kseedsl, std::vector<double>& kseedsa,  std::vector<double>& kseedsb, int* klabels, bool init_labels_flag, cv::Mat& seeds);
    
	int m_nx;
    int m_ny;
    double m_sx;
    double m_sy;
    double m_pixel_offset; // (e.g. -0.5) to shift and reshift pixel cooridnates to the discrete pixel centers 
	

    vector<double> m_kseedsl;
    vector<double> m_kseedsa;
    vector<double> m_kseedsb;
    vector<double> m_kseedsx;
    vector<double> m_kseedsy;

private:
	//============================================================================
	// The main SLIC algorithm for generating superpixels
	//============================================================================
	void PerformSuperpixelSLIC(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		int*&						klabels,
		const int&					STEP,
                const vector<double>&		edgemag,
		const double&				m = 10.0);
    
    void PerformSuperpixelSLIC_preemptive(
        vector<double>&             kseedsl,
        vector<double>&             kseedsa,
        vector<double>&             kseedsb,
        vector<double>&             kseedsx,
        vector<double>&             kseedsy,
        int*&                       klabels,
        const double&               m = 10.0,
        const int&                  maxIter = 10);
	//============================================================================
	// Pick seeds for superpixels when step size of superpixels is given.
	//============================================================================
	void GetLABXYSeeds_ForGivenStepSize(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		const int&					STEP,
		const bool&					perturbseeds,
		const vector<double>&		edgemag);

	//============================================================================
	// Move the superpixel seeds to low gradient positions to avoid putting seeds
	// at region boundaries.
	//============================================================================
	void PerturbSeeds(
		vector<double>&				kseedsl,
		vector<double>&				kseedsa,
		vector<double>&				kseedsb,
		vector<double>&				kseedsx,
		vector<double>&				kseedsy,
		const vector<double>&		edges);
	//============================================================================
	// Detect color edges, to help PerturbSeeds()
	//============================================================================
	void DetectLabEdges(
		const double*				lvec,
		const double*				avec,
		const double*				bvec,
		const int&					width,
		const int&					height,
		vector<double>&				edges);

	//============================================================================
	// Post-processing of SLIC segmentation, to avoid stray labels.
	//============================================================================
	void EnforceLabelConnectivity(
		const int*					labels,
		const int					width,
		const int					height,
		int*&						nlabels,//input labels that need to be corrected to remove stray labels
		int&						numlabels,//the number of labels changes in the end if segments are removed
		const int&					K); //the number of superpixels desired by the user
	//============================================================================
	// Post-processing of SLIC supervoxel segmentation, to avoid stray labels.
	//============================================================================
	void EnforceSupervoxelLabelConnectivity(
		int**&						labels,//input - previous labels, output - new labels
		const int&					width,
		const int&					height,
		const int&					depth,
		int&						numlabels,
		const int&					STEP);

private:
	int										m_width;
	int										m_height;
	int										m_depth;

	double*									m_lvec;
	double*									m_avec;
	double*									m_bvec;
    
    int m_w_seed;
    int m_h_seed;
};

#endif // !defined(_SLIC_H_INCLUDED_)
