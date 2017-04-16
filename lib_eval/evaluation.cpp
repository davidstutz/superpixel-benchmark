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

#include <limits>
#include <glog/logging.h>
#include "io_util.h"
#include "evaluation.h"

////////////////////////////////////////////////////////////////////////////////
// computeBoundaryRecall
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeBoundaryRecall(const cv::Mat &labels, 
        const cv::Mat &gt, float d) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int H = gt.rows;
    int W = gt.cols;
    
    int r = std::round(d*std::sqrt(H*H + W*W));
   
    float tp = 0;
    float fn = 0;

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (is4ConnectedBoundaryPixel(gt, i, j)) {

                bool pos = false;
                for (int k = std::max(0, i - r); k < std::min(H - 1, i + r) + 1; k++) {
                    for (int l = std::max(0, j - r); l < std::min(W - 1, j + r) + 1; l++) {
                        if (is4ConnectedBoundaryPixel(labels, k, l)) {
                            pos = true;
                        }
                    }
                }

                if (pos) {
                    tp++;
                }
                else {
                    fn++;
                }
            }
        }
    }
    
    if (tp + fn > 0) {
        return tp/(tp + fn);
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// computeBoundaryPrecision
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeBoundaryPrecision(const cv::Mat &labels, 
        const cv::Mat &gt, float d) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int H = gt.rows;
    int W = gt.cols;
    
    int r = std::round(d*std::sqrt(H*H + W*W));
        
    float tp = 0;
    float fp = 0;

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (is4ConnectedBoundaryPixel(gt, i, j)) {

                bool pos = false;
                // Search for boundary pixel in the supervoxel segmentation.
                for (int k = std::max(0, i - r); k < std::min(H - 1, i + r) + 1; k++) {
                    for (int l = std::max(0, j - r); l < std::min(W - 1, j + r) + 1; l++) {
                        if (is4ConnectedBoundaryPixel(labels, k, l)) {
                            pos = true;
                        }
                    }
                }

                if (pos) {
                    tp++;
                }
            }
            else if (is4ConnectedBoundaryPixel(labels, i, j)) {
                bool pos = false;
                // Search for boundary pixel in the supervoxel segmentation.
                for (int k = std::max(0, i - r); k < std::min(H - 1, i + r) + 1; k++) {
                    for (int l = std::max(0, j - r); l < std::min(W - 1, j + r) + 1; l++) {
                        if (is4ConnectedBoundaryPixel(gt, k, l)) {
                            pos = true;
                        }
                    }
                }

                if (!pos) {
                    fp++;
                }
            }
        }
    }

    if (tp + fp > 0) {
        return tp/(tp + fp);
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// computeUndersegmentationError
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeUndersegmentationError(const cv::Mat &labels, 
        const cv::Mat &gt) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int H = gt.rows;
    int W = gt.cols;
    int N = H*W;

    cv::Mat intersection_matrix;
    std::vector<int> superpixel_sizes;
    std::vector<int> gt_sizes;
    
    Evaluation::computeIntersectionMatrix(labels, gt, intersection_matrix, 
            superpixel_sizes, gt_sizes);

    float error = 0;
    for (int j = 0; j < intersection_matrix.cols; ++j) {

        int min = std::numeric_limits<int>::max();
        for (int i = 0; i < intersection_matrix.rows; ++i) {
            int superpixel_j_minus_gt_i = superpixel_sizes[j]
                    - intersection_matrix.at<int>(i, j);

            LOG_IF(FATAL, superpixel_j_minus_gt_i < 0) << "Set difference is negative.";
            if (superpixel_j_minus_gt_i < min) {
                min = superpixel_j_minus_gt_i;
            }
        }

        error += min;
    }

    return error/N;
}

