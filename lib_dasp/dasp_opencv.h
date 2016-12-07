/**
 * Copyright (c) 2016, David Stutz
 * Contact: david.stutz@rwth-aachen.de, davidstutz.de
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DASP_OPENCV_H
#define	DASP_OPENCV_H

#include <opencv2/opencv.hpp>
#include "Tools.hpp"

/** \brief Wrapper for running DASP on OpenCV images.
 * \author David Stutz
 */
class DASP_OpenCV {
public:
    /** \brief Random seed mode. */
    static const int SEED_MODE_RANDOM = 0;
    /** \brief SPDS seed mode. */
    static const int SEED_MODE_SPDS = 1;
    /** \brief Delta seed mode. */
    static const int SEEDS_MODE_DELTA = 2;
    
    /** \brief Compute superpixels using DASP; see README.md for details.
     * \param[in] image image to compute superpixels on
     * \param[in] depth depth image as unsigned short
     * \param[in] superpixels number of superpixels to generate
     * \param[in] spatial_weight weight of spatial dimensions
     * \param[in] normal_weight weight of normals
     * \param[in] seed_mode seed mode to use
     * \param[in] iterations number of iterations
     * \param[in] camera dasp::Camera object specifying camera parameters, see dasp_cli/main.cpp and lib_eval/depth_tools.h for details
     * \param[out] labels superpixel labels
     */
    static void computeSuperpixels(const cv::Mat &image, const cv::Mat &depth, 
            int superpixels, float spatial_weight, float normal_weight, 
            int seed_mode, int iterations, dasp::Camera camera, cv::Mat &labels);
};

#endif	/* DASP_OPENCV_H */

