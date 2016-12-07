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

#include <fstream>
#include <glog/logging.h>
#include "transformation.h"
#include "io_util.h"
#include "evaluation_summary.h"
#include "robustness_tool.h"

////////////////////////////////////////////////////////////////////////////////
// RobustnessTool::RobustnessTool
////////////////////////////////////////////////////////////////////////////////

RobustnessTool::RobustnessTool(boost::filesystem::path& base_directory_, boost::filesystem::path& image_directory_, 
        boost::filesystem::path& gt_directory_, std::string command_line_, RobustnessToolDriver* driver_) : base_directory(base_directory_),
        image_directory(image_directory_), gt_directory(gt_directory_), command_line(command_line_), driver(driver_) {
    
    
}

////////////////////////////////////////////////////////////////////////////////
// RobustnessTool::setFilesToKeep
////////////////////////////////////////////////////////////////////////////////

void RobustnessTool::setFilesToKeep(const std::vector<std::string> &files_) {
    files = files_;
}

////////////////////////////////////////////////////////////////////////////////
// RobustnessTool::evaluate
////////////////////////////////////////////////////////////////////////////////

void RobustnessTool::evaluate() {
    
    std::vector<std::string> image_extensions;
    IOUtil::getImageExtensions(image_extensions);
//    
//    std::vector<std::string> csv_extensions;
//    IOUtil::getCSVExtensions(csv_extensions);
    
    std::ofstream file(base_directory.string() + "/command_line.txt");
    file << command_line;
    file.close();
    
    boost::filesystem::path append_file = base_directory 
            / boost::filesystem::path("summary.csv");
    
    int k = 0;
    do {
        boost::filesystem::path current_directory = base_directory
                / boost::filesystem::path(std::to_string(k) + "_" + driver->identify());
        
        if (!boost::filesystem::is_directory(current_directory)) {
            boost::filesystem::create_directories(current_directory);
        }
        
        boost::filesystem::path current_image_directory = current_directory
                / boost::filesystem::path("images");
        if (!boost::filesystem::is_directory(current_image_directory)) {
            boost::filesystem::create_directories(current_image_directory);
        }
        
        boost::filesystem::path current_segmentation_directory = current_directory
                / boost::filesystem::path("csv_groundTruth");
        if (!boost::filesystem::is_directory(current_segmentation_directory)) {
            boost::filesystem::create_directories(current_segmentation_directory);
        }
        
        boost::filesystem::path current_superpixel_directory = current_directory
                / boost::filesystem::path("sp");
        if (!boost::filesystem::is_directory(current_superpixel_directory)) {
            boost::filesystem::create_directories(current_superpixel_directory);
        }
        
        std::multimap<std::string, boost::filesystem::path> images;
        IOUtil::readDirectory(image_directory, image_extensions, images);
        
        for (std::multimap<std::string, boost::filesystem::path>::iterator it = images.begin(); 
                it != images.end(); it++) {
            
            cv::Mat image = cv::imread(it->first);
            
            bool multiple_segmentations = false;
            boost::filesystem::path segmentation_file = gt_directory 
                    / boost::filesystem::path(it->second.stem().string() + ".csv");
            
            if (!boost::filesystem::is_regular_file(segmentation_file)) {
                segmentation_file = gt_directory 
                    / boost::filesystem::path(it->second.stem().string() + "-0.csv");
                
                LOG_IF(FATAL, !boost::filesystem::is_regular_file(segmentation_file)) << "Segmentation file not found for: " << it->first;
                multiple_segmentations = true;
            }
            
            if (multiple_segmentations) {
                int i = 0;
                while (boost::filesystem::is_regular_file(segmentation_file)) {
                    
                    cv::Mat segmentation;
                    IOUtil::readMatCSVInt(segmentation_file, segmentation);
                    
                    cv::Mat computed_segmentation;
                    driver->computeSegmentation(segmentation, computed_segmentation);
                    
                    boost::filesystem::path computed_segmentation_file = current_segmentation_directory
                            / boost::filesystem::path(it->second.stem().string() + "-" + std::to_string(i) + ".csv");
                    IOUtil::writeMatCSV<int>(computed_segmentation_file, computed_segmentation);
                    
                    i++;
                    segmentation_file = gt_directory 
                    / boost::filesystem::path(it->second.stem().string() + "-" + std::to_string(i) + ".csv");
                }
            }
            else {
                cv::Mat segmentation;
                IOUtil::readMatCSVInt(segmentation_file, segmentation);

                cv::Mat computed_segmentation;
                driver->computeSegmentation(segmentation, computed_segmentation);

                boost::filesystem::path computed_segmentation_file = current_segmentation_directory
                        / boost::filesystem::path(it->second.stem().string() + ".csv");
                IOUtil::writeMatCSV<int>(computed_segmentation_file, computed_segmentation);
            }
            
            cv::Mat computed_image;
            driver->computeImage(image, computed_image);
            
            boost::filesystem::path computed_image_file = current_image_directory
                    / it->second.filename();
            cv::imwrite(computed_image_file.string(), computed_image);
        }
        
        std::string current_command_line = command_line 
                + " -i " + current_image_directory.string() 
                + " -o " + current_superpixel_directory.string();
        
        int status = system(current_command_line.c_str());
        
        if (status != 0) {
            LOG(FATAL) << "Command line was not successful: " << current_command_line;
        }
        
        EvaluationSummary summary(current_superpixel_directory, 
                current_segmentation_directory, current_image_directory);
        summary.setComputeCorrelation(true);
        summary.setAppendFile(append_file);
        
        int gt_max = 0;
        summary.computeSummary(gt_max);
        
        cleanDirectory(current_segmentation_directory);
        cleanDirectory(current_image_directory);
        cleanDirectory(current_superpixel_directory);
        
        std::cout << ".";
        k++;
    } while(driver->next());
    
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// RobustnessTool::cleanDirectory
////////////////////////////////////////////////////////////////////////////////

void RobustnessTool::cleanDirectory(boost::filesystem::path directory) {
    
    boost::filesystem::directory_iterator end;
    for (boost::filesystem::directory_iterator it(directory); it != end; it++) {
            
        bool found = false;
        if (it->path().string().find("summary") != std::string::npos) {
            found = true;
        }
        if (it->path().string().find("results") != std::string::npos) {
            found = true;
        }
        if (it->path().string().find("runtime") != std::string::npos) {
            found = true;
        }
        if (it->path().string().find("correlation") != std::string::npos) {
            found = true;
        }
        
        for (unsigned int i = 0; i < files.size(); i++) {
            if (it->path().stem().string().find(files[i]) != std::string::npos) {
                found = true;
            }
        }

        if (!found) {
            boost::filesystem::remove(it->path());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// GaussianNoiseDriver::GaussianNoiseDriver
////////////////////////////////////////////////////////////////////////////////

GaussianNoiseDriver::GaussianNoiseDriver(std::string type_, const std::vector<float> &variances_)
        : type(type_), variances(variances_), current(0) {
    
    if (type != "additive" && type != "sampling") {
        LOG(FATAL) << "Only 'additive' or 'sampling' type supported.";
    }
    
    LOG_IF(FATAL, variances.size() <= 0) << "Empty vector given.";
}

////////////////////////////////////////////////////////////////////////////////
// GaussianNoiseDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void GaussianNoiseDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    if (type == "additive") {
        Transformation::applyGaussianAdditiveNoise(image, variances[current], computed_image);
    }
    else if (type == "sampling") {
        Transformation::applyGaussianSamplingErrors(image, variances[current], computed_image);
    }
}

////////////////////////////////////////////////////////////////////////////////
// GaussianNoiseDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void GaussianNoiseDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// GaussianNoiseDriver::next
////////////////////////////////////////////////////////////////////////////////

bool GaussianNoiseDriver::next() {
    
    current++;
    return (current < (int) variances.size());
}

////////////////////////////////////////////////////////////////////////////////
// GaussianNoiseDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string GaussianNoiseDriver::identify() {
    return "gaussian_additive_" + std::to_string(variances[current]);
}

////////////////////////////////////////////////////////////////////////////////
// PoissonNoiseDriver::PoissonNoiseDriver
////////////////////////////////////////////////////////////////////////////////

PoissonNoiseDriver::PoissonNoiseDriver() {
    
}

////////////////////////////////////////////////////////////////////////////////
// PoissonNoiseDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void PoissonNoiseDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applyPoissonNoise(image, computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// PoissonNoiseDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void PoissonNoiseDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// PoissonNoiseDriver::next
////////////////////////////////////////////////////////////////////////////////

bool PoissonNoiseDriver::next() {
    // Nothing ...
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// PoissonNoiseDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string PoissonNoiseDriver::identify() {
    return "poisson";
}

////////////////////////////////////////////////////////////////////////////////
// SaltAndPepperNoiseDriver::SaltAndPepperNoiseDriver
////////////////////////////////////////////////////////////////////////////////

SaltAndPepperNoiseDriver::SaltAndPepperNoiseDriver(const std::vector<float> &probabilities_) : probabilities(probabilities_), current(0) {
    LOG_IF(FATAL, probabilities.size() <= 0) << "Empty vector given.";
}

////////////////////////////////////////////////////////////////////////////////
// SaltAndPepperNoiseDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void SaltAndPepperNoiseDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applySaltAndPepperNoise(image, probabilities[current], computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// SaltAndPepperNoiseDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void SaltAndPepperNoiseDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// SaltAndPepperNoiseDriver::next
////////////////////////////////////////////////////////////////////////////////

bool SaltAndPepperNoiseDriver::next() {
    current++;
    return (current < (int) probabilities.size());
}

////////////////////////////////////////////////////////////////////////////////
// SaltAndPepperNoiseDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string SaltAndPepperNoiseDriver::identify() {
    return "salt_pepper_" + std::to_string(probabilities[current]);
}

