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

#ifndef LSC_OPENCV_H
#define	LSC_OPENCV_H

#include <opencv2/opencv.hpp>
#include "LSC.h"

/** \brief Wrapper for running LSC on OpenCV images.
 * \author David Stutz
 */
class LSC_OpenCV {
public:
    /** \brief Compute superpixels using LSC.
     * \param[in] image image to computer superpixels on
     * \param[in] region_height horizontal step between superpixel centers, implicitly defining the number of superpixels
     * \param[in] region_width vertical step between superpixel centers, implicitly defining the number of superpixels
     * \param[in] ration compactness parameter
     * \param[in] iterations number of iterations
     * \param[in] threshold threshold for enforcing connectivity
     * \param[in] color space, >0 for Lab, 0 for RGB
     * \param[out] labels superpixel labels
     */
    static void computeSuperpixels(const cv::Mat &image, int region_height, 
            int region_width, double ratio, int iterations, int threshold, 
            int color_space, cv::Mat &labels)
    {
        unsigned char* R = new unsigned char[image.rows*image.cols];
        unsigned char* G = new unsigned char[image.rows*image.cols];
        unsigned char* B = new unsigned char[image.rows*image.cols];
        
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                R[i*image.cols + j] = image.at<cv::Vec3b>(i, j)[2];
                G[i*image.cols + j] = image.at<cv::Vec3b>(i, j)[1];
                B[i*image.cols + j] = image.at<cv::Vec3b>(i, j)[0];
            }
        }
        
        unsigned short* labeling = new unsigned short[image.rows*image.cols];
        for (int i = 0; i < image.rows*image.cols; i++) {
            labeling[i] = 0;
        }
        
        LSC(R, G, B, image.rows, image.cols, region_height, region_width, ratio, 
                iterations, threshold, color_space, labeling);
        
        labels.create(image.rows, image.cols, CV_32SC1);
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                labels.at<int>(i, j) = labeling[i*image.cols + j];
            }
        }
    }
};

#endif	/* LSC_OPENCV_H */

