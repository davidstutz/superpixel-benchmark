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

#ifndef ERGC_OPENCV_H
#define	ERGC_OPENCV_H

#include <CImg.h>
#include <opencv2/opencv.hpp>
#include "ergc.h"

/** \brief Wrapper for running ERGC on OpenCV images.
 * \author David Stutz
 */
class ERGC_OpenCV {
public:
    /** \brief Computer superpixels using ERGC. 
     * \param[in] image image to computer superpixels on
     * \param[in] region_height horizontal step between superpixel centers, implicitly defining the number of superpixels
     * \param[in] region_width vertical step between superpixel centers, implicitly defining the number of superpixels
     * \param[in] lab whether to use Lab color space
     * \param[in] perturb_seeds whether to perturb seeds to increase performance
     * \param[in] m m parameter, see paper
     * \param[out] labels superpixel labels
     */
    static void computeSuperpixels(const cv::Mat &image, int region_height, int region_width, 
            bool lab, bool perturb_seeds, int m, cv::Mat &labels) {
        
        int dx = region_width; // Seeds sampling wrt axis x (for custom grids)
        int dy = region_height; // Seeds sampling wrt axis y (for custom grids)
        // int m = 0; // Compacity value

        /////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////
        CImg<> im(image.cols, image.rows, 1, 3);
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                im(j, i, 0, 0) = image.at<cv::Vec3b>(i, j)[0];
                im(j, i, 0, 1) = image.at<cv::Vec3b>(i, j)[1];
                im(j, i, 0, 2) = image.at<cv::Vec3b>(i, j)[2];
            }
        }
        
        // useful variables
        CImg<> distances;
        CImg<int> states, seeds;
        vector<SP*> SPs;

        // convert to Lab if needed (better superpixels with color images)
        if (lab) {
            im.RGBtoLab();
        }

        CImg<> gradient;
        if (perturb_seeds) {
            gradient = compute_gradient(im);
        }

        placeSeedsOnCustomGrid2d(im.width(), im.height(), dx, dy, seeds);

        if (perturb_seeds) {
            CImg<int> perturbedSeeds;
            if (im.depth() == 1) {
                perturbSeeds2d(seeds, gradient, perturbedSeeds);
            }
            else {
                perturbSeeds3d(seeds, gradient, perturbedSeeds);
            }
            
            seeds = perturbedSeeds;
        }

        initialize_images(seeds, distances, states);
        SPs = initialize_superpixels(im, seeds);

        fmm3d(distances, seeds, states, im, SPs, m);

        labels.create(image.rows, image.cols, CV_32SC1);
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                labels.at<int>(i, j) = seeds(j, i, 0, 0);
            }
        }
    }
};

#endif	/* ERGC_OPENCV_H */