////////////////////////////////////////////////////////////////////////////////
// BlurDriver::BlurDriver
////////////////////////////////////////////////////////////////////////////////

BlurDriver::BlurDriver(const std::vector<int> &sizes_) : sizes(sizes_), current(0) {
    LOG_IF(FATAL, sizes.size() <= 0) << "Empty vector given.";
}

////////////////////////////////////////////////////////////////////////////////
// BlurDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void BlurDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applyBlur(image, sizes[current], computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// BlurDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void BlurDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// BlurDriver::next
////////////////////////////////////////////////////////////////////////////////

bool BlurDriver::next() {
    current++;
    return (current < (int) sizes.size());
}

////////////////////////////////////////////////////////////////////////////////
// BlurDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string BlurDriver::identify() {
    return "blur_" + std::to_string(sizes[current]);
}

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurDriver::GaussianBlurDriver
////////////////////////////////////////////////////////////////////////////////

GaussianBlurDriver::GaussianBlurDriver(const std::vector<int> &sizes_, const std::vector<float> &variances_) 
        : sizes(sizes_), variances(variances_), current(0) {
    LOG_IF(FATAL, sizes.size() <= 0) << "Empty vector given.";
    LOG_IF(FATAL, sizes.size() != variances.size()) << "Invalid vectors.";
}

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void GaussianBlurDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applyGaussianBlur(image, sizes[current], variances[current], 
            variances[current], computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void GaussianBlurDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurDriver::next
////////////////////////////////////////////////////////////////////////////////

bool GaussianBlurDriver::next() {
    current++;
    return (current < (int) sizes.size());
}

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string GaussianBlurDriver::identify() {
    return "gaussian_blur_" + std::to_string(sizes[current]) + "_" + std::to_string(variances[current]);
}

////////////////////////////////////////////////////////////////////////////////
// MedianBlurDriver::MedianBlurDriver
////////////////////////////////////////////////////////////////////////////////

MedianBlurDriver::MedianBlurDriver(const std::vector<int> &sizes_) 
        : sizes(sizes_), current(0) {
    LOG_IF(FATAL, sizes.size() <= 0) << "Empty vector given.";
}

////////////////////////////////////////////////////////////////////////////////
// MedianBlurDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void MedianBlurDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applyMedianBlur(image, sizes[current], computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// MedianBlurDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void MedianBlurDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// MedianBlurDriver::next
////////////////////////////////////////////////////////////////////////////////

bool MedianBlurDriver::next() {
    current++;
    return (current < (int) sizes.size());
}

////////////////////////////////////////////////////////////////////////////////
// MedianBlurDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string MedianBlurDriver::identify() {
    return "median_blur_" + std::to_string(sizes[current]);
}

