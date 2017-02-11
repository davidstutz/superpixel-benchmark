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

#ifndef MSS_OPENCV_H
#define	MSS_OPENCV_H

#include <opencv2/opencv.hpp>
#include "superpixel_tools.h"
#include "mss.h"

/** \brief Wrapper for running MSS on OpenCV images.
 * \author David Stutz
 */
class MSS_OpenCV {
public:
    /** \brief Compute superpixels using MSS.
     * \param[in] image image to computer superpixels on
     * \param[out] labels superpixel labels
     * \param[in] region_size
     * \param[in] size
     * \param[in] size
     * \param[in] noise
     * \param[in] tolerance
     * \param[in] iterations
     */
    static void computeSuperpixels(const cv::Mat &image, cv::Mat &labels, 
            int region_size, int size, double noise, double tolerance, int iterations) {
        
        cv::Mat boundaries;
        MSP(image, boundaries, size, noise, tolerance, region_size, 
                region_size, iterations);
        SuperpixelTools::assignBoundariesToSuperpixels(image, boundaries, labels);
    }
};

#endif	/* MSS_OPENCV_H */

