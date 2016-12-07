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

#ifndef VC_OPENCV__H
#define	VC_OPENCV__H

#include "VCells.h"
#include <opencv2/opencv.hpp>

/** \brief Wrapper for running VC on OpenCV images.
 * \author David Stutz
 */
class VC_OpenCV {
public:
    /** \brief Compute superpixels using VC.
     * \param[in] image image to compute superpixels on
     * \param[in] superpixels numberof superpixels
     * \param[in] weight_length weight length parameter, see paper
     * \param[in] radius radius parameter, see paper
     * \param[in] num_nei_cluster see paper
     * \param[in] num_direct_nei see paper
     * \param[in] threshold see paper
     * \param[out] labels superpixel labels
     */
    static void computeSuperpixels(const cv::Mat &image, int superpixels, 
            double weight_length, int radius, int num_nei_cluster, 
            int num_direct_nei, int threshold, cv::Mat &labels) {
        
        // i: height, j: width, k: channels
        // (pBmpBuf + i * lineByte + j * 3 + k)
        VCells vc(superpixels, weight_length);
//	vc.pBmpBuf = new unsigned char[image.rows*image.cols*image.channels()];
        vc.bmpWidth = image.cols;
        vc.bmpHeight = image.rows;
        vc.lineByte = vc.bmpWidth;
        // lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;
        
	struct pixel* pixelArray = new pixel[vc.bmpHeight*vc.bmpWidth];
        for (int i = 0; i < vc.bmpHeight; i++) {
            for (int j = 0; j < vc.bmpWidth; j++) {
                int index = vc.getIndexFromRC(i, j);
                pixelArray[index].color[0] = image.at<cv::Vec3b>(i, j)[0];
                pixelArray[index].color[1] = image.at<cv::Vec3b>(i, j)[1];
                pixelArray[index].color[2] = image.at<cv::Vec3b>(i, j)[2];
            }
	}
        
	/************************************************************************************/
	/************                     initialization                         ************/
	/************************************************************************************/
	struct centroid* generators = new centroid[superpixels];
	vc.initializePixel(pixelArray);
	vc.initializeGenerators(generators, pixelArray); 
	vc.classicCVT(pixelArray, generators);

	/************************************************************************************/
	/************                     VCells by EWCVT                        ************/
	/************************************************************************************/
	vc.EWCVT(generators, pixelArray);
        
        labels.create(image.rows, image.cols, CV_32SC1);
        for (int i = 0; i < vc.bmpHeight; i++) {
            for (int j = 0; j < vc.bmpWidth; j++) {
                int index = vc.getIndexFromRC(i, j);
                labels.at<int>(i, j) = pixelArray[index].indexCluster;
            }
	}
        
	delete[] pixelArray;
	delete[] generators;
    }
};

#endif	/* VC_OPENCV__H */

