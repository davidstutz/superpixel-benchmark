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

#ifndef VLSLIC_OPENCV_H
#define	VLSLIC_OPENCV_H

#include <opencv2/opencv.hpp>
#include "slic.h"

/** \brief Wrapper for running vlSLIC on OpenCV images.
 * \author David Stutz
 */
class VLSLIC_OpenCV {
public:
    /** \brief Computing superpixels using vlSLIC. 
     * \param[in] mat image to compute superpixels on
     * \param[in] region_size region size between superpixel centers, implicitly defining number of superpixels
     * \param[in] regularization compactness parameter
     * \param[in] min_region_size minimum size of superpixels
     * \param[out] superpixel labels
     */
    static void computeSuperpixels(const cv::Mat &mat, int region_size, 
            double regularization, int min_region_size, int iterations, cv::Mat &labels)
    {
        // Convert image to one-dimensional array.
        float* image = new float[mat.rows*mat.cols*mat.channels()];
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                if (mat.channels() == 1) {
                    image[j + mat.cols*i] = mat.at<unsigned char>(i, j);
                }
                else if (mat.channels() == 3) {
                    image[j + mat.cols*i + mat.cols*mat.rows*0] = mat.at<cv::Vec3b>(i, j)[0];
                    image[j + mat.cols*i + mat.cols*mat.rows*1] = mat.at<cv::Vec3b>(i, j)[1];
                    image[j + mat.cols*i + mat.cols*mat.rows*2] = mat.at<cv::Vec3b>(i, j)[2];
                }
            }
        }
        
        vl_uint32* segmentation = new vl_uint32[mat.rows*mat.cols];
        vl_size height = mat.rows;
        vl_size width = mat.cols;
        vl_size channels = mat.channels();
        
        vl_slic_segment_t(segmentation, image, width, height, channels, region_size, 
                regularization, min_region_size, iterations);
        
        // Convert segmentation.
        labels.create(mat.rows, mat.cols, CV_32SC1);
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                labels.at<int>(i, j) = (int) segmentation[j + mat.cols*i];
            }
        }
    }
};

#endif	/* VLSLIC_OPENCV_H */

