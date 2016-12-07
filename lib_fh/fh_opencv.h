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

#ifndef FH_OPENCV_H
#define	FH_OPENCV_H

#include <opencv2/opencv.hpp>
#include "misc.h"
#include "image.h"
#include "segment-image-labels.h"

/** \brief Wrapper for running FH on OpenCV images. 
 * \author David Stutz
 */
class FH_OpenCV {
public:
    /** \brief Computer superpixels using FH, see README.md for details.
     * \param[in] mat image to computer superpixels on
     * \param[in] sigma sigma parameter for pre-smoothing, see paper
     * \param[in] threshold threshold to stop merging segments
     * \param[in] minimum_size minimum superpixel size to enforce
     * \param[out] labels superpixel labels
     */
    static int computeSuperpixels(const cv::Mat &mat, float sigma, 
            float threshold, int minimum_size, cv::Mat &labels) {
        
        image<rgb>* rgbImage = new image<rgb>(mat.cols, mat.rows);
        
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                imRef(rgbImage, j, i).r = mat.at<cv::Vec3b>(i, j)[2];
                imRef(rgbImage, j, i).g = mat.at<cv::Vec3b>(i, j)[1];
                imRef(rgbImage, j, i).b = mat.at<cv::Vec3b>(i, j)[0];
            }
        }
        
        int superpixels = 0;
        image<int> *segmentation = segment_image_labels(rgbImage, sigma, threshold, minimum_size, &superpixels);

        labels.create(mat.rows, mat.cols, CV_32SC1);
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                labels.at<int>(i, j) = imRef(segmentation, j, i);
            }
        }
        
        delete rgbImage;
        delete segmentation;
        
        return superpixels;
    }
};

#endif	/* FH_OPENCV_H */