////////////////////////////////////////////////////////////////////////////////
// computeOversegmentationError
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeOversegmentationError(const cv::Mat &labels, 
        const cv::Mat &gt) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int H = gt.rows;
    int W = gt.cols;
    int N = H*W;
        
    cv::Mat intersection_matrix;
    std::vector<int> superpixel_sizes;
    std::vector<int> gt_sizes;
    
    Evaluation::computeIntersectionMatrix(labels, gt, intersection_matrix, 
            superpixel_sizes, gt_sizes);
    
    float error = 0;
    for (int i = 0; i < intersection_matrix.rows; ++i) {

        int min = std::numeric_limits<int>::max();
        for (int j = 0; j < intersection_matrix.cols; ++j) {
            int gt_i_minus_superpixel_j = gt_sizes[i]
                    - intersection_matrix.at<int>(i, j);

            LOG_IF(FATAL, gt_i_minus_superpixel_j < 0) << "Set difference is negative.";
            if (gt_i_minus_superpixel_j < min) {
                min = gt_i_minus_superpixel_j;
            }
        }

        error += min;
    }
    
    return error /= N;
}

////////////////////////////////////////////////////////////////////////////////
// computeNPUndersegmentationError
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeNPUndersegmentationError(const cv::Mat &labels, 
        const cv::Mat &gt) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int N = gt.rows*gt.cols;
        
    cv::Mat intersection_matrix;
    std::vector<int> superpixel_sizes;
    std::vector<int> gt_sizes;

    Evaluation::computeIntersectionMatrix(labels, gt, 
            intersection_matrix, superpixel_sizes, gt_sizes);
    
    float error = 0;
    for (int i = 0; i < intersection_matrix.rows; ++i) {
        for (int j = 0; j < intersection_matrix.cols; ++j) {
            if (intersection_matrix.at<int>(i, j) > 0) {
                LOG_IF (ERROR, superpixel_sizes[j] - intersection_matrix.at<int>(i, j) < 0)
                        << "Invalid intersection computed, set difference is negative!";
                
                error += std::min(intersection_matrix.at<int>(i, j), 
                        superpixel_sizes[j] - intersection_matrix.at<int>(i, j));
            }
        }
    }

    return error/N;
}

////////////////////////////////////////////////////////////////////////////////
// computeLevinUndersegmentationError
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeLevinUndersegmentationError(const cv::Mat &labels, 
        const cv::Mat &gt) {
    
    cv::Mat intersection_matrix;
    std::vector<int> superpixel_sizes;
    std::vector<int> gt_sizes;

    Evaluation::computeIntersectionMatrix(labels, gt, 
            intersection_matrix, superpixel_sizes, gt_sizes);
    
    float error = 0;
    for (int i = 0; i < intersection_matrix.rows; i++) {
        
        float gt_error = 0;
        for (int j = 0; j < intersection_matrix.cols; j++) {
            if (intersection_matrix.at<int>(i, j) > 0) {
                gt_error += superpixel_sizes[j];
            }
        }
        
        gt_error -= gt_sizes[i];
        
        if (gt_sizes[i] > 0) {
            gt_error /= gt_sizes[i];
            error += gt_error;
        }
    }
    
    return error/gt_sizes.size();
}

////////////////////////////////////////////////////////////////////////////////
// computeAchievableSegmentationAccuracy
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeAchievableSegmentationAccuracy(const cv::Mat &labels, 
        const cv::Mat &gt) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int H = gt.rows;
    int W = gt.cols;
    int N = H*W;
        
    cv::Mat intersection_matrix;
    std::vector<int> superpixel_sizes;
    std::vector<int> gt_sizes;

    Evaluation::computeIntersectionMatrix(labels, gt, 
            intersection_matrix, superpixel_sizes, gt_sizes);

    float accuracy = 0;
    for (int j = 0; j < intersection_matrix.cols; ++j) {

        int max = 0;
        for (int i = 0; i < intersection_matrix.rows; ++i) {
            if (intersection_matrix.at<int>(i, j) > max) {
                max = intersection_matrix.at<int>(i, j);
            }
        }

        accuracy += max;
    }

    return accuracy/N;
}

