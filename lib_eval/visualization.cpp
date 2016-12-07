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
#include <random>
#include "evaluation.h"
#include "visualization.h"

void Visualization::drawContours(const cv::Mat &image, const cv::Mat &labels, cv::Mat &contours,
            bool eight_connected) {
    
    LOG_IF(FATAL, image.rows != labels.rows || image.cols != labels.cols) 
            << "Image size and superpixel segmentation size do not match: " 
            << image.size() << "!=" << labels.size();
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only three-channel images are supported.";
    
    contours.create(image.rows, image.cols, CV_8UC3);
    cv::Vec3b color(0, 0, 0);
    
    for (int i = 0; i < contours.rows; ++i) {
        for (int j = 0; j < contours.cols; ++j) {
            if (Evaluation::is4ConnectedBoundaryPixel(labels, i, j) 
                    || (eight_connected && Evaluation::is8Minus4ConnectedBoundaryPixel(labels, i, j))) {
                
                contours.at<cv::Vec3b>(i, j) = color;
            }
            else {
                contours.at<cv::Vec3b>(i, j) = image.at<cv::Vec3b>(i, j);
            }
        }
    }
}

cv::Vec3b getRandomColor(int label, int discretization) {
    std::hash<int> hash;
    std::size_t factor = hash(100000*label);
    
    int b = factor%discretization;
    int g = (factor/discretization)%discretization;
    int r = (factor/discretization/discretization)%discretization;
    
    cv::Vec3b color;
    color[0] = b*(256/discretization);
    color[1] = g*(256/discretization);
    color[2] = r*(256/discretization);
    
    return color;
}

void Visualization::drawRandom(const cv::Mat &labels, cv::Mat &random) {
    
    LOG_IF(FATAL, labels.empty()) << "Given labels are empty.";
    
    int max_label = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > max_label) {
                max_label = labels.at<int>(i, j);
            }
        }
    }
    
    int discretization = 1;
    int number = std::pow(256/discretization, 3);
    
    while (number > max_label) {
        discretization *= 2;
        number = std::pow(256/discretization, 3);
    }
    
    random.create(labels.rows, labels.cols, CV_8UC3);
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            random.at<cv::Vec3b>(i, j) = getRandomColor(labels.at<int>(i, j), 
                    discretization);
        }
    }
}

void Visualization::drawPerturbedMeans(const cv::Mat &image, const cv::Mat &labels, 
            cv::Mat &mean_image) {
    
    LOG_IF(FATAL, image.rows != labels.rows || image.cols != labels.cols) 
            << "Image size and superpixel segmentation size do not match: " 
            << image.size() << "!=" << labels.size();
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only three-channel images are supported.";
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<cv::Vec3f> means(superpixels, cv::Vec3f(0, 0, 0));
    std::vector<cv::Vec3f> perturbation(superpixels, cv::Vec3f(0, 0, 0));
    std::vector<int> counts(superpixels, 0);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> gaussian(0, 8);
    
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            means[labels.at<int>(i, j)][0] += image.at<cv::Vec3b>(i, j)[0];
            means[labels.at<int>(i, j)][1] += image.at<cv::Vec3b>(i, j)[1];
            means[labels.at<int>(i, j)][2] += image.at<cv::Vec3b>(i, j)[2];
            
            while (perturbation[labels.at<int>(i, j)][0] == 0
                    && perturbation[labels.at<int>(i, j)][1] == 0
                    && perturbation[labels.at<int>(i, j)][2] == 0) {
                
                perturbation[labels.at<int>(i, j)][0] += gaussian(gen);
                perturbation[labels.at<int>(i, j)][1] += gaussian(gen);
                perturbation[labels.at<int>(i, j)][2] += gaussian(gen);
            }
            
            counts[labels.at<int>(i, j)]++;
        }
    }
    
    for (int k = 0; k < superpixels; ++k) {
        means[k] /= counts[k];
    }
    
    mean_image.create(image.rows, image.cols, CV_8UC3);
    for (int i = 0; i < mean_image.rows; ++i) {
        for (int j = 0; j < mean_image.cols; ++j) {
            
            cv::Vec3f color = means[labels.at<int>(i, j)] + perturbation[labels.at<int>(i, j)];
            color[0] = std::max(0.f, std::min(255.f, color[0]));
            color[1] = std::max(0.f, std::min(255.f, color[1]));
            color[2] = std::max(0.f, std::min(255.f, color[2]));
            
            mean_image.at<cv::Vec3b>(i, j) = color;
        }
    }
}

