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
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>
#include <boost/timer.hpp>
#include "dasp_opencv.h"
#include "io_util.h"
#include "superpixel_tools.h"
#include "visualization.h"

/** \brief Command line tool for running DASP.
 * Usage:
 * \code{sh}
 *   $ ../bin/dasp_cli --help
 *   Allowed options:
 *     -h [ --help ]                         produce help message
 *     -i [ --input ] arg                    the folder to process
 *     -d [ --depth ] arg                    the folder containing the 
 *                                           corresnponding depth images.
 *     --intrinsics arg                      directory containing intrinsic matrices
 *                                           (as CSV files); when set, 
 *                                           --principal-x/y and --focal are 
 *                                           overwritten
 *     --principal-x arg (=325.582458)       principal point x coordinate (default 
 *                                           for NYUV2)
 *     --principal-y arg (=253.73616)        principal point y coordinate (default 
 *                                           for NYUV2)
 *     --focal arg (=519)                    focal length (default for NYUV2; 
 *                                           rounded from focal-x and focal-y)
 *     --cropping-x arg (=16)                size of cropped border in x (default 
 *                                           for cropped NYUV2)
 *     --cropping-y arg (=16)                size of cropped border in x (default 
 *                                           for cropped NYUV2)
 *     -m [ --seed-mode ] arg (=0)           seed mode (0: random, 1: spds, 2: 
 *                                           delta)
 *     -s [ --superpixels ] arg (=400)       number of superpixels (overwrites 
 *                                           'radius')
 *     -p [ --spatial-weight ] arg (=0.300000012)
 *                                           spatial weight
 *     -n [ --normal-weight ] arg (=0.200000003)
 *                                           normal weight
 *     -t [ --iterations ] arg (=5)          iterations
 *     -o [ --csv ] arg                      specify the output directory (default 
 *                                           is ./output)
 *     -v [ --vis ] arg                      visualize contours
 *     -x [ --prefix ] arg                   output file prefix
 *     -w [ --wordy ]                        verbose/wordy/debug
 * \endcode 
 * \author David Stutz
 */
int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", boost::program_options::value<std::string>(), "the folder to process")
        ("depth,d", boost::program_options::value<std::string>(), "the folder containing the corresnponding depth images.")
        ("intrinsics", boost::program_options::value<std::string>()->default_value(""), "directory containing intrinsic matrices (as CSV files); when set, --principal-x/y and --focal are overwritten")
        ("principal-x", boost::program_options::value<float>()->default_value(325.582449), "principal point x coordinate (default for NYUV2)")
        ("principal-y", boost::program_options::value<float>()->default_value(253.736166), "principal point y coordinate (default for NYUV2)")
        ("focal", boost::program_options::value<float>()->default_value(519), "focal length (default for NYUV2; rounded from focal-x and focal-y)")
        ("cropping-x", boost::program_options::value<float>()->default_value(16), "size of cropped border in x (default for cropped NYUV2)")
        ("cropping-y", boost::program_options::value<float>()->default_value(16), "size of cropped border in x (default for cropped NYUV2)")
        ("seed-mode,m", boost::program_options::value<int>()->default_value(0), "seed mode (0: random, 1: spds, 2: delta)")
        ("superpixels,s", boost::program_options::value<int>()->default_value(400), "number of superpixels (overwrites 'radius')")
        ("spatial-weight,p", boost::program_options::value<float>()->default_value(0.3f), "spatial weight")
//        ("color-weight,c", boost::program_options::value<float>()->default_value(2.0f), "color weight")
        ("normal-weight,n", boost::program_options::value<float>()->default_value(0.2f), "normal weight")
        ("iterations,t", boost::program_options::value<int>()->default_value(5), "iterations")
        ("csv,o", boost::program_options::value<std::string>()->default_value(""), "specify the output directory (default is ./output)")
        ("vis,v", boost::program_options::value<std::string>()->default_value(""), "visualize contours")
        ("prefix,x", boost::program_options::value<std::string>()->default_value(""), "output file prefix")
        ("wordy,w", "verbose/wordy/debug");

    boost::program_options::positional_options_description positionals;
    positionals.add("input", 1);
    positionals.add("depth", 1);
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path output_dir(parameters["csv"].as<std::string>());
    if (!output_dir.empty()) {
        if (!boost::filesystem::is_directory(output_dir)) {
            boost::filesystem::create_directories(output_dir);
        }
    }
    
    boost::filesystem::path vis_dir(parameters["vis"].as<std::string>());
    if (!vis_dir.empty()) {
        if (!boost::filesystem::is_directory(vis_dir)) {
            boost::filesystem::create_directories(vis_dir);
        }
    }
    
    boost::filesystem::path image_dir(parameters["input"].as<std::string>());
    if (!boost::filesystem::is_directory(image_dir)) {
        std::cout << "Image directory not found ..." << std::endl;
        return 1;
    }
    
    boost::filesystem::path depth_dir(parameters["depth"].as<std::string>());
    if (!boost::filesystem::is_directory(depth_dir)) {
        std::cout << "Depth directory not found ..." << std::endl;
        return 1;
    }

    boost::filesystem::path intrinsics_dir(parameters["intrinsics"].as<std::string>());
    std::string prefix = parameters["prefix"].as<std::string>();
    
    bool wordy = false;
    if (parameters.find("wordy") != parameters.end()) {
        wordy = true;
    }
    
    dasp::Camera camera;
    camera.cx = parameters["principal-x"].as<float>();
    camera.cy = parameters["principal-y"].as<float>();
    camera.focal = parameters["focal"].as<float>();
    camera.z_slope = 0.001f;
    camera.crx = parameters["cropping-x"].as<float>();
    camera.cry = parameters["cropping-y"].as<float>();
    
    float spatial_weight = parameters["spatial-weight"].as<float>();
    float normal_weight= parameters["normal-weight"].as<float>();
    int superpixels = parameters["superpixels"].as<int>();
    int seed_mode = parameters["seed-mode"].as<int>();
    int iterations = parameters["iterations"].as<int>();
    
    if (spatial_weight < 0 || spatial_weight > 1) {
        std::cout << "Invalid spatial weight, select spatial weight in [0,1]." << std::endl;
        return 1;
    }
    
    if (normal_weight < 0 || normal_weight > 1) {
        std::cout << "Invalid normal weight, select normal weight in [0,1]." << std::endl;
        return 1;
    }
    
    if (normal_weight + spatial_weight > 1) {
        std::cout << "Invalid normal and spatial weight, sum needs to be in [0,1]." << std::endl;
        return 1;
    }
    
    std::multimap<std::string, boost::filesystem::path> images;
    std::vector<std::string> extensions;
    IOUtil::getImageExtensions(extensions);
    IOUtil::readDirectory(image_dir, extensions, images);
    
    float total = 0;
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = images.begin(); 
            it != images.end(); ++it) {
        
        cv::Mat image = cv::imread(it->first);
        
        boost::filesystem::path depth_file = depth_dir 
                / boost::filesystem::path(it->second.stem().string() + ".png");
        if (!boost::filesystem::is_regular_file(depth_file)) {
            depth_file = depth_dir 
                / boost::filesystem::path(it->second.stem().string() + ".jpg");
            
            if (!boost::filesystem::is_regular_file(depth_file)) {
                std::cout << "Depth file not found for: " << it->first << "." 
                        << std::endl;
                return 1;
            }
        }
        
        cv::Mat depth = cv::imread(depth_file.string(), CV_LOAD_IMAGE_ANYDEPTH);
        
        if (depth.rows != image.rows || depth.cols != image.cols) {
            std::cout << "Image and depth dimensions do not match for: " 
                    << it->first << std::endl;
            return 1;
        }
        
        if (!intrinsics_dir.empty()) {
            boost::filesystem::path intrinsics_file = intrinsics_dir 
                    / boost::filesystem::path(it->second.stem().string() + ".csv");
            
            if (!boost::filesystem::is_regular_file(intrinsics_file)) {
                std::cout << "Intrinsics directory given but file not found for: "
                        << it->first << "." << std::endl;
                return 1;
            }
            
            cv::Mat intrinsics;
            IOUtil::readMatCSVFloat(intrinsics_file, intrinsics);
            if (intrinsics.rows != intrinsics.cols || intrinsics.rows != 3) {
                std::cout << "Invalid intrinsics CSV file for: " << it->first << "." 
                        << std::endl;
                return 1;
            }
            
            camera.cx = intrinsics.at<float>(0, 2);
            camera.cy = intrinsics.at<float>(1, 2);
            camera.focal = intrinsics.at<float>(0, 0);
            camera.z_slope = 0.001f;
        }
        
        boost::timer timer;
        cv::Mat labels;
        DASP_OpenCV::computeSuperpixels(image, depth, superpixels, spatial_weight, 
                normal_weight, seed_mode, iterations, camera, labels);
        float elapsed = timer.elapsed();
        total += elapsed;
        
        int unconnected_components = SuperpixelTools::relabelConnectedSuperpixels(labels);
        int merged_components = SuperpixelTools::enforceMinimumSuperpixelSize(image, labels, 5);
        merged_components += SuperpixelTools::enforceMinimumSuperpixelSizeUpTo(image, labels, unconnected_components);
//        SuperpixelTools::relabelSuperpixels(labels);
        
        if (wordy) {
            std::cout << SuperpixelTools::countSuperpixels(labels) << " superpixels for " << it->first 
                    << " (" << unconnected_components << " not connected; " 
                    << merged_components << " merged; "
                    << elapsed <<")." << std::endl;
        }
        
        if (!output_dir.empty()) {
            boost::filesystem::path csv_file(output_dir 
                    / boost::filesystem::path(prefix + it->second.stem().string() + ".csv"));
            IOUtil::writeMatCSV<int>(csv_file, labels);
        }
        
        if (!vis_dir.empty()) {
            boost::filesystem::path contours_file(vis_dir 
                    / boost::filesystem::path(prefix + it->second.stem().string() + ".png"));
            cv::Mat image_contours;
            Visualization::drawContours(image, labels, image_contours);
            cv::imwrite(contours_file.string(), image_contours);
        }
    }
    
    if (wordy) {
        std::cout << "Average time: " << total / images.size() << "." << std::endl;
    }
    
    if (!output_dir.empty()) {
        std::ofstream runtime_file(output_dir.string() + "/" + prefix + "runtime.txt", 
                std::ofstream::out | std::ofstream::app);
        
        runtime_file << total / images.size() << "\n";
        runtime_file.close();
    }
    
    return 0;
}