////////////////////////////////////////////////////////////////////////////////
// computeSumOfSquaredErrorRGB
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeSumOfSquaredErrorRGB(const cv::Mat &labels,
        const cv::Mat &image) {
    
    LOG_IF(FATAL, image.channels() != 3) << "Currently only 3-channel images are supported.";
    LOG_IF(FATAL, labels.rows != image.rows || labels.cols != image.cols) 
            << "Superpixel segmentation does not match image size.";
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<cv::Vec3f> mean(superpixels, cv::Vec3f(0, 0, 0));
    std::vector<int> count(superpixels, 0);
    
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            for (int c = 0; c < image.channels(); ++c) {
                mean[labels.at<int>(i, j)][c] += image.at<cv::Vec3b>(i, j)[c];
            }
            
            count[labels.at<int>(i, j)]++;
        }
    }
    
    for (int k = 0; k < superpixels; k++) {
        mean[k] /= count[k];
    }
    
    float squared_sum = 0;
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            for (int c = 0; c < image.channels(); ++c) {
                squared_sum += (mean[labels.at<int>(i, j)][c] - image.at<cv::Vec3b>(i, j)[c])
                        * (mean[labels.at<int>(i, j)][c] - image.at<cv::Vec3b>(i, j)[c]);
            }
        }
    }
    
    return squared_sum/(image.rows*image.cols);
}

////////////////////////////////////////////////////////////////////////////////
// computeSumOfSquaredErrorXY
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeSumOfSquaredErrorXY(const cv::Mat &labels,
        const cv::Mat &image) {
    
    LOG_IF(FATAL, image.channels() != 3) << "Currently only 3-channel images are supported.";
    LOG_IF(FATAL, labels.rows != image.rows || labels.cols != image.cols) 
            << "Superpixel segmentation does not match image size.";
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<cv::Vec2f> mean(superpixels, cv::Vec2f(0, 0));
    std::vector<int> count(superpixels, 0);
    
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            mean[labels.at<int>(i, j)][0] += i;
            mean[labels.at<int>(i, j)][1] += j;
            count[labels.at<int>(i, j)]++;
        }
    }
    
    for (int k = 0; k < superpixels; k++) {
        mean[k] /= count[k];
    }
    
    float squared_sum = 0;
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            squared_sum += (mean[labels.at<int>(i, j)][0] - i)
                    * (mean[labels.at<int>(i, j)][0] - i);
            squared_sum += (mean[labels.at<int>(i, j)][1] - j)
                    * (mean[labels.at<int>(i, j)][1] - j);
        }
    }
    
    return squared_sum/(image.rows*image.cols);
}

////////////////////////////////////////////////////////////////////////////////
// computeExplainedVariation
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeExplainedVariation(const cv::Mat &labels,
        const cv::Mat &image) {
    
    LOG_IF(FATAL, image.channels() != 3) << "Currently only 3-channel images are supported.";
    LOG_IF(FATAL, labels.rows != image.rows || labels.cols != image.cols) 
            << "Superpixel segmentation does not match image size.";
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<cv::Vec3f> mean(superpixels, cv::Vec3f(0, 0, 0));
    std::vector<cv::Vec3f> squared_mean(superpixels, cv::Vec3f(0, 0, 0));
    std::vector<int> count(superpixels, 0);
    
    cv::Vec3f overall_mean = 0;
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            for (int c = 0; c < image.channels(); ++c) {
                mean[labels.at<int>(i, j)][c] += image.at<cv::Vec3b>(i, j)[c];
                overall_mean[c] += image.at<cv::Vec3b>(i, j)[c];
            }
            
            count[labels.at<int>(i, j)]++;
        }
    }
    
    for (int i = 0; i < superpixels; ++i) {
        for (int c = 0; c < image.channels(); ++c) {
            mean[i][c] /= count[i];
        }
    }
    
    overall_mean /= image.rows*image.cols;
    
    float sum_top = 0;
    float sum_bottom = 0;
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            for (int c = 0; c < image.channels(); ++c) {
                sum_top += (mean[labels.at<int>(i, j)][c] - overall_mean[c])
                        *(mean[labels.at<int>(i, j)][c] - overall_mean[c]);
                sum_bottom += (image.at<cv::Vec3b>(i, j)[c] - overall_mean[c])
                        *(image.at<cv::Vec3b>(i, j)[c] - overall_mean[c]);
            }
        }
    }
    
    return sum_top/sum_bottom;
}