////////////////////////////////////////////////////////////////////////////////
// BilateralFilterDriver::BilateralFilterDriver
////////////////////////////////////////////////////////////////////////////////

BilateralFilterDriver::BilateralFilterDriver(const std::vector<int> &sizes_, const std::vector<float> &color_variances_, 
        const std::vector<float> &space_variances_) : sizes(sizes_), color_variances(color_variances_), 
        space_variances(space_variances_), current(0) {
    
    LOG_IF(FATAL, sizes.size() <= 0) << "Empty vector given.";
    LOG_IF(FATAL, sizes.size() != color_variances.size()) << "Invalid vectors.";
    LOG_IF(FATAL, sizes.size() != space_variances.size()) << "Invalid vectors.";
}

////////////////////////////////////////////////////////////////////////////////
// BilateralFilterDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void BilateralFilterDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    
    cv::Mat image_lab;
    cv::cvtColor(image, image_lab, CV_BGR2Lab);
    
    cv::Mat computed_image_lab;
    Transformation::applyBilateralFilter(image_lab, sizes[current], color_variances[current], 
            space_variances[current], computed_image_lab);
    cv::cvtColor(computed_image_lab, computed_image, CV_Lab2BGR);
    
}

////////////////////////////////////////////////////////////////////////////////
// BilateralFilterDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void BilateralFilterDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// BilateralFilterDriver::next
////////////////////////////////////////////////////////////////////////////////

