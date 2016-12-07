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

#ifndef SUPERPIXEL_TOOLS_H
#define	SUPERPIXEL_TOOLS_H

#include <opencv2/opencv.hpp>

/** \brief Superpixel utilities.
 * \author David Stutz
 */
class SuperpixelTools {
public:
    /** \brief Compute region size from desired number of superpixels.
     * \param[in] image image for number of rows and cols
     * \param[in] superpixels number of desired superpixels
     * \return region size required for the desired number of superpixels on the given image size
     */
    static int computeRegionSizeFromSuperpixels(const cv::Mat &image, int superpixels);
    
    /** \brief Compute width and height for the given number of superpixels.
     * \param[in] image image for the number of rows and cols
     * \param[in] superpixels number of desired superpixels
     * \param[out] height region height to achieve desired number of superpixels
     * \param[out] width region width to achieve desired number of superpixels
     */
    static void computeHeightWidthFromSuperpixels(const cv::Mat &image, 
            int superpixels, int &height, int &width);
    
    /** \brief Compute width, height and levels for the given number of superpixels.
     * \param[in] image image for the number of rows and cols
     * \param[in] superpixels desired number of superpixels
     * \param[out] height region height on lowest level
     * \param[out] width region width on lowest level
     * \param[out] levels number of levels to achieve the desired number of superpixels
     */
    static void computeHeightWidthLevelsFromSuperpixels(const cv::Mat &image, 
            int superpixels, int &height, int &width, int &levels);
    
    /** \brief Compute region size and levels for the given number of superpixels.
     * \param[in] image image for the number of rows and cols
     * \param[in] superpixels desired number of superpixels
     * \param[out] region_size region size on lowest level
     * \param[out] levels number of levels to achieve desired number of superpixels
     */
    static void computeRegionSizeLevels(const cv::Mat &image, 
            int superpixels, int &region_size, int &levels);
    
    /** \brief Relabel superpixel segmentation.
     * \param[in] labels superpixel labels to relabel
     */
    static void relabelSuperpixels(cv::Mat &labels);
    
    /** \brief Compute label map from boundaries by assigning boundary pixels to the 
     * nearest superpixel and flooding the connected components.
     * \param[in] image image to compute superpixel labels for
     * \param[in] boundaries given boundaries for image
     * \param[out] labels computed superpixel labels
     * \param[in] BOUDNARY_VALUE the value a boundary pixel takes
     * \param[in] INNER_VALUE the value an inner pixel takes
     */
    static void computeLabelsFromBoundaries(const cv::Mat &image, const cv::Mat &boundaries, 
            cv::Mat &labels, int BOUNDARY_VALUE = -1, int INNER_VALUE = -2);
    
    /** \brief Assigns boundary pixels (indicated by -1) to the nearest superpixel 
     * (indicated by labels >= 0).
     * \param[in] image image to compute superpixels for
     * \param[in] boudnaries given boundaries to convert to superpixel labels
     * \param[in] BOUDARY_VALUE value a boundary pixel takes
     */
    static void assignBoundariesToSuperpixels(const cv::Mat &image, 
            const cv::Mat &boundaries, cv::Mat &labels, int BOUNDARY_VALUE = -1);
    
    /** \brief Count number of superpixels.
     * \param[in] labels superpixel labels
     * \return number of superpixels
     */
    static int countSuperpixels(const cv::Mat &labels);
    
    /** \brief Relabel superpixels based on connected components.
     * \param[in] labels superpixel labels to relabel as connected
     */
    static int relabelConnectedSuperpixels(cv::Mat &labels);
    
    /** \brief Enforce the minimum segment size by merging small superpixels
     * with similar neighboring superpixels.
     * \param[in] image image to enforce minimum superpixel size for
     * \param[in] labels superpixel labels
     * \param[in] size minimum superpixel size
     * \return new number of superpixels
     */
    static int enforceMinimumSuperpixelSize(const cv::Mat &image, cv::Mat &labels, 
            int size);
    
    /** \brief Enforce the minimum segment size by merging small superpixels
     * with similar neighboring superpixels such that the given number of
     * superpixels is merged.
     * \param[in] image image to enforce minimum superpixels on
     * \param[in] labels superpixel labels
     * \param[in] number number of desired superpixels after enforcing minimum size
     */
    static int enforceMinimumSuperpixelSizeUpTo(const cv::Mat &image, cv::Mat &labels, 
            int number);

};

#endif	/* SUPERPIXEL_TOOLS_H */