////////////////////////////////////////////////////////////////////////////////
// computeMeanDistanceToEdge
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeMeanDistanceToEdge(const cv::Mat &labels,
        const cv::Mat &gt) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    cv::Mat boundary(labels.rows, labels.cols, CV_8UC1, cv::Scalar(1));
    cv::Mat gt_boundary(gt.rows, gt.cols, CV_8UC1, cv::Scalar(0));
    
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (is4ConnectedBoundaryPixel(labels, i, j)) {
                boundary.at<unsigned char>(i, j) = 0;
            }
            if (is4ConnectedBoundaryPixel(gt, i, j)) {
                gt_boundary.at<unsigned char>(i, j) = 1;
            }
        }
    }
    
    cv::Mat distance;
    cv::distanceTransform(boundary, distance, CV_DIST_L2, 3);
    
    float mean_distance_edge = 0;
    int count = 0;
    
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (gt_boundary.at<unsigned char>(i, j) > 0) {
                mean_distance_edge += distance.at<float>(i, j);
                count++;
            }
        }
    }
    
    return mean_distance_edge/count;
}

////////////////////////////////////////////////////////////////////////////////
// computeIntraClusterVariation
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeIntraClusterVariation(const cv::Mat &labels, 
        const cv::Mat &image) {
    
    LOG_IF(FATAL, image.channels() != 3) << "Currently only 3-channel images are supported.";
    LOG_IF(FATAL, labels.rows != image.rows || labels.cols != image.cols) 
            << "Superpixel segmentation does not match image size.";
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<cv::Vec3f> mean(superpixels, cv::Vec3f(0, 0, 0));
    std::vector<int> count(superpixels, 0);
    
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            for (int c = 0; c < image.channels(); ++c) {
                mean[labels.at<int>(i, j)][c] += image.at<cv::Vec3b>(i, j)[c];
            }
            
            ++count[labels.at<int>(i, j)];
        }
    }
    
    for (int i = 0; i < superpixels; ++i) {
        for (int c = 0; c < image.channels(); ++c) {
            if (count[i] > 0) {
                mean[i][c] /= count[i];
            }
        }
    }
    
    std::vector<float> variance(superpixels, 0);
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            for (int c = 0; c < image.channels(); ++c) {
                variance[labels.at<int>(i, j)] += (image.at<cv::Vec3b>(i, j)[c] - mean[labels.at<int>(i, j)][c])
                        * (image.at<cv::Vec3b>(i, j)[c] - mean[labels.at<int>(i, j)][c]);
            }
        }
    }
    
    for (int i = 0; i < superpixels; ++i) {
        if (count[i] > 0) {
            variance[i] /= count[i];
            variance[i] = std::sqrt(variance[i]);
        }
    }
    
    float sum = 0;
    for (int i = 0; i < superpixels; ++i) {
        sum += variance[i];
    }
    
    if (superpixels > 0) {
        return sum/superpixels;
    }
    
    return sum;
}

