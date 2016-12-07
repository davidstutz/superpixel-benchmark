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
#include "connected_components.h"
#include "superpixel_tools.h"

////////////////////////////////////////////////////////////////////////////////
// computeRegionSizeFromSuperpixels
////////////////////////////////////////////////////////////////////////////////

int SuperpixelTools::computeRegionSizeFromSuperpixels(const cv::Mat &image, 
        int superpixels) {
    
    // Copied from SLIC.cpp!
    return (int) (0.5f + std::sqrt(image.rows*image.cols / (float) superpixels));
}

////////////////////////////////////////////////////////////////////////////////
// computeHeightWidthFromSuperpixels
////////////////////////////////////////////////////////////////////////////////

void SuperpixelTools::computeHeightWidthFromSuperpixels(const cv::Mat &image, 
        int superpixels, int &height, int &width) {
    
    int s = image.rows*image.cols/ superpixels;
    height = 0.5f + std::sqrt(s*image.rows/ (float) image.cols);
    width = 0.5f + s / (float) height;
}

////////////////////////////////////////////////////////////////////////////////
// computeRegionSizeLevels
////////////////////////////////////////////////////////////////////////////////

void SuperpixelTools::computeRegionSizeLevels(const cv::Mat &image, 
        int superpixels, int &region_size, int &levels) {

//    float ratio = image.cols/ (float) image.rows;

    int max_region_size = 20;
    int max_levels = 12;

    int min_difference = -1;
    levels = 1;
    region_size = 2;

    for (int r = 2; r <= max_region_size; ++r) {
        for (int l = 2; l <= max_levels; ++l) {
            int computed_superpixels = std::floor(image.cols/(r*pow(2, l - 1))) 
                    * std::floor(image.rows/(r*pow(2, l - 1)));

            int difference = abs(superpixels - computed_superpixels);
            if (difference < min_difference || min_difference < 0) {
                
                min_difference = difference;
                levels = l;
                region_size = r;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// computeHeightWidthLevelsFromSuperpixels
////////////////////////////////////////////////////////////////////////////////

void SuperpixelTools::computeHeightWidthLevelsFromSuperpixels(const cv::Mat &image, 
        int superpixels, int &height, int &width, int &levels) {

//    float ratio = image.cols/ (float) image.rows;

    int max_width = 20;
    int max_height = 20;
    int max_levels = 12;

    int min_difference = -1;
    levels = 1;
    width = 2;
    height = 2;

    for (int w = 2; w <= max_width; ++w) {
        for (int h = 2; h <= max_height; ++h) {
//            if (w/ (float) h > ratio) {
//                continue;
//            }

            for (int l = 1; l <= max_levels; ++l) {
                int computed_superpixels = std::floor(image.cols/(w*pow(2, l - 1))) 
                        * std::floor(image.rows/(h*pow(2, l - 1)));
                
                int difference = abs(superpixels - computed_superpixels);
                if (difference < min_difference || min_difference < 0) {
                    min_difference = difference;
                    levels = l;
                    width = w;
                    height = h;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// relabelSuperpixels
////////////////////////////////////////////////////////////////////////////////

void SuperpixelTools::relabelSuperpixels(cv::Mat &labels) {
    
    int max_label = 0;
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            if (labels.at<int>(i, j) > max_label) {
                max_label = labels.at<int>(i, j);
            }
        }
    }
    
    int current_label = 0;
    std::vector<int> label_correspondence(max_label + 1, -1);
    
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            int label = labels.at<int>(i, j);
            
            if (label_correspondence[label] < 0) {
                label_correspondence[label] = current_label++;
            }
            
            labels.at<int>(i, j) = label_correspondence[label];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// computeDistance
////////////////////////////////////////////////////////////////////////////////

float computeDistance(const cv::Vec3b &x, const cv::Vec3b &y) {
    return (x[0] - y[0])*(x[0] - y[0])
            + (x[1] - y[1])*(x[1] - y[1])
            + (x[2] - y[2])*(x[2] - y[2]);
}

////////////////////////////////////////////////////////////////////////////////
// computeLabelsFromBoundaries
////////////////////////////////////////////////////////////////////////////////

void SuperpixelTools::computeLabelsFromBoundaries(const cv::Mat &image, 
        const cv::Mat &boundaries, cv::Mat &labels, int BOUNDARY_VALUE,
        int INNER_VALUE) {
    
    LOG_IF(FATAL, image.rows != boundaries.rows) 
            << "Image size does not match boundaries size: " 
            << image.rows << "," << image.cols << " != " << boundaries.rows << "," << boundaries.cols;
    LOG_IF(FATAL, image.cols != boundaries.cols) 
            << "Image size does not match boundaries size: " 
            << image.rows << "," << image.cols << " != " << boundaries.rows << "," << boundaries.cols;
    LOG_IF(FATAL, boundaries.type() != CV_32S) << "Invalid boundaries type!";
    
    cv::Mat tmp_labels(boundaries.rows, boundaries.cols, CV_32FC1, cv::Scalar(0));
    for (int i = 0; i < tmp_labels.rows; i++) {
        for (int j = 0; j < tmp_labels.cols; j++) {
            if (boundaries.at<int>(i, j) > 0) {
                tmp_labels.at<float>(i,j) = BOUNDARY_VALUE;
            }
            else {
                tmp_labels.at<float>(i,j) = INNER_VALUE;
            }
        }
    }
    
    int label = 1;
    for (int i = 0; i < tmp_labels.rows; i++) {
        for (int j = 0; j < tmp_labels.cols; j++) {
            if (tmp_labels.at<float>(i, j) == INNER_VALUE) {
                cv::floodFill(tmp_labels, cv::Point(j, i), cv::Scalar(label), 
                        0, cv::Scalar(0), cv::Scalar(0));
                label++;
            }
        }
    }
    
    tmp_labels.convertTo(tmp_labels, CV_32SC1);
    std::vector<cv::Vec3b> means(label + 1, cv::Vec3b(0, 0, 0));
    std::vector<int> counts(label + 1, 0);
    
    for (int i = 0; i < tmp_labels.rows; i++) {
        for (int j = 0; j < tmp_labels.cols; j++) {
            if (tmp_labels.at<int>(i, j) != BOUNDARY_VALUE) {
                means[tmp_labels.at<int>(i, j) - 1][0] += image.at<cv::Vec3b>(i, j)[0];
                means[tmp_labels.at<int>(i, j) - 1][1] += image.at<cv::Vec3b>(i, j)[1];
                means[tmp_labels.at<int>(i, j) - 1][2] += image.at<cv::Vec3b>(i, j)[2];
                counts[tmp_labels.at<int>(i, j) - 1]++;
            }
        }
    }
    
    for (int k = 0; k < label + 1; k++) {
        if (counts[k] > 0) {
            means[k][0] /= counts[k];
            means[k][1] /= counts[k];
            means[k][2] /= counts[k];
        }
    }
    
    labels.create(image.rows, image.cols, CV_32SC1);
    for (int i = 0; i < tmp_labels.rows; i++) {
        for (int j = 0; j < tmp_labels.cols; j++) {
            if (tmp_labels.at<int>(i, j) == BOUNDARY_VALUE) {

                int min_label = BOUNDARY_VALUE;
                float min_distance = std::numeric_limits<float>::max();

                if (i + 1 < image.rows && tmp_labels.at<int>(i + 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[tmp_labels.at<int>(i + 1, j) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = tmp_labels.at<int>(i + 1, j);
                    }
                }

                if (j + 1 < image.cols && tmp_labels.at<int>(i, j + 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[tmp_labels.at<int>(i, j + 1) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = tmp_labels.at<int>(i, j + 1);
                    }
                }

                if (i > 0 && tmp_labels.at<int>(i - 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[tmp_labels.at<int>(i - 1, j) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = tmp_labels.at<int>(i - 1, j);
                    }
                }

                if (j > 0 && tmp_labels.at<int>(i, j - 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[tmp_labels.at<int>(i, j - 1) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = tmp_labels.at<int>(i, j - 1);
                    }
                }

                labels.at<int>(i, j) = min_label;
            }
            else {
                labels.at<int>(i, j) = tmp_labels.at<int>(i, j);
            }
        }
    }
    
    // Second pass to resolve diagonal issues.
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            if (labels.at<int>(i, j) == BOUNDARY_VALUE) {

                int min_label = BOUNDARY_VALUE;
                float min_distance = std::numeric_limits<float>::max();

                if (i + 1 < image.rows && labels.at<int>(i + 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i + 1, j) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i + 1, j);
                    }
                }

                if (j + 1 < image.cols && labels.at<int>(i, j + 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i, j + 1) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i, j + 1);
                    }
                }

                if (i > 0 && labels.at<int>(i - 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i - 1, j) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i - 1, j);
                    }
                }

                if (j > 0 && labels.at<int>(i, j - 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i, j - 1) - 1]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i, j - 1);
                    }
                }

                labels.at<int>(i, j) = min_label;
            }
        }
    }
    
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            labels.at<int>(i, j)--;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// assignBoundariesToSuperpixels
////////////////////////////////////////////////////////////////////////////////

void SuperpixelTools::assignBoundariesToSuperpixels(const cv::Mat &image, 
        const cv::Mat &boundaries, cv::Mat &labels, int BOUNDARY_VALUE) {
    
    int label = 0;
    for (int i = 0; i < boundaries.rows; i++) {
        for (int j = 0; j < boundaries.cols; j++) {
            if (boundaries.at<int>(i, j) > label) {
                label = boundaries.at<int>(i, j);
            }
        }
    }
    
    std::vector<cv::Vec3b> means(label + 1, cv::Vec3b(0, 0, 0));
    std::vector<int> counts(label + 1, 0);
    
    for (int i = 0; i < boundaries.rows; i++) {
        for (int j = 0; j < boundaries.cols; j++) {
            if (boundaries.at<int>(i, j) != BOUNDARY_VALUE) {
                means[boundaries.at<int>(i, j)][0] += image.at<cv::Vec3b>(i, j)[0];
                means[boundaries.at<int>(i, j)][1] += image.at<cv::Vec3b>(i, j)[1];
                means[boundaries.at<int>(i, j)][2] += image.at<cv::Vec3b>(i, j)[2];
                counts[boundaries.at<int>(i, j)]++;
            }
        }
    }
    
    for (int k = 0; k < label + 1; k++) {
        if (counts[k] > 0) {
            means[k][0] /= counts[k];
            means[k][1] /= counts[k];
            means[k][2] /= counts[k];
        }
    }
    
    labels.create(image.rows, image.cols, CV_32SC1);
    for (int i = 0; i < boundaries.rows; i++) {
        for (int j = 0; j < boundaries.cols; j++) {
            if (boundaries.at<int>(i, j) == BOUNDARY_VALUE) {

                int min_label = BOUNDARY_VALUE;
                float min_distance = std::numeric_limits<float>::max();

                if (i + 1 < image.rows && boundaries.at<int>(i + 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[boundaries.at<int>(i + 1, j)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = boundaries.at<int>(i + 1, j);
                    }
                }

                if (j + 1 < image.cols && boundaries.at<int>(i, j + 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[boundaries.at<int>(i, j + 1)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = boundaries.at<int>(i, j + 1);
                    }
                }

                if (i > 0 && boundaries.at<int>(i - 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[boundaries.at<int>(i - 1, j)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = boundaries.at<int>(i - 1, j);
                    }
                }

                if (j > 0 && boundaries.at<int>(i, j - 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[boundaries.at<int>(i, j - 1)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = boundaries.at<int>(i, j - 1);
                    }
                }

                labels.at<int>(i, j) = min_label;
            }
            else {
                labels.at<int>(i, j) = boundaries.at<int>(i, j);
            }
        }
    }
    
    // Second pass to resolve diagonal issues.
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            if (labels.at<int>(i, j) == BOUNDARY_VALUE) {

                int min_label = BOUNDARY_VALUE;
                float min_distance = std::numeric_limits<float>::max();

                if (i + 1 < image.rows && labels.at<int>(i + 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i + 1, j)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i + 1, j);
                    }
                }

                if (j + 1 < image.cols && labels.at<int>(i, j + 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i, j + 1)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i, j + 1);
                    }
                }

                if (i > 0 && labels.at<int>(i - 1, j) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i - 1, j)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i - 1, j);
                    }
                }

                if (j > 0 && labels.at<int>(i, j - 1) != BOUNDARY_VALUE) {
                    float distance = computeDistance(image.at<cv::Vec3b>(i, j), means[labels.at<int>(i, j - 1)]);

                    if (distance < min_distance) {
                        min_distance = distance;
                        min_label = labels.at<int>(i, j - 1);
                    }
                }

                labels.at<int>(i, j) = min_label;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// countSuperpixels
////////////////////////////////////////////////////////////////////////////////

int SuperpixelTools::countSuperpixels(const cv::Mat &labels) {
    
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
            LOG_IF(FATAL, labels.at<int>(i, j) > max) << "Invalid label: " << labels.at<int>(i, j);
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
// relabelConnectedSuperpixels
////////////////////////////////////////////////////////////////////////////////

int SuperpixelTools::relabelConnectedSuperpixels(cv::Mat &labels) {
    LOG_IF(FATAL, labels.type() != CV_32SC1) << "Invalid label type.";
    
    SuperpixelTools::relabelSuperpixels(labels);
    
    int max = 0;
    for (int i = 0; i < labels.rows; ++i) {
        for (int j = 0; j < labels.cols; ++j) {
            if (labels.at<int>(i, j) > max) {
                max = labels.at<int>(i, j);
            }
        }
    }
    
    ConnectedComponents cc(2*max);
            
    cv::Mat components(labels.rows, labels.cols, CV_32SC1, cv::Scalar(0));
    int component_count = cc.connected<int, int, std::equal_to<int>, bool>((int*) labels.data, (int*) components.data, labels.cols, 
            labels.rows, std::equal_to<int>(), false);
    
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            labels.at<int>(i, j) = components.at<int>(i, j);
        }
    }
    
    // component_count would be the NEXT label index, max is the current highest!
    return component_count - max - 1;
}

////////////////////////////////////////////////////////////////////////////////
// enforceMinimumSuperpixelSize
////////////////////////////////////////////////////////////////////////////////

int SuperpixelTools::enforceMinimumSuperpixelSize(const cv::Mat &image, cv::Mat &labels, int size) {
    int max_label = 0;
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            if (labels.at<int>(i, j) > max_label) {
                max_label = labels.at<int>(i, j);
            }
        }
    }
    
    std::vector<cv::Vec3b> means(max_label + 1, cv::Vec3b(0, 0, 0));
    std::vector<int> counts(max_label + 1, 0);
    
    std::vector< std::vector<int> > neighbors(max_label + 1);
    for (unsigned int k = 0; k < neighbors.size(); k++) {
        neighbors[k] = std::vector<int>(0);
    }
    
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            int label = labels.at<int>(i, j);
            
            means[label][0] += image.at<cv::Vec3b>(i, j)[0];
            means[label][1] += image.at<cv::Vec3b>(i, j)[1];
            means[label][2] += image.at<cv::Vec3b>(i, j)[2];
            counts[label]++;
            
            int neighbor_label = labels.at<int>(std::min(i + 1, labels.rows - 1), j);
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
            
            neighbor_label = labels.at<int>(std::max(0, i - 1), j);
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
            
            neighbor_label = labels.at<int>(i, std::min(j + 1, labels.cols - 1));
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
            
            neighbor_label = labels.at<int>(i, std::max(0, j - 1));
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
        }
    }
    
    for (unsigned int k = 0; k < counts.size(); k++) {
        if (counts[k] > 0) {
            means[k][0] /= counts[k];
            means[k][1] /= counts[k];
            means[k][2] /= counts[k];
        }
    }
    
    int count = 0;
    std::vector<int> new_labels(max_label + 1, -1);
    
    for (unsigned int k = 0; k < counts.size(); k++) {
        if (counts[k] < size) {
            
            float min_distance = std::numeric_limits<float>::max();
            for (unsigned int kk = 0; kk < neighbors[k].size(); kk++) {
                float distance = (means[k][0] - means[neighbors[k][kk]][0])*(means[k][0] - means[neighbors[k][kk]][0])
                        + (means[k][1] - means[neighbors[k][kk]][1])*(means[k][1] - means[neighbors[k][kk]][1])
                        + (means[k][2] - means[neighbors[k][kk]][2])*(means[k][2] - means[neighbors[k][kk]][2]);
                
                if (distance < min_distance && new_labels[neighbors[k][kk]] < 0) {
                    min_distance = distance;
                    new_labels[k] = neighbors[k][kk];
                }
            }
            
            count++;
        }
    }
    
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            
            int label = labels.at<int>(i, j);
            if (new_labels[label] >= 0) {
                
                int new_label = new_labels[label];
                while (new_labels[new_label] >= 0) {
                    new_label = new_labels[new_label];
                }
                
                labels.at<int>(i, j) = new_labels[label];
            }
        }
    }
    
    return count;
}

////////////////////////////////////////////////////////////////////////////////
// enforceMinimumSuperpixelSizeUpTo
////////////////////////////////////////////////////////////////////////////////

int SuperpixelTools::enforceMinimumSuperpixelSizeUpTo(const cv::Mat &image, cv::Mat &labels, int number) {
    int max_label = 0;
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            if (labels.at<int>(i, j) > max_label) {
                max_label = labels.at<int>(i, j);
            }
        }
    }
    
    std::vector<cv::Vec3b> means(max_label + 1, cv::Vec3b(0, 0, 0));
    std::vector<int> counts(max_label + 1, 0);
    
    std::vector< std::vector<int> > neighbors(max_label + 1);
    for (unsigned int k = 0; k < neighbors.size(); k++) {
        neighbors[k] = std::vector<int>(0);
    }
    
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            int label = labels.at<int>(i, j);
            
            means[label][0] += image.at<cv::Vec3b>(i, j)[0];
            means[label][1] += image.at<cv::Vec3b>(i, j)[1];
            means[label][2] += image.at<cv::Vec3b>(i, j)[2];
            counts[label]++;
            
            int neighbor_label = labels.at<int>(std::min(i + 1, labels.rows - 1), j);
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
            
            neighbor_label = labels.at<int>(std::max(0, i - 1), j);
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
            
            neighbor_label = labels.at<int>(i, std::min(j + 1, labels.cols - 1));
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
            
            neighbor_label = labels.at<int>(i, std::max(0, j - 1));
            if (neighbor_label != label) {
                if (std::find(neighbors[label].begin(), neighbors[label].end(), neighbor_label) == std::end(neighbors[label])) {
                    neighbors[label].push_back(neighbor_label);
                }
            }
        }
    }
    
    for (unsigned int k = 0; k < counts.size(); k++) {
        if (counts[k] > 0) {
            means[k][0] /= counts[k];
            means[k][1] /= counts[k];
            means[k][2] /= counts[k];
        }
    }
    
    std::vector<int> ids(max_label + 1, 0);
    for (unsigned int k = 0; k < ids.size(); k++) {
        ids[k] = k;
    }
    
    std::sort(ids.begin(), ids.end(), [&counts](int i, int j) {
        return counts[i] < counts[j];
    });
    
//    for (unsigned int k = 0; k < counts.size(); k++) {
//        std::cout << ids[k] << ": " << counts[ids[k]] << std::endl;
//    }
    
    int count = 0;
    std::vector<int> new_labels(max_label + 1, -1);
    
    for (unsigned int k = 0; k < std::min((int) counts.size(), number); k++) {
        int label = ids[k];
        
        float min_distance = std::numeric_limits<float>::max();
        for (unsigned int kk = 0; kk < neighbors[label].size(); kk++) {
            float distance = (means[label][0] - means[neighbors[label][kk]][0])*(means[label][0] - means[neighbors[label][kk]][0])
                    + (means[label][1] - means[neighbors[label][kk]][1])*(means[label][1] - means[neighbors[label][kk]][1])
                    + (means[label][2] - means[neighbors[label][kk]][2])*(means[label][2] - means[neighbors[label][kk]][2]);

            if (distance < min_distance && new_labels[neighbors[label][kk]] < 0) {
                min_distance = distance;
                new_labels[label] = neighbors[label][kk];
            }
        }
        
        count++;
    }
    
    for (int i = 0; i < labels.rows; i++) {
        for (int j = 0; j < labels.cols; j++) {
            
            int label = labels.at<int>(i, j);
            if (new_labels[label] >= 0) {
                
                int new_label = new_labels[label];
                while (new_labels[new_label] >= 0) {
                    new_label = new_labels[new_label];
                }
                
                labels.at<int>(i, j) = new_labels[label];
            }
        }
    }
    
    return count;
}