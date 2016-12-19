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

#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>
#include <glog/logging.h>
#include "visualization.h"
#include "io_util.h"

/** \brief Combine three visualizations.
 * Usage:
 * \code{sh}
 *   $ ../bin/eval_visualization_fuse_3_cli --help
 *   Allowed options:
 *     --help                     produce help message
 *     --images-one arg           image directory one
 *     --images-two arg           image directory two
 *     --images-three arg         image directory three
 *     -v [ --vis ] arg (=output) output folder
 *     -x [ --prefix ] arg        input and output file prefix
 *     --suffix1 arg              input and output file suffix
 *     --suffix2 arg              input and output file suffix
 *     --suffix3 arg              input and output file suffix
 *     --exclude arg              file name part to exclude
 *     -w [ --wordy ]             verbose/wordy/debug
 * \endcode
 * \author David Stutz
 */
int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("images-one", boost::program_options::value<std::string>(), "image directory one")
        ("images-two", boost::program_options::value<std::string>(), "image directory two")
        ("images-three", boost::program_options::value<std::string>(), "image directory three")
        ("vis,v", boost::program_options::value<std::string>()->default_value("output"), "output folder")
        ("prefix,x", boost::program_options::value<std::string>()->default_value(""), "input and output file prefix")
        ("suffix1", boost::program_options::value<std::string>()->default_value(""), "input and output file suffix")
        ("suffix2", boost::program_options::value<std::string>()->default_value(""), "input and output file suffix")
        ("suffix3", boost::program_options::value<std::string>()->default_value(""), "input and output file suffix")
        ("exclude", boost::program_options::value<std::string>()->default_value(""), "file name part to exclude")
        ("wordy,w", "verbose/wordy/debug");

    boost::program_options::positional_options_description positionals;
    positionals.add("images-one", 1);
    positionals.add("images-two", 1);
    positionals.add("images-three", 1);
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path image1_dir(parameters["images-one"].as<std::string>());
    if (!boost::filesystem::is_directory(image1_dir)) {
        std::cout << "Image directory does not exist." << std::endl;
        return 1;
    }
    
    boost::filesystem::path image2_dir(parameters["images-two"].as<std::string>());
    if (!boost::filesystem::is_directory(image2_dir)) {
        std::cout << "Image directory does not exist." << std::endl;
        return 1;
    }
    
    boost::filesystem::path image3_dir(parameters["images-three"].as<std::string>());
    if (!boost::filesystem::is_directory(image3_dir)) {
        std::cout << "Image directory does not exist." << std::endl;
        return 1;
    }
    
    boost::filesystem::path out_dir(parameters["vis"].as<std::string>());
    if (!boost::filesystem::is_directory(out_dir)) {
        boost::filesystem::create_directories(out_dir);
    }
    
    std::string prefix = parameters["prefix"].as<std::string>();
    std::string suffix1 = parameters["suffix1"].as<std::string>();
    std::string suffix2 = parameters["suffix2"].as<std::string>();
    std::string suffix3 = parameters["suffix3"].as<std::string>();
    std::string exclude = parameters["exclude"].as<std::string>();
    
    if (!suffix1.empty() && suffix2.empty()) {
        suffix2 = suffix1;
    }
    
    if (!suffix1.empty() && suffix3.empty()) {
        suffix3 = suffix1;
    }
    
    bool wordy = false;
    if (parameters.find("wordy") != parameters.end()) {
        wordy = true;
    }
    
    std::multimap<std::string, boost::filesystem::path> files1;
    std::multimap<std::string, boost::filesystem::path> files2;
    std::multimap<std::string, boost::filesystem::path> files3;
    
    std::vector<std::string> extensions;
    IOUtil::getImageExtensions(extensions);
    
    std::vector<std::string> exclude_vector;
    if (!exclude.empty()) {
        exclude_vector = {exclude};
    }
    
    IOUtil::readDirectory(image1_dir, extensions, files1, prefix, suffix1, exclude_vector);
    IOUtil::readDirectory(image2_dir, extensions, files2, prefix, suffix2, exclude_vector);
    IOUtil::readDirectory(image3_dir, extensions, files3, prefix, suffix3, exclude_vector);
    
    if (files1.size() != files2.size() || files2.size() != files3.size()) {
        std::cout << "Not same number of images found in all three directories: " 
                << files1.size() << ", " << files2.size() << ", " << files3.size() << std::endl;
        return 1;
    }
    
    if (wordy) {
        std::cout << "Found " << files1.size() << " files in the first directory." << std::endl;
    }
    
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = files1.begin(), it2 = files2.begin(), it3 = files3.begin(); 
            it != files1.end() && it2 != files2.end() && it3 != files3.end(); it++, it2++, it3++) {
        
        cv::Mat image1_raw = cv::imread(it->second.string());
        cv::Mat image2_raw = cv::imread(it2->second.string());
        cv::Mat image3_raw = cv::imread(it3->second.string());
        
        LOG_IF(FATAL, image1_raw.rows != image2_raw.rows || image1_raw.cols != image2_raw.cols 
                || image1_raw.rows != image3_raw.rows || image1_raw.cols != image3_raw.cols) 
                << "Image do not have same dimensions!";
        
        bool rotated = false;
        cv::Mat image1;
        cv::Mat image2;
        cv::Mat image3;
        
        if (image1_raw.rows < image1_raw.cols) {
            
            image1.create (image1_raw.cols, image1_raw.rows, CV_8UC3);
            image2.create (image2_raw.cols, image2_raw.rows, CV_8UC3);
            image3.create (image3_raw.cols, image3_raw.rows, CV_8UC3);
            
            for (int i = 0; i < image1.rows; i++) {
                for (int j = 0; j < image2.cols; j++) {
                    image1.at<cv::Vec3b>(i, j) = image1_raw.at<cv::Vec3b>(j, i);
                    image2.at<cv::Vec3b>(i, j) = image2_raw.at<cv::Vec3b>(j, i);
                    image3.at<cv::Vec3b>(i, j) = image3_raw.at<cv::Vec3b>(j, i);
                }
            }
            
            // !
            rotated = true;
        }
        else {
            image1 = image1_raw;
            image2 = image2_raw;
            image3 = image3_raw;
        }
        
        int rows = image1.rows;
        int cols = image1.cols;
        
        int line_row_start1 = rows/3 + 0.35*rows;
        int line_row_end1 = rows/3 - 0.35*rows;
        float slope1 = (line_row_end1 - line_row_start1) / (float) cols;
        
        int line_row_start2 = 2*rows/3 + 0.35*rows;
        int line_row_end2 = 2*rows/3 - 0.35*rows;
        float slope2 = (line_row_end2 - line_row_start2) / (float) cols;
        
        cv::Mat image_raw (rows, cols, CV_8UC3, cv::Scalar(255, 255, 255));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (i < j*slope1 + line_row_start1) {
                    image_raw.at<cv::Vec3b>(i, j) = image1.at<cv::Vec3b>(i, j);
                }
                else if (i < j*slope2 + line_row_start2) {
                    image_raw.at<cv::Vec3b>(i, j) = image2.at<cv::Vec3b>(i, j);
                }
                else {
                    image_raw.at<cv::Vec3b>(i, j) = image3.at<cv::Vec3b>(i, j);
                }
            }
        }
        
        cv::line(image_raw, cv::Point(0, line_row_start1), cv::Point(cols - 1, line_row_end1), 
                cv::Scalar(255, 255, 255), 2);