////////////////////////////////////////////////////////////////////////////////
// computeCompactness
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeCompactness(const cv::Mat &labels) {
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<float> perimeter(superpixels, 0);
//    std::vector<float> old_perimeter(superpixels, 0);
    std::vector<float> area(superpixels, 0); // = number of pixels!
    
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            
            int count = 0;
            if (i > 0) {
                if (labels.at<int>(i, j) != labels.at<int>(i - 1, j)) {
                    count++;
                }
            }
            else {
                count++;
            }

            if (i < labels.rows - 1) {
                if (labels.at<int>(i, j) != labels.at<int>(i + 1, j)) {
                    count++;
                }
            }
            else {
                count++;
            }

            if (j > 0) {
                if (labels.at<int>(i, j) != labels.at<int>(i, j - 1)) {
                    count++;
                }
            }
            else {
                count++;
            }

            if (j < labels.cols - 1) {
                if (labels.at<int>(i, j) != labels.at<int>(i, j + 1)) {
                    count++;
                }
            }
            else {
                count++;
            }
            
            perimeter[labels.at<int>(i, j)] += count;
            area[labels.at<int>(i, j)] += 1;
        }
    }
    
    float compactness = 0;
//    float old_compactness = 0;
    
    for (int i = 0; i < superpixels; ++i) {
        if (perimeter[i] > 0) {
            compactness += area[i] * (4*M_PI*area[i])/(perimeter[i]*perimeter[i]);
        }
//        old_compactness += area[i] * (4*M_PI*area[i])/(old_perimeter[i]*old_perimeter[i]);
    }
    
    compactness /= labels.rows*labels.cols;
//    old_compactness /= labels.rows*labels.cols;
    LOG_IF (ERROR, compactness > 1.0f) 
            << "Invalid compactness: " << compactness;
    
    return compactness;
}

////////////////////////////////////////////////////////////////////////////////
// computeContourDensity
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeContourDensity(const cv::Mat &labels) {
    
    int contour = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (is4ConnectedBoundaryPixel(labels, i, j)) {
                ++contour;
            }
        }
    }
    
    return contour/((float) (labels.rows*labels.cols));
}

////////////////////////////////////////////////////////////////////////////////
// computeGradientMagnitude
////////////////////////////////////////////////////////////////////////////////

void computeGradientMagnitude(const cv::Mat &image, cv::Mat &gradient_magnitude) {
    cv::Mat image_gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, image_gray, CV_BGR2GRAY);
    }
    else {
        image_gray = image.clone();
    }
    
    cv::Mat grad_x;
    cv::Mat grad_y;

    cv::Sobel(image_gray, grad_x, CV_32F, 1, 0, 3);
    cv::convertScaleAbs(grad_x, grad_x);

    cv::Sobel(image_gray,grad_y, CV_32F, 0, 1, 3);
    cv::convertScaleAbs(grad_y, grad_y);

    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, gradient_magnitude);
    
    float max = 0;
    for (int i = 0; i < gradient_magnitude.rows; ++i) {
        for (int j = 0; j < gradient_magnitude.cols; ++j) {
            if (gradient_magnitude.at<float>(i, j) > max) {
                max = gradient_magnitude.at<float>(i, j);
            }
        }
    }
    
    gradient_magnitude /= max;
}

////////////////////////////////////////////////////////////////////////////////
// computeCannyEdges
////////////////////////////////////////////////////////////////////////////////

void computeCannyEdges(const cv::Mat &image, float threshold, cv::Mat &canny) {
    cv::Mat image_gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, image_gray, CV_BGR2GRAY);
    }
    else {
        image_gray = image.clone();
    }
    
    cv::Canny(image_gray, canny, threshold, 2*threshold, 3);
}

////////////////////////////////////////////////////////////////////////////////
// computeRegularity
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeRegularity(const cv::Mat &labels) {
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    int H = labels.rows;
    int W = labels.rows;
    int region_size = std::sqrt((labels.rows*labels.cols)/superpixels);
    
    int number_H = H/region_size;
    int number_W = W/region_size;
    
    int boundary_grid = number_H*W + number_W*H - number_H*number_W;
    
    int boundary_sp = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (is4ConnectedBoundaryPixel(labels, i, j)) {
                ++boundary_sp;
            }
        }
    }
    
    return boundary_grid/((float) boundary_sp);
}

