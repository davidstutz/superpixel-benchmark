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

#include <glog/logging.h>
#include "depth_tools.h"

////////////////////////////////////////////////////////////////////////////////
// projectZ
////////////////////////////////////////////////////////////////////////////////

float DepthTools::Camera::projectZ(unsigned short depth, float factor) const {
    return ((float) depth/factor);
}

////////////////////////////////////////////////////////////////////////////////
// projectX
////////////////////////////////////////////////////////////////////////////////

template <typename T>
float DepthTools::Camera::projectX(T x, unsigned short depth, float factor) const {
    return (((float) x + cropping_x) - principal_x) * (((float) depth)/factor)/focal_x;
}

template float DepthTools::Camera::projectX<int>(int, unsigned short, float) const;
template float DepthTools::Camera::projectX<float>(float, unsigned short, float) const;

////////////////////////////////////////////////////////////////////////////////
// projectY
////////////////////////////////////////////////////////////////////////////////

template <typename T>
float DepthTools::Camera::projectY(T y, unsigned short depth, float factor) const {
    return (((float) y + cropping_y) - principal_y) * (((float) depth)/factor)/focal_y;
}

template float DepthTools::Camera::projectY<int>(int, unsigned short, float) const;
template float DepthTools::Camera::projectY<float>(float, unsigned short, float) const;

////////////////////////////////////////////////////////////////////////////////
// backprojectX
////////////////////////////////////////////////////////////////////////////////

int DepthTools::Camera::backprojectX(float x, float depth, float factor) const {
    return round(x*focal_x*factor/depth + principal_x - cropping_x);
}

////////////////////////////////////////////////////////////////////////////////
// backprojectY
////////////////////////////////////////////////////////////////////////////////

int DepthTools::Camera::backprojectY(float y, float depth, float factor) const {
    return round(y*focal_y*factor/depth + principal_y - cropping_y);
}

////////////////////////////////////////////////////////////////////////////////
// computeCloudFromDepth
////////////////////////////////////////////////////////////////////////////////

void DepthTools::computeCloudFromDepth(const cv::Mat &depth, const DepthTools::Camera &camera,
        cv::Mat &cloud) {
    
    cloud.create(depth.rows, depth.cols, CV_32FC3);
    for (int i = 0; i < depth.rows; ++i) {
        for (int j = 0; j < depth.cols; ++j) {
            cloud.at<cv::Vec3f>(i, j)[0] = camera.projectX<int>(j, depth.at<unsigned short>(i, j));
            cloud.at<cv::Vec3f>(i, j)[1] = camera.projectY<int>(i, depth.at<unsigned short>(i, j));
            cloud.at<cv::Vec3f>(i, j)[2] = camera.projectZ(depth.at<unsigned short>(i, j));
            
            LOG_IF (FATAL, std::isinf(cloud.at<cv::Vec3f>(i, j)[0])) << "Infinite cloud value!";
            LOG_IF (FATAL, std::isinf(cloud.at<cv::Vec3f>(i, j)[1])) << "Infinite cloud value!";
            LOG_IF (FATAL, std::isinf(cloud.at<cv::Vec3f>(i, j)[2])) << "Infinite cloud value!";
        }
    }
}