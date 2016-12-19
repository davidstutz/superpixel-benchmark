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
#include <boost/program_options.hpp>
#include <boost/timer.hpp>
#include <bitset>
#include "vccs_opencv_pcl.h"
#include "io_util.h"
#include "superpixel_tools.h"
#include "depth_tools.h"
#include "visualization.h"

/** \brief Command line tool for running VCCS.
 * Usage:
 * \code{sh}
 *   $ ../bin/vccs_cli --help
 *   Allowed options:
 *     -h [ --help ]                         produce help message
 *     -i [ --input ] arg                    the folder to process
 *     -d [ --depth ] arg                    the depth folder to process
 *     --intrinsics arg                      directory containing intrinsic matrices
 *                                           (as CSV files); when set, 
 *                                           --principal-x/y and --focal are 
 *                                           overwritten
 *     --principal-x arg (=325.582458)       principal point x coordinate (default 
 *                                           for NYUV2)
 *     --principal-y arg (=253.73616)        principal point y coordinate (default 
 *                                           for NYUV2)
 *     --cropping-x arg (=16)                size of cropped border in x (default 
 *                                           for cropped NYUV2)
 *     --cropping-y arg (=16)                size of cropped border in x (default 
 *                                           for cropped NYUV2)
 *     --focal-x arg (=518.85791)            focal length in x (default for NYUV2)
 *     --focal-y arg (=519.469604)           focal length in y (default for NYUV2)
 *     -r [ --voxel-resolution ] arg (=0.00800000038)
 *                                           voxel resolution
 *     -s [ --seed-resolution ] arg (=0.100000001)
 *                                           seed resolution
 *     -p [ --spatial-weight ] arg (=0.300000012)
 *                                           spatial weight
 *     -n [ --normal-weight ] arg (=0.200000003)
 *                                           normal weight
 *     -o [ --csv ] arg                      save segmentation as CSV file
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
        ("depth,d", boost::program_options::value<std::string>(), "the depth folder to process")
        ("intrinsics", boost::program_options::value<std::string>()->default_value(""), "directory containing intrinsic matrices (as CSV files); when set, --principal-x/y and --focal are overwritten")
        ("principal-x", boost::program_options::value<float>()->default_value(325.582449), "principal point x coordinate (default for NYUV2)")
        ("principal-y", boost::program_options::value<float>()->default_value(253.736166), "principal point y coordinate (default for NYUV2)")
        ("cropping-x", boost::program_options::value<float>()->default_value(16), "size of cropped border in x (default for cropped NYUV2)")
        ("cropping-y", boost::program_options::value<float>()->default_value(16), "size of cropped border in x (default for cropped NYUV2)")
        ("focal-x", boost::program_options::value<float>()->default_value(518.857901), "focal length in x (default for NYUV2)")
        ("focal-y", boost::program_options::value<float>()->default_value(519.469611), "focal length in y (default for NYUV2)")
        ("voxel-resolution,r", boost::program_options::value<float>()->default_value(0.008f), "voxel resolution")
        ("seed-resolution,s", boost::program_options::value<float>()->default_value(0.1f), "seed resolution")
        ("spatial-weight,p", boost::program_options::value<float>()->default_value(0.3f), "spatial weight")
        ("normal-weight,n", boost::program_options::value<float>()->default_value(0.2f), "normal weight")
        ("csv,o", boost::program_options::value<std::string>()->default_value(""), "save segmentation as CSV file")
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
    
    boost::filesystem::path input_dir(parameters["input"].as<std::string>());
    if (!boost::filesystem::is_directory(input_dir)) {
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
    
    // Set up camera parameters for cloud computation.
    DepthTools::Camera camera;
    camera.principal_x = parameters["principal-x"].as<float>();
    camera.principal_y = parameters["principal-y"].as<float>();
    camera.focal_x = parameters["focal-x"].as<float>();
    camera.focal_y = parameters["focal-y"].as<float>();
    camera.cropping_x = parameters["cropping-x"].as<float>();
    camera.cropping_y = parameters["cropping-y"].as<float>();
    
    float voxel_resolution = parameters["voxel-resolution"].as<float>();
    float seed_resolution = parameters["seed-resolution"].as<float>();
    float spatial_weight = parameters["spatial-weight"].as<float>();
    float normal_weight = parameters["normal-weight"].as<float>();
    
    if (spatial_weight + normal_weight > 1) {
        std::cout << "Invalid spatial and normal weight - sum needs to be in [0,1]." << std::endl;
        return 1;
    }
    
    bool use_transform = false;
//    if (parameters.find("use-transform") != parameters.end()) {
        use_transform = true;
//    }
    
    std::multimap<std::string, boost::filesystem::path> images;
    std::vector<std::string> extensions;
    IOUtil::getImageExtensions(extensions);
    IOUtil::readDirectory(input_dir, extensions, images);
    
    float total = 0;
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = images.begin();
            it != images.end(); it++) {
        
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
            
            camera.principal_x = intrinsics.at<float>(0, 2);
            camera.principal_y = intrinsics.at<float>(1, 2);
            camera.focal_x = intrinsics.at<float>(0, 0);
            camera.focal_y = intrinsics.at<float>(1, 1);
        }
        
        cv::Mat cloud;
        cv::Mat labels;
        DepthTools::computeCloudFromDepth(depth, camera, cloud);
        
        boost::timer timer;
        VCCS_OpenCV_PCL::computeSuperpixels(image, cloud, voxel_resolution, 
                seed_resolution, spatial_weight, normal_weight, use_transform, labels);
        float elapsed = timer.elapsed();
        total += elapsed;
        
        int unconnected_components = SuperpixelTools::relabelConnectedSuperpixels(labels);
//        int merged_components = SuperpixelTools::enforceMinimumSuperpixelSize(image, labels, 5);
        int merged_components = SuperpixelTools::enforceMinimumSuperpixelSizeUpTo(image, labels, unconnected_components);
        SuperpixelTools::relabelSuperpixels(labels);
        
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