////////////////////////////////////////////////////////////////////////////////
// computeSuperpixels
////////////////////////////////////////////////////////////////////////////////

int Evaluation::computeSuperpixels(const cv::Mat &labels) {
    int max = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > max) {
                max = labels.at<int>(i, j);
            }
        }
    }
    
    std::vector<int> superpixels(max + 1, 0);
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            LOG_IF (FATAL, labels.at<int>(i, j) > max) << labels.at<int>(i, j);
            superpixels[labels.at<int>(i, j)] = 1;
        }
    }
    
    int sum = 0;
    for (unsigned int i = 0; i < superpixels.size(); i++) {
        sum += superpixels[i];
    }
    
    return sum;
}

////////////////////////////////////////////////////////////////////////////////
// computeSuperpixelSizes
////////////////////////////////////////////////////////////////////////////////

void Evaluation::computeSuperpixelSizes(const cv::Mat& labels, float& average_size, 
        int& min_size, int& max_size, float &size_variation) {
    
    int max_label = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > max_label) {
                max_label = labels.at<int>(i, j);
            }
        }
    }
    
    std::vector<unsigned long long int> counts(max_label + 1, 0);
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            LOG_IF (FATAL, labels.at<int>(i, j) > max_label) << labels.at<int>(i, j);
            counts[labels.at<int>(i, j)]++;
        }
    }
    
    unsigned long long int sum = 0;
    unsigned long long int squared_sum = 0;
    int superpixels = 0;
    
    min_size = std::numeric_limits<int>::max();
    max_size = 0;
    
    for (unsigned int k = 0; k < counts.size(); k++) {
        if (counts[k] > 0) {
            superpixels++;
            
            sum += counts[k];
            squared_sum += counts[k]*counts[k];
            
            if (counts[k] < min_size) {
                min_size = counts[k];
            }
            if (counts[k] > max_size) {
                max_size = counts[k];
            }
        }
    }
    
    if (superpixels > 0) {
        average_size = (double) sum / superpixels;
        
        float variance = (double) squared_sum / superpixels - (double) sum / superpixels * (double) sum / superpixels;
        LOG_IF(ERROR, variance < 0) << "Invalid variance: " << variance <<  " " << squared_sum << " " << sum << " " << superpixels;
        size_variation = std::sqrt(variance);
    }
}

