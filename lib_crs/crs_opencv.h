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

#ifndef CRS_OPENCV_H
#define	CRS_OPENCV_H

#include <opencv2/opencv.hpp>
#include "FeatureType.h"
#include "ContourRelaxation.h"
#include "InitializationFunctions.h"

/** \brief Wrapper for running CRS on OpenCV images.
 * \author David Stutz
 */
class CRS_OpenCV {
public:
    /** \brief Compute superpixels using CRS.
     * \param[in] image image to compute superpixels on
     * \param[in] region_height vertical step size between superpixels, implicitly defining the number of superpixels
     * \param[in] region_width horizontal step size between superpixels, implicitly defining the number of superpixels
     * \param[in] clique_cost clique cost, see paper
     * \param[in] compactness compactness parameter
     * \param[in] iterations number of iterations
     * \param[in] color_space color space to use, 0 for YCrCb, 1 for RGB
     * \param[in] labels superpixel labels
     */
    static void computeSuperpixels(const cv::Mat &image, int region_height, 
            int region_width, double clique_cost, double compactness, 
            int iterations, int color_space, cv::Mat &labels) {
        
        double diagonal_cost = clique_cost/std::sqrt(2);
        
        bool color_image = false;
        if (image.channels() == 3) {
            color_image = true;
        }
        
        std::vector<FeatureType> features;
        if (color_image) {
            features.push_back(Color);
        }
        else {
            features.push_back(Grayvalue);
        }

        features.push_back(Compactness);
        
        ContourRelaxation<boost::uint16_t> contour_relaxation(features);
        contour_relaxation.setCompactnessData(compactness);
        
        if (color_image) {
            
            cv::Mat image_YCrCb;
            std::vector<cv::Mat> image_channels;
            
            switch (color_space) {
                default:
                case 0: // YCrCb
                    cv::cvtColor(image, image_YCrCb, CV_BGR2YCrCb);
                    cv::split(image_YCrCb, image_channels);
                    break;
                case 1: // RGB
                    cv::split(image, image_channels);
                    break;
            }

            contour_relaxation.setColorData(image_channels[0], image_channels[1], 
                    image_channels[2]);
        }
        else {
//            cv::Mat imageGray = image.clone();
//            cv::cvtColor(imageGray, image, CV_GRAY2BGR);

            contour_relaxation.setGrayvalueData(image);
        }

        cv::Mat label_image = createBlockInitialization<boost::uint16_t>(image.size(), 
                region_width, region_height);
        cv::Mat relaxed_label_image;
        cv::Mat mean_image;
        
        contour_relaxation.relax(label_image, clique_cost, diagonal_cost, 
                iterations, relaxed_label_image, mean_image);
        
        labels.create(image.rows, image.cols, CV_32SC1);
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                labels.at<int>(i, j) = relaxed_label_image.at<boost::uint16_t>(i, j);
            }
        }
    }
};

#endif	/* CRS_OPENCV_H */

