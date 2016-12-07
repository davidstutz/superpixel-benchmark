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

#ifndef VISUALIZATION_H
#define	VISUALIZATION_H

#include <opencv2/opencv.hpp>

/** \brief Some basic visualizations of superpixel segmentations.
 * \author David Stutz
 */
class Visualization {
public:
    /** \brief Draw contours.
     * \param[in] image image to draw contours in
     * \param[in] labels superpixel labels
     * \param[out] contours copy of the images with painted contours
     * \param[in] eight_connected whether to use eight connected graph
     */
    static void drawContours(const cv::Mat &image, const cv::Mat &labels, 
            cv::Mat &contours, bool eight_connected = false);
    
    /** \brief Color superpixels randomly.
     * \param[in] image image to draw contours in
     * \param[in] labels superpixel labels
     * \param[out] random image with randomly colored superpixels
     */
    static void drawRandom(const cv::Mat &labels, cv::Mat &random);
    
    /** \brief Draw mean colored superpixels.
     * \param[in] image image to draw contours in
     * \param[in] labels superpixel labels
     * \param[out] means image with superpixels colored by the mean color
     */
    static void drawMeans(const cv::Mat &image, const cv::Mat &labels, 
            cv::Mat &means);
    
    /** \brief Draw perturbed mean colored superpixels.
     * \param[in] image image to draw contours in
     * \param[in] labels superpixel labels
     * \param[out] means image with superpixels colored by the mean color plus a small noise term
     */
    static void drawPerturbedMeans(const cv::Mat &image, const cv::Mat &labels, 
            cv::Mat &means);
    
    /** \brief Indicate false negatives and false positives.
     * \param[in] image image to draw contours in
     * \param[in] labels superpixel labels
     * \param[out] pre_rec visualization of false negatives and positives
     * \param[in] d fraction of diagonal to use as tolerance
     */
    static void drawPrecisionRecall(const cv::Mat &image, const cv::Mat &labels, 
            const cv::Mat &gt, cv::Mat &pre_rec, float d = 0.0025);
    
    /** \brief Indicate Undersegmentation Error.
     * \param[in] image image to draw contours in
     * \param[in] labels superpixel labels
     * \param[in] gt ground truth segmentation
     * \param[out] ue image with undersegmentation error indicated red
     */
    static void drawUndersegmentationError(const cv::Mat &image, const 
            cv::Mat &labels, const cv::Mat &gt, cv::Mat &ue);
    
};

#endif	/* VISUALIZATION_H */