////////////////////////////////////////////////////////////////////////////////
// computeEdgeRecall
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeEdgeRecall(const cv::Mat &labels, const cv::Mat &edges,
        float d) {
    LOG_IF(FATAL, labels.rows != edges.rows || labels.cols != edges.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int H = edges.rows;
    int W = edges.cols;
    
    int r = std::round(d*std::sqrt(H*H + W*W));
   
    float tp = 0;
    float fn = 0;

//    double min;
//    double max;
//    cv::minMaxLoc(edges, &min, &max);
    
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (edges.at<unsigned char>(i, j) > 100) {

                bool pos = false;
                for (int k = std::max(0, i - r); k < std::min(H - 1, i + r) + 1; k++) {
                    for (int l = std::max(0, j - r); l < std::min(W - 1, j + r) + 1; l++) {
                        if (is4ConnectedBoundaryPixel(labels, k, l)) {
                            pos = true;
                        }
                    }
                }

                if (pos) {
                    tp++;
//                    tp += pos/255;
                }
                else {
//                    fn++;
                    fn += ((float) edges.at<unsigned char>(i, j))/255;
                }
            }
        }
    }
    
    if (tp + fn > 0) {
        return tp/(tp + fn);
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// computeAverageMetric
////////////////////////////////////////////////////////////////////////////////

float Evaluation::computeAverageMetric(const std::vector<float>& values, const std::vector<float>& superpixels,
        int min_superpixels, int max_superpixels) {
    
    LOG_IF(FATAL, values.size() != superpixels.size()) 
            << "Number of metric values does not match number of numbers of superpixels.";
    
    std::vector<int> indices(values.size());
    for (unsigned int i = 0; i < values.size(); i++) {
        indices[i] = i;
    }
    
    std::sort(std::begin(indices), std::end(indices), 
            [&](int i1, int i2) {return superpixels[i1] < superpixels[i2];});
            
    int i_min = 0;
    while (superpixels[indices[i_min + 1]] <= min_superpixels) {
        i_min++;
    }
    
    int i_max = indices.size() - 1;
    while (superpixels[indices[i_max - 1]] >= max_superpixels) {
        i_max--;
    }
    
    std::vector<float> corrected_values;
    std::vector<int> corrected_superpixels;
    
    if (superpixels[indices[i_min]] > min_superpixels) {
        corrected_values.push_back(values[indices[i_min]]);
        corrected_superpixels.push_back(min_superpixels);
    }
    else {        
        float m = (values[indices[i_min + 1]] - values[indices[i_min]])/(superpixels[indices[i_min + 1]] - superpixels[indices[i_min]]);
        float n = values[indices[i_min]] - m*superpixels[indices[i_min]];
        
        corrected_values.push_back(m*min_superpixels + n);
        corrected_superpixels.push_back(min_superpixels);
        i_min++;
    }
    
    for (unsigned int i = i_min; i < i_max - 1; i++) {
        corrected_values.push_back(values[indices[i]]);
        corrected_superpixels.push_back(superpixels[indices[i]]);
    }
    
    if (superpixels[indices[i_max]] < max_superpixels) {
        corrected_values.push_back(values[indices[i_max]]);
        corrected_superpixels.push_back(max_superpixels);
    }
    else {        
        int end = corrected_superpixels.size() - 1;
        float m = (corrected_values[end] - corrected_values[end - 1])/(corrected_superpixels[end] - corrected_superpixels[end - 1]);
        float n = corrected_values[end] - m*corrected_superpixels[end];
        
        corrected_values[end] = m*max_superpixels + n;
        corrected_superpixels[end] = max_superpixels;
    }
    
    float average = 0;
    for (unsigned int i = 1; i < corrected_values.size(); i++) {
        
        float a = 0;
        float b = 0;
        
        if (corrected_values[i] > corrected_values[i - 1]) {
            a = (1 - corrected_values[i])*(corrected_superpixels[i] - corrected_superpixels[i - 1]);
            b = 0.5*(corrected_values[i] - corrected_values[i - 1])*(corrected_superpixels[i] - corrected_superpixels[i - 1]);
        }
        else {
            a = (1 - corrected_values[i - 1])*(corrected_superpixels[i] - corrected_superpixels[i - 1]);
            b = 0.5*(corrected_values[i - 1] - corrected_values[i])*(corrected_superpixels[i] - corrected_superpixels[i - 1]);
        }
        
        average += a + b;
    }
    
    return 100*average/(max_superpixels - min_superpixels);
}

////////////////////////////////////////////////////////////////////////////////
// computeBoundingBoxes
////////////////////////////////////////////////////////////////////////////////

void Evaluation::computeBoundingBoxes(const cv::Mat &labels, 
        std::vector<cv::Rect> &rectangles) {
    
    int superpixels = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    
    std::vector<int> min_i(superpixels, std::numeric_limits<int>::max());
    std::vector<int> max_i(superpixels, std::numeric_limits<int>::min());
    std::vector<int> min_j(superpixels, std::numeric_limits<int>::max());
    std::vector<int> max_j(superpixels, std::numeric_limits<int>::min());
    
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            int label = labels.at<int>(i, j);
            
            if (i < min_i[label]) {
                min_i[label] = i;
            }
            
            if (i > max_i[label]) {
                max_i[label] = i;
            }
            
            if (j < min_j[label]) {
                min_j[label] = j;
            }
            
            if (j > max_j[label]) {
                max_j[label] = j;
            }
        }
    }
    
    rectangles.resize(superpixels);
    for (int i = 0; i < superpixels; ++i) {
        
        int width = max_j[i] - min_j[i];
        int height = max_i[i] - min_i[i];
        
        cv::Rect rectangle(min_j[i], min_i[i], width, height);
        rectangles.push_back(rectangle);
    }
}