void Visualization::drawMeans(const cv::Mat &image, const cv::Mat &labels, 
        cv::Mat &mean_image) {
    
    LOG_IF(FATAL, image.rows != labels.rows || image.cols != labels.cols) 
            << "Image size and superpixel segmentation size do not match: " 
            << image.size() << "!=" << labels.size();
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only three-channel images are supported.";
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<cv::Vec3f> means(superpixels, cv::Vec3f(0, 0, 0));
    std::vector<int> counts(superpixels, 0);
    
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            means[labels.at<int>(i, j)][0] += image.at<cv::Vec3b>(i, j)[0];
            means[labels.at<int>(i, j)][1] += image.at<cv::Vec3b>(i, j)[1];
            means[labels.at<int>(i, j)][2] += image.at<cv::Vec3b>(i, j)[2];
            
            counts[labels.at<int>(i, j)]++;
        }
    }
    
    for (int k = 0; k < superpixels; ++k) {
        means[k] /= counts[k];
    }
    
    mean_image.create(image.rows, image.cols, CV_8UC3);
    for (int i = 0; i < mean_image.rows; ++i) {
        for (int j = 0; j < mean_image.cols; ++j) {
            mean_image.at<cv::Vec3b>(i, j) = means[labels.at<int>(i, j)];
        }
    }
}

void Visualization::drawPrecisionRecall(const cv::Mat &image, const cv::Mat &labels, 
        const cv::Mat &gt, cv::Mat &pre_rec, float d)
{
    LOG_IF(FATAL, image.rows != labels.rows || image.cols != labels.cols) 
            << "Image size and superpixel segmentation size do not match: " 
            << image.size() << "!=" << labels.size();
    LOG_IF(FATAL, image.rows != gt.rows || image.cols != gt.cols) 
            << "Image size and ground truth size do not match: " 
            << image.size() << "!=" << gt.size();
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only three-channel images are supported.";
    
    int H = image.rows;
    int W = image.cols;
    int r = std::round(d*std::sqrt(H*H + W*W));
    
    pre_rec = image.clone();
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            if (Evaluation::is4ConnectedBoundaryPixel(gt, i, j)) {

                bool pos = false;
                for (int k = std::max(0, i - r); k < std::min(H - 1, i + r) + 1; k++) {
                    for (int l = std::max(0, j - r); l < std::min(W - 1, j + r) + 1; l++) {
                        if (Evaluation::is4ConnectedBoundaryPixel(labels, k, l)) {
                            pos = true;
                        }
                    }
                }

                if (!pos) {
                    // This is a false negative!
                    pre_rec.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
                }
            }
            else if (Evaluation::is4ConnectedBoundaryPixel(labels, i, j)) {
                
                bool pos = false;
                for (int k = std::max(0, i - r); k < std::min(H - 1, i + r) + 1; k++) {
                    for (int l = std::max(0, j - r); l < std::min(W - 1, j + r) + 1; l++) {
                        if (Evaluation::is4ConnectedBoundaryPixel(gt, k, l)) {
                            pos = true;
                        }
                    }
                }

                if (!pos) {
                    // This is a false positive!
                    pre_rec.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 255, 0);
                }
            }
        }
    }
}

void Visualization::drawUndersegmentationError(const cv::Mat &image, const cv::Mat &labels, 
        const cv::Mat &gt, cv::Mat &ue)
{
    cv::Mat intersection_matrix;
    std::vector<int> superpixel_sizes;
    std::vector<int> gt_sizes;
    Evaluation::computeIntersectionMatrix(labels, gt, intersection_matrix, superpixel_sizes,
            gt_sizes);
    
    std::vector<int> superpixel_labels(superpixel_sizes.size(), 0);
    for (int j = 0; j < intersection_matrix.cols; ++j) {
        
        int max_intersection = 0;
        for (int i = 0; i < intersection_matrix.rows; ++i) {
            if (intersection_matrix.at<int>(i, j) > max_intersection) {
                max_intersection = intersection_matrix.at<int>(i, j);
                superpixel_labels[j] = i;
            }
        }
    }
    
    ue = image.clone ();
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int gt_label = gt.at<int>(i, j);
            int sp_label = labels.at<int>(i, j);
            
            if (gt_label != superpixel_labels[sp_label]) {
                ue.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
            }
        }
    }
}