//        cv::line(image_raw, cv::Point(0, line_row_start1 + 3), cv::Point(cols - 1, line_row_end1 + 3), 
//                cv::Scalar(0, 0, 0), 2);
//        cv::line(image_raw, cv::Point(0, line_row_start1 - 3), cv::Point(cols - 1, line_row_end1 - 3), 
//                cv::Scalar(0, 0, 0), 2);
        
        cv::line(image_raw, cv::Point(0, line_row_start2), cv::Point(cols - 1, line_row_end2), 
                cv::Scalar(255, 255, 255), 2);
//        cv::line(image_raw, cv::Point(0, line_row_start2 + 3), cv::Point(cols - 1, line_row_end2 + 3), 
//                cv::Scalar(0, 0, 0), 2);
//        cv::line(image_raw, cv::Point(0, line_row_start2 - 3), cv::Point(cols - 1, line_row_end2 - 3), 
//                cv::Scalar(0, 0, 0), 2);
        
        cv::Mat image;
        if (rotated) {
            image.create(image_raw.cols, image_raw.rows, CV_8UC3);
            
            for (int i = 0; i < image.rows; i++) {
                for (int j = 0; j < image.cols; j++) {
                    image.at<cv::Vec3b>(i, j) = image_raw.at<cv::Vec3b>(j, i);
                }
            }
        }
        else {
            image = image_raw;
        }
        
        boost::filesystem::path out_file = out_dir 
            / boost::filesystem::path(it->second.stem().string() + ".png");
        cv::imwrite(out_file.string(), image);
        
        
    }
    
    return 0;
}