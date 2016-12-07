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

#ifndef CIS_OPENCV_H
#define	CIS_OPENCV_H

#include <opencv2/opencv.hpp>
#include "superpixels.h"

/** \brief Wrapper for running CIS with OpenCV images.
 * \author David Stutz
 */
class CIS_OpenCV {
public:
    /** \brief Computing superpixels using CIS; for details also see README.md.
     * \param[in] mat image to compute superpixels on
     * \param[in] region_size region size implicitly defining the number of superpixels
     * \param[in] lambda lambda parameter, see paper
     * \param[in] iterations number of iterations
     * \param[in] type type to use; 0 for compact superpixels, 1 for constant intensity superpixels
     * \param[in] sigma sigma parameter, see paper
     * \param[in] color whether to use color
     * \param[out] labels superpixel labels
     */
    static void computeSuperpixels(const cv::Mat &mat, int region_size, 
        int lambda, int iterations, int type, float sigma, bool color, cv::Mat &labels) {
        
        cv::Mat image_gray;
        cv::cvtColor(mat, image_gray, CV_BGR2GRAY);

        cv::Mat grad_x;
        cv::Mat grad_y;

        cv::Sobel(image_gray, grad_x, -1, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(grad_x, grad_x);

        cv::Sobel(image_gray, grad_y, -1, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
        cv::convertScaleAbs(grad_y, grad_y);

        image<unsigned char> *I_gray = new image<unsigned char>(mat.cols, mat.rows);
        for (int i = 0; i < mat.rows; i++){
            for (int j = 0; j < mat.cols; j++) {
                imRef(I_gray, j, i) = image_gray.at<unsigned char>(i, j);
            }
        }
        
        image<rgb> *I;
        if (color) {
            I = new image<rgb>(mat.cols, mat.rows);
            for (int i = 0; i < mat.rows; i++){
                for (int j = 0; j < mat.cols; j++) {
                    rgb color;
                    color.r = mat.at<cv::Vec3b>(i, j)[2];
                    color.g = mat.at<cv::Vec3b>(i, j)[1];
                    color.b = mat.at<cv::Vec3b>(i, j)[0];
                    
                    imRef(I, j, i) = color;
                }
            }
        }
        
        int width  = I_gray->width();
        int height = I_gray->height();
        int num_pixels = width*height;

        float variance;
        if (color) {
            variance = computeImageVarianceColor(I, width, height);
        }
        else {
            variance = computeImageVariance(I_gray, width, height);
        }
        
        // Initialize and place seeds
        std::vector<int> Seeds(num_pixels);
        int numSeeds = 0;
        PlaceSeeds(I_gray, width, height, num_pixels, Seeds, &numSeeds, region_size);
        MoveSeedsFromEdges(I_gray, width, height, num_pixels, Seeds, numSeeds, region_size);

        std::vector<int> weights_horizontal(num_pixels, lambda);
        std::vector<int> weights_vertical(num_pixels, lambda);
        std::vector<int> weights_diagonal_1(num_pixels, lambda);
        std::vector<int> weights_diagonal_2(num_pixels, lambda);

        image<unsigned char> *I_x = new image<unsigned char>(mat.cols, mat.rows);
        image<unsigned char> *I_y = new image<unsigned char>(mat.cols, mat.rows);
        for (int i = 0; i < mat.rows; i++){
            for (int j = 0; j < mat.cols; j++) {
                imRef(I_x, j, i) = grad_x.at<unsigned char>(i, j);
                imRef(I_y, j, i) = grad_y.at<unsigned char>(i, j);
            }
        }
        
        loadEdges(weights_horizontal, num_pixels, width, height, lambda, I_x);
        loadEdges(weights_vertical, num_pixels, width, height, lambda, I_y);
        
        if (color) {
            computeWeightsColor(weights_horizontal, num_pixels, width,height, 
                    lambda, variance, -1, 0, I, type, sigma);
            computeWeightsColor(weights_vertical, num_pixels, width,height, 
                    lambda, variance, 0, -1, I, type, sigma);
            computeWeightsColor(weights_diagonal_1, num_pixels, width,height, 
                    lambda, variance, -1, -1, I, type, sigma);
            computeWeightsColor(weights_diagonal_2, num_pixels, width,height, 
                    lambda, variance, 1, -1, I, type, sigma);
        }
        else {
            computeWeights(weights_horizontal, num_pixels, width,height, 
                    lambda, variance, -1, 0, I_gray, type, sigma);
            computeWeights(weights_vertical, num_pixels, width,height, 
                    lambda, variance, 0, -1, I_gray, type, sigma);
            computeWeights(weights_diagonal_1, num_pixels, width,height, 
                    lambda, variance, -1, -1, I_gray, type, sigma);
            computeWeights(weights_diagonal_2, num_pixels, width,height, 
                    lambda, variance, 1, -1, I_gray, type, sigma);
        }
        
        vector<int> labeling(num_pixels);
        initializeLabeling(labeling, width, height, Seeds, numSeeds, region_size);

        int oldEnergy, newEnergy;

        std::vector<int> changeMask(num_pixels, 1);
        std::vector<int> changeMaskNew(num_pixels, 0);

        std::vector<int> order(numSeeds);
        for (int i = 0; i < numSeeds; i++) {
            order[i] = i;
        }

        int j = 0;
        //purturbSeeds(order,numSeeds);

        while (true) {
            if (color) {
                newEnergy = computeEnergyColor(labeling, width, height, 
                        num_pixels, weights_horizontal, weights_vertical, 
                        weights_diagonal_1, weights_diagonal_2, Seeds, I, type);
            }
            else {
                newEnergy = computeEnergy(labeling, width, height, num_pixels, 
                        weights_horizontal, weights_vertical, weights_diagonal_1, 
                        weights_diagonal_2, Seeds, I_gray, type);
            
            }
            if (j == 0) {
                oldEnergy = newEnergy + 1;
            }

            if (newEnergy == oldEnergy || j >= iterations) { 
                break;
            }

            oldEnergy = newEnergy;

            for (int i = 0; i < numSeeds; i++) {
                if (color) {
                    expandOnLabelColor(order[i], width, height, num_pixels, 
                            Seeds, numSeeds, labeling, weights_horizontal,
                            weights_vertical, lambda, weights_diagonal_1, 
                            weights_diagonal_2, region_size, changeMask,
                            changeMaskNew, I, type, variance);
                }
                else {
                    expandOnLabel(order[i], width, height, num_pixels, Seeds, 
                            numSeeds, labeling, weights_horizontal, weights_vertical, 
                            lambda, weights_diagonal_1, weights_diagonal_2, region_size, 
                            changeMask, changeMaskNew, I_gray, type, variance);
                }
            }

            for (int i = 0; i < num_pixels; i++) {
                changeMask[i] = changeMaskNew[i];
                changeMaskNew[i] = 0;
            }

            //purturbSeeds(order, numSeeds);
            j++;
        }

        labels.create(mat.rows, mat.cols, CV_32SC1);
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                labels.at<int>(i, j) = labeling[j + i*width];
            }
        }

        delete I_gray;
        if (color) {
            delete I;
        }
        delete I_x;
        delete I_y;
    }
    
};

#endif	/* CIS_OPENCV_H */