////////////////////////////////////////////////////////////////////////////////
// computeIntersectionMatrix
////////////////////////////////////////////////////////////////////////////////

void Evaluation::computeIntersectionMatrix(const cv::Mat &labels, const cv::Mat &gt,
        cv::Mat &intersection_matrix, std::vector<int> &superpixel_sizes, std::vector<int> &gt_sizes) {
    
    LOG_IF(FATAL, labels.rows != gt.rows || labels.cols != gt.cols) 
            << "Superpixel segmentation does not match ground truth size.";
    
    int superpixels = 0;
    int gt_segments = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > superpixels) {
                superpixels = labels.at<int>(i, j);
            }
            if (gt.at<int>(i, j) > gt_segments) {
                gt_segments = gt.at<int>(i, j);
            }
        }
    }
    
    superpixels++;
    gt_segments++;
    
    superpixel_sizes.resize(superpixels, 0);
    gt_sizes.resize(gt_segments, 0);
    
    intersection_matrix.create(gt_segments, superpixels, CV_32SC1);
    
    // Rember to reset as not done in create.
    for (int i = 0; i < intersection_matrix.rows; ++i) {
        for (int j = 0; j < intersection_matrix.cols; ++j) {
            intersection_matrix.at<int>(i, j) = 0;
        }
    }
    
    for (int i = 0; i < gt.rows; ++i) {
        for (int j = 0; j < gt.cols; ++j) {
            intersection_matrix.at<int>(gt.at<int>(i, j), labels.at<int>(i, j))++;
            superpixel_sizes[labels.at<int>(i, j)]++;
            gt_sizes[gt.at<int>(i, j)]++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// is4ConnectedBoundaryPixel
////////////////////////////////////////////////////////////////////////////////

bool Evaluation::is4ConnectedBoundaryPixel(const cv::Mat &labels, int i, int j) {
    
    if (i > 0) {
        if (labels.at<int>(i, j) != labels.at<int>(i - 1, j)) {
            return true;
        }
    }
    
    if (i < labels.rows - 1) {
        if (labels.at<int>(i, j) != labels.at<int>(i + 1, j)) {
            return true;
        }
    }
    
    if (j > 0) {
        if (labels.at<int>(i, j) != labels.at<int>(i, j - 1)) {
            return true;
        }
    }
    
    if (j < labels.cols - 1) {
        if (labels.at<int>(i, j) != labels.at<int>(i, j + 1)) {
            return true;
        }
    }
    
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// is8Minus4ConnectedBoundaryPixel
////////////////////////////////////////////////////////////////////////////////

bool Evaluation::is8Minus4ConnectedBoundaryPixel(const cv::Mat &labels, int i, int j) {
    if (i < labels.rows - 1 && j < labels.cols - 1) {
        if (labels.at<int>(i, j) != labels.at<int>(i + 1, j + 1)) {
            return true;
        }
    }
    
    if (i < labels.rows - 1 && j > 0) {
        if (labels.at<int>(i, j) != labels.at<int>(i + 1, j - 1)) {
            return true;
        }
    }
    
    if (i > 0 && j < labels.cols - 1) {
        if (labels.at<int>(i, j) != labels.at<int>(i - 1, j + 1)) {
            return true;
        }
    }
    
    if (i > 0 && j > 0) {
        if (labels.at<int>(i, j) != labels.at<int>(i - 1, j - 1)) {
            return true;
        }
    }
    
    return false;
}