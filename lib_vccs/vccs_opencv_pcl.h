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

#ifndef VCCS_OPENCV_PCL_H
#define	VCCS_OPENCV_PCL_H

#include <opencv2/opencv.hpp>

/** \brief Wrapper for running PCL on OpenCV images given the point cloud as OpenCV image.
 * \author David Stutz
 */
class VCCS_OpenCV_PCL {
public:
    /** \brief Compute superpixels using VCCS. 
     * \param[in] image image to compute superpixels on
     * \param[in] cloud point cloud to compute superpixels in
     * \param[in] voxel_resolution resolution of voxels in meters
     * \param[in] seed_resolution step between superpixels in the cloud (in meters)
     * \param[in] spatial_weight spatial weight
     * \param[in] normal_weight normal weight
     * \param[in] use_transform whether to use the transform, see PCL docs
     * \param[out] labels superpixel labels after backprojection to the image plane
     */
    static void computeSuperpixels(const cv::Mat &image, const cv::Mat &cloud, 
            float voxel_resolution, float seed_resolution, float spatial_weight, 
            float normal_weight, bool use_transform, cv::Mat &labels);
};

#endif	/* VCCS_OPENCV_PCL_H */

