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

#include "Parameters.hpp"
#include "Superpixels.hpp"
#include "Plots.hpp"
#include "Segmentation.hpp"
#include "Neighbourhood.hpp"
#include "IO.hpp"
#include "density/PointDensity.hpp"
#include "lib_dasp_slimage/Slimage/Slimage.hpp"
#include "dasp_opencv.h"

void DASP_OpenCV::computeSuperpixels(const cv::Mat &image, const cv::Mat &depth, 
        int desired_superpixels, float spatial_weight, float normal_weight, 
        int seed_mode, int iterations, dasp::Camera camera, cv::Mat &labels) {
    
    dasp::Parameters opt;
    opt.camera = camera;
    opt.weight_spatial = spatial_weight;
    opt.weight_color = 1 - spatial_weight - normal_weight;
    opt.weight_normal = normal_weight;
    opt.count = desired_superpixels;
    opt.width = image.cols;
    opt.height = image.rows;
    opt.iterations = iterations;
    
    if (seed_mode == SEED_MODE_RANDOM) {
        opt.seed_mode = dasp::SeedModes::Random;
    }
    else if(seed_mode == SEED_MODE_SPDS) {
        opt.seed_mode = dasp::SeedModes::SimplifiedPDS;
    }
//    else if(seed_mode == SEED_MODE_DELTA) {
//        opt.seed_mode = dasp::SeedModes::Delta;
//    }
    else {
        // TODO
    }
    
    dasp::Superpixels superpixels;
    slimage::Image3ub slimage_color;
    slimage::Image1ui16 slimage_depth;
    slimage::Image1i slimage_labels;
        
    slimage_color = slimage::Image3ub(image.cols, image.rows);
    slimage_depth = slimage::Image1ui16(image.cols, image.rows);

    // Convert OpenCV images to SLImage images.
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            const auto& pixel = slimage_color(j, i);
            pixel[0] = image.at<cv::Vec3b>(i, j)[0];
            pixel[1] = image.at<cv::Vec3b>(i, j)[1];
            pixel[2] = image.at<cv::Vec3b>(i, j)[2];

            slimage_depth(j, i) = (uint16_t) depth.at<unsigned short>(i, j);
        }
    }
        
    superpixels.opt = opt;
    dasp::ComputeSuperpixelsIncremental(superpixels, slimage_color, slimage_depth);
    slimage_labels = superpixels.ComputeLabels();

    // ComputeLabels uses -1 as default value, so there may be a -1 as label.
    int min_label = 0;
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            if (slimage_labels(j, i) < min_label) {
                min_label = slimage_labels(j, i);
            }
        }
    }

    labels.create(image.rows, image.cols, CV_32SC1);
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            labels.at<int>(i, j) = slimage_labels(j, i) + abs(min_label);
        }
    }
}