bool BilateralFilterDriver::next() {
    current++;
    return (current < (int) sizes.size());
}

////////////////////////////////////////////////////////////////////////////////
// BilateralFilterDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string BilateralFilterDriver::identify() {
    return "bilateral_filter_" + std::to_string(sizes[current]) + "_" + std::to_string(color_variances[current])
            + "_" + std::to_string(space_variances[current]);
}

////////////////////////////////////////////////////////////////////////////////
// MotionBlurDriver::MotionBlurDriver
////////////////////////////////////////////////////////////////////////////////

MotionBlurDriver::MotionBlurDriver(const std::vector<int> &sizes_, const std::vector<float> &angles_) 
        : sizes(sizes_), angles(angles_), current(0) {
    LOG_IF(FATAL, sizes.size() <= 0) << "Empty vector given.";
    LOG_IF(FATAL, sizes.size() != angles.size()) << "Empty vector given.";
}

////////////////////////////////////////////////////////////////////////////////
// MotionBlurDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void MotionBlurDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applyMotionBlur(image, sizes[current], angles[current], computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// MotionBlurDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void MotionBlurDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Nothing ...
    computed_segmentation = segmentation.clone();
}

////////////////////////////////////////////////////////////////////////////////
// MotionBlurDriver::next
////////////////////////////////////////////////////////////////////////////////

bool MotionBlurDriver::next() {
    current++;
    return (current < (int) sizes.size());
}

////////////////////////////////////////////////////////////////////////////////
// MotionBlurDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string MotionBlurDriver::identify() {
    return "bilateral_filter_" + std::to_string(sizes[current]) + "_" + std::to_string(angles[current]);
}

////////////////////////////////////////////////////////////////////////////////
// ShearDriver::ShearDriver
////////////////////////////////////////////////////////////////////////////////

ShearDriver::ShearDriver(std::string type_, int crop_, const std::vector<float> &m_)
        : type(type_), crop(crop_), m(m_), current(0) {
    
    if (type != "vertical" && type != "horizontal") {
        LOG(FATAL) << "Only 'vertical' or 'horizontal' type supported.";
    }
    
    LOG_IF(FATAL, m.size() <= 0) << "Empty vector given.";
}

////////////////////////////////////////////////////////////////////////////////
// ShearDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void ShearDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    if (type == "vertical") {
        Transformation::applyVerticalShear<cv::Vec3b>(image, crop, m[current], computed_image);
    }
    else if (type == "horizontal") {
        Transformation::applyHorizontalShear<cv::Vec3b>(image, crop, m[current], computed_image);
    }
}

////////////////////////////////////////////////////////////////////////////////
// ShearDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void ShearDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    if (type == "vertical") {
        Transformation::applyVerticalShear<int>(segmentation, crop, m[current], computed_segmentation);
    }
    else if (type == "horizontal") {
        Transformation::applyHorizontalShear<int>(segmentation, crop, m[current], computed_segmentation);
    }
}

////////////////////////////////////////////////////////////////////////////////
// GaussianNoiseDriver::next
////////////////////////////////////////////////////////////////////////////////

bool ShearDriver::next() {
    
    current++;
    return (current < (int) m.size());
}

////////////////////////////////////////////////////////////////////////////////
// ShearDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string ShearDriver::identify() {
    return "shear_" + std::to_string(m[current]);
}

////////////////////////////////////////////////////////////////////////////////
// RotationDriver::RotationDriver
////////////////////////////////////////////////////////////////////////////////

RotationDriver::RotationDriver(int crop_, const std::vector<float> &angles_)
        : crop(crop_), angles(angles_), current(0) {
    
    LOG_IF(FATAL, angles.size() <= 0) << "Empty vector given.";
}

////////////////////////////////////////////////////////////////////////////////
// RotationDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void RotationDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applyRotation<cv::Vec3b>(image, crop, angles[current], computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// RotationDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void RotationDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    // Dirty workaroound
    cv::Mat float_segmentation;
    segmentation.convertTo(float_segmentation, CV_32F);
    
    cv::Mat float_computed_segmentation;
    Transformation::applyRotation<float>(float_segmentation, crop, angles[current], 
            float_computed_segmentation, cv::INTER_NEAREST);
    
    float_computed_segmentation.convertTo(computed_segmentation, CV_32S);
}

////////////////////////////////////////////////////////////////////////////////
// RotationDriver::next
////////////////////////////////////////////////////////////////////////////////

bool RotationDriver::next() {
    
    current++;
    return (current < (int) angles.size());
}

////////////////////////////////////////////////////////////////////////////////
// RotationDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string RotationDriver::identify() {
    return "rotation_" + std::to_string(angles[current]);
}

////////////////////////////////////////////////////////////////////////////////
// TranslationDriver::TranslationDriver
////////////////////////////////////////////////////////////////////////////////

TranslationDriver::TranslationDriver(int crop_, const std::vector<int> &x_, const std::vector<int> &y_)
        : crop(crop_), x(x_), y(y_), current(0) {
    
    LOG_IF(FATAL, x.size() <= 0) << "Empty vector given.";
    LOG_IF(FATAL, x.size() != y.size()) << "Invalid vectors.";
}

////////////////////////////////////////////////////////////////////////////////
// TranslationDriver::computeImage
////////////////////////////////////////////////////////////////////////////////

void TranslationDriver::computeImage(const cv::Mat& image, cv::Mat& computed_image) {
    Transformation::applyTranslation<cv::Vec3b>(image, crop, x[current], y[current], computed_image);
}

////////////////////////////////////////////////////////////////////////////////
// TranslationDriver::computeSegmentation
////////////////////////////////////////////////////////////////////////////////

void TranslationDriver::computeSegmentation(const cv::Mat& segmentation, cv::Mat& computed_segmentation) {
    Transformation::applyTranslation<int>(segmentation, crop, x[current], y[current], computed_segmentation);
}

////////////////////////////////////////////////////////////////////////////////
// TranslationDriver::next
////////////////////////////////////////////////////////////////////////////////

bool TranslationDriver::next() {
    
    current++;
    return (current < (int) x.size());
}

////////////////////////////////////////////////////////////////////////////////
// TranslationDriver::identify
////////////////////////////////////////////////////////////////////////////////

std::string TranslationDriver::identify() {
    return "translation_" + std::to_string(x[current]) + "_" + std::to_string(y[current]);
}