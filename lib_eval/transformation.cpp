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

#include <random>
#include <glog/logging.h>
#include "transformation.h"

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyGaussianAdditiveNoise
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyGaussianAdditiveNoise(const cv::Mat &image, float variance, 
        cv::Mat &noisy_image) {
    
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only color images are supported.";
    LOG_IF(FATAL, variance <= 0) << "Invalid variance.";
    
    std::random_device random;
    std::mt19937 gen(random());
    std::normal_distribution<float> g(0, variance);
    
    noisy_image.create(image.rows, image.cols, image.type());
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            
            cv::Vec3b noisy_color = image.at<cv::Vec3b>(i, j);
            noisy_color[0] = std::max(0, std::min(255, (int) (noisy_color[0] + g(gen))));
            noisy_color[1] = std::max(0, std::min(255, (int) (noisy_color[1] + g(gen))));
            noisy_color[2] = std::max(0, std::min(255, (int) (noisy_color[2] + g(gen))));
            
            noisy_image.at<cv::Vec3b>(i, j) = noisy_color;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyGaussianSamplingErrors
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyGaussianSamplingErrors(const cv::Mat &image, float variance, 
        cv::Mat &noisy_image) {
    
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only color images are supported.";
    LOG_IF(FATAL, variance <= 0) << "Invalid variance.";
    
    std::random_device random;
    std::mt19937 gen(random());
    std::normal_distribution<float> g(0, variance);
    
    cv::Mat image_gray;
    cv::cvtColor(image, image_gray, CV_BGR2GRAY);
    
    cv::Mat grad_x;
    cv::Mat grad_y;
    cv::Sobel(image_gray, grad_x, CV_32F, 1, 0, 3);
    cv::Sobel(image_gray, grad_y, CV_32F, 0, 1, 3);
    
    cv::Mat magnitude;
    cv::convertScaleAbs(grad_x, grad_x);
    cv::convertScaleAbs(grad_y, grad_y);
    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, magnitude);
    
    noisy_image.create(image.rows, image.cols, image.type());
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            
            cv::Vec3b noisy_color = image.at<cv::Vec3b>(i, j);
            float r = g(gen);
            
            noisy_color[0] = std::max(0, std::min(255, (int) (noisy_color[0] + r*magnitude.at<unsigned char>(i, j))));
            noisy_color[1] = std::max(0, std::min(255, (int) (noisy_color[1] + r*magnitude.at<unsigned char>(i, j))));
            noisy_color[2] = std::max(0, std::min(255, (int) (noisy_color[2] + r*magnitude.at<unsigned char>(i, j))));
            
            noisy_image.at<cv::Vec3b>(i, j) = noisy_color;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applySaltAndPepperNoise
////////////////////////////////////////////////////////////////////////////////

void Transformation::applySaltAndPepperNoise(const cv::Mat &image, float p,
        cv::Mat &noisy_image) {
    
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only color images are supported.";
    
    noisy_image.create(image.rows, image.cols, image.type());
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            
            cv::Vec3b noisy_color = image.at<cv::Vec3b>(i, j);
            
            float r = (float) std::rand() / RAND_MAX;
            if (r <= p) {
                
                r = (float) std::rand() / RAND_MAX;
                if (r <= 0.5) {
                    noisy_color[0] = 0;
                    noisy_color[1] = 0;
                    noisy_color[2] = 0;
                }
                else {
                    noisy_color[0] = 255;
                    noisy_color[1] = 255;
                    noisy_color[2] = 255;
                }
            }
            
            noisy_image.at<cv::Vec3b>(i, j) = noisy_color;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyPoissonNoise
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyPoissonNoise(const cv::Mat &image, cv::Mat &noisy_image) {
    
    LOG_IF(FATAL, image.empty()) << "Given image is empty.";
    LOG_IF(FATAL, image.channels() != 3) << "Currently only color images are supported.";
    
    std::random_device random;
    std::mt19937 gen(random());
    
    noisy_image.create(image.rows, image.cols, image.type());
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            
            cv::Vec3b noisy_color = image.at<cv::Vec3b>(i, j);
            std::poisson_distribution<unsigned char> p0(noisy_color[0]);
            std::poisson_distribution<unsigned char> p1(noisy_color[1]);
            std::poisson_distribution<unsigned char> p2(noisy_color[2]);
            
            noisy_color[0] = p0(gen);
            noisy_color[1] = p1(gen);
            noisy_color[2] = p2(gen);
            
            noisy_image.at<cv::Vec3b>(i, j) = noisy_color;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyBlur
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyBlur(const cv::Mat &image, int size, cv::Mat &blurred_image) {
    LOG_IF(FATAL, size <= 0) << "Size needs to be greater than zero.";
    LOG_IF(FATAL, size%2 == 0) << "Size needs to be odd.";
    
    cv::blur(image, blurred_image, cv::Size(size, size));
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyGaussianBlur
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyGaussianBlur(const cv::Mat &image, int size, float sigma_x, 
        float sigma_y, cv::Mat &blurred_image) {
    
    LOG_IF(FATAL, size <= 0) << "Size needs to be greater than zero.";
    LOG_IF(FATAL, size%2 == 0) << "Size needs to be odd.";
    
    cv::GaussianBlur(image, blurred_image, cv::Size(size, size), sigma_x, sigma_y);
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyMedianBlur
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyMedianBlur(const cv::Mat &image, int size, cv::Mat &blurred_image) {
    LOG_IF(FATAL, size <= 0) << "Size needs to be greater than zero.";
    LOG_IF(FATAL, size%2 == 0) << "Size needs to be odd.";
    
    cv::medianBlur(image, blurred_image, size);
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyBilateralFilter
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyBilateralFilter(const cv::Mat &image, int size, float sigma_color, 
        float sigma_space, cv::Mat &filtered_image) {
    
    cv::bilateralFilter(image, filtered_image, size, sigma_color, sigma_space);
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyMotionBlur
////////////////////////////////////////////////////////////////////////////////

void Transformation::applyMotionBlur(const cv::Mat &image, int size, float angle, cv::Mat &blurred_image) {
    LOG_IF(FATAL, size <= 0) << "Size needs to be greater than zero.";
    LOG_IF(FATAL, size%2 == 0) << "Size needs to be odd.";
    
    cv::Point2f pt1;
    pt1.x = 0;
    pt1.y = 0;
    
    cv::Point2f pt2;
    pt2.x = size/2;
    pt2.y = std::tan(angle)*pt2.x;
    
    float sum = 0;
    cv::Mat filter(size, size, CV_32FC1, cv::Scalar(0));
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int ii = i - size/2;
            int jj = j - size/2;
            
            float distance = std::abs((pt2.y- pt1.y)*jj - (pt2.x - pt1.x)*ii + pt2.x*pt1.y - pt2.y*pt1.x)
                    / std::sqrt((pt2.y - pt1.y)*(pt2.y - pt1.y) + (pt2.x - pt1.x)*(pt2.x - pt1.x));
            filter.at<float>(i, j) = std::max(1 - distance, 0.f);
            sum += filter.at<float>(i, j);
        }
    }
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            filter.at<float>(i, j) /= sum;
        }
    }
    
    cv::filter2D(image, blurred_image, -1, filter, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
}

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyVerticalShear
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void Transformation::applyVerticalShear(const cv::Mat &image, int crop, float shear, cv::Mat &sheared_image) {
    
    int cols = image.cols;
    LOG_IF(FATAL, shear*(cols - 1) > crop) << "Shear too strong for given crop!";
    
    sheared_image.create(image.rows - 2*crop, image.cols - 2*crop, image.type());
    for (int i = 0; i < sheared_image.rows; i++) {
        for (int j = 0; j < sheared_image.cols; j++) {
            sheared_image.at<T>(i, j) = image.at<T>(i + crop, shear*(i + crop) + crop + j);
        }
    }
}

template void Transformation::applyVerticalShear<cv::Vec3b>(const cv::Mat &image, 
        int crop, float shear, cv::Mat &sheared_image);
template void Transformation::applyVerticalShear<int>(const cv::Mat &image, 
        int crop, float shear, cv::Mat &sheared_image);

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyHorizontalShear
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void Transformation::applyHorizontalShear(const cv::Mat &image, int crop, float shear, cv::Mat &sheared_image) {
    
    int rows = image.rows;
    LOG_IF(FATAL, shear*(rows - 1) > crop) << "Shear too strong for given crop!";
    
    sheared_image.create(image.rows - 2*crop, image.cols - 2*crop, image.type());
    for (int i = 0; i < sheared_image.rows; i++) {
        for (int j = 0; j < sheared_image.cols; j++) {
            sheared_image.at<T>(i, j) = image.at<T>(shear*(j + crop) + crop + i, j + crop);
        }
    }
}

template void Transformation::applyHorizontalShear<cv::Vec3b>(const cv::Mat &image, int crop, float shear, 
        cv::Mat &sheared_image);
template void Transformation::applyHorizontalShear<int>(const cv::Mat &image, int crop, float shear, 
        cv::Mat &sheared_image);

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyRotation
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void Transformation::applyRotation(const cv::Mat &image, int crop, float angle, cv::Mat &rotated_image, int interpolation) {
    
    float rad = angle/180.f * M_PI;
    int long_side = std::max(image.rows - 2*crop, image.cols - 2*crop);
    LOG_IF(FATAL, rad > std::atan((float) crop/(long_side/2))) << "Angle too large for given crop: " 
            << rad << " > " << std::atan((float) crop/(long_side/2));
    LOG_IF(FATAL, image.type() == CV_32S) 
            << "(Signed) integer matrices not supported in cv::warAffine (http://answers.opencv.org/question/18787/warpperspective-with-inter_cubic-or-inter_linear-and-one-channel-int-source-crash/).";
    
    cv::Mat full_rotated_image;
    cv::Mat rotation = cv::getRotationMatrix2D(cv::Point2f((float) image.cols/2, (float) image.rows/2), angle, 1.0);
    cv::warpAffine(image, full_rotated_image, rotation, cv::Size(image.cols, image.rows), interpolation);
    
    rotated_image.create(image.rows - 2*crop, image.cols - 2*crop, image.type());
    for (int i = 0; i < rotated_image.rows; i++) {
        for (int j = 0; j < rotated_image.cols; j++) {
            rotated_image.at<T>(i, j) = full_rotated_image.at<T>(i + crop, j+ crop);
        }
    }
}

template void Transformation::applyRotation<cv::Vec3b>(const cv::Mat &image, int crop, float angle, 
        cv::Mat &rotated_image, int interpolation);
template void Transformation::applyRotation<int>(const cv::Mat &image, int crop, float angle, 
        cv::Mat &rotated_image, int interpolation);
template void Transformation::applyRotation<float>(const cv::Mat &image, int crop, float angle, 
        cv::Mat &rotated_image, int interpolation);

////////////////////////////////////////////////////////////////////////////////
// Transformation::applyTranslation
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void Transformation::applyTranslation(const cv::Mat &image, int crop, int translation_x, int translation_y, cv::Mat &translated_image) {
    
    LOG_IF(FATAL, translation_x > crop || translation_y > crop) << "Translation too large for crop!";
    
    translated_image.create(image.rows - 2*crop, image.cols - 2*crop, image.type());
    for (int i = 0; i < translated_image.rows; i++) {
        for (int j = 0; j < translated_image.cols; j++) {
            translated_image.at<T>(i, j) = image.at<T>(i + crop + translation_y, 
                    j + crop + translation_x);
        }
    }
}

template void Transformation::applyTranslation<cv::Vec3b>(const cv::Mat &image, 
        int crop, int translation_x, int translation_y, cv::Mat &translated_image);
template void Transformation::applyTranslation<int>(const cv::Mat &image, 
        int crop, int translation_x, int translation_y, cv::Mat &translated_image);