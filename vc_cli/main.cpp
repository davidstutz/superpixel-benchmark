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
#include "vc_opencv.h"
#include "io_util.h"
#include "superpixel_tools.h"
#include "visualization.h"

/** \brief Command line tool for running VC.
 * Usage:
 * \code{sh}
 *   $ ../bin/vc_cli --help
 *   Allowed options:
 *     -h [ --help ]                         produce help message
 *     -i [ --input ] arg                    folder containing the images to process
 *     -s [ --superpixels ] arg (=400)       number superpixels
 *     -c [ --weight ] arg (=5)              number superpixels
 *     -g [ --radius ] arg (=3)              radius
 *     -n [ --neighboring-clusters ] arg (=200)
 *                                           number of neighboring clusters
 *     -d [ --direct-neighbors ] arg (=4)    number of direct neighbors
 *     -t [ --threshold ] arg (=10)          threshold influencing the number of 
 *                                           iterations
 *     -r [ --color-space ] arg (=1)         color space; 0 for RGB, > 0 for Lab
 *     -o [ --csv ] arg                      save segmentation as CSV file
 *     -v [ --vis ] arg                      visualize contours
 *     -x [ --prefix ] arg                   output file prefix
 *     -w [ --wordy ]                        verbose/wordy/debug
 * \endcode
 * \author David Stutz
 */
int main (int argc, char ** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", boost::program_options::value<std::string>(), "folder containing the images to process")
        ("superpixels,s", boost::program_options::value<int>()->default_value(400), "number superpixels")
        ("weight,c", boost::program_options::value<double>()->default_value(5.0), "number superpixels")
        ("radius,g", boost::program_options::value<int>()->default_value(3), "radius")
        ("neighboring-clusters,n", boost::program_options::value<int>()->default_value(200), "number of neighboring clusters")
        ("direct-neighbors,d", boost::program_options::value<int>()->default_value(4), "number of direct neighbors")
        ("threshold,t", boost::program_options::value<int>()->default_value(10), "threshold influencing the number of iterations")
        ("color-space,r", boost::program_options::value<int>()->default_value(1), "color space; 0 for RGB, > 0 for Lab")
        ("csv,o", boost::program_options::value<std::string>()->default_value(""), "save segmentation as CSV file")
        ("vis,v", boost::program_options::value<std::string>()->default_value(""), "visualize contours")
        ("prefix,x", boost::program_options::value<std::string>()->default_value(""), "output file prefix")
        ("wordy,w", "verbose/wordy/debug");
    
    boost::program_options::positional_options_description positionals;
    positionals.add("input", 1);
    
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
    
    std::string prefix = parameters["prefix"].as<std::string>();
    
    bool wordy = false;
    if (parameters.find("wordy") != parameters.end()) {
        wordy = true;
    }
    
    int superpixels = parameters["superpixels"].as<int>();
    double weight = parameters["weight"].as<double>();
    int radius = parameters["radius"].as<int>();
    int neighboring_clusters = parameters["neighboring-clusters"].as<int>();
    int direct_neighbors = parameters["direct-neighbors"].as<int>();
    int threshold = parameters["threshold"].as<int>();
    
    std::multimap<std::string, boost::filesystem::path> images;
    std::vector<std::string> extensions;
    IOUtil::getImageExtensions(extensions);
    IOUtil::readDirectory(input_dir, extensions, images);
    
    float total = 0;
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = images.begin(); 
            it != images.end(); ++it) {
        
        cv::Mat image = cv::imread(it->first);
        cv::Mat labels;
        
        boost::timer timer;
        VC_OpenCV::computeSuperpixels(image, superpixels, weight, radius, 
                neighboring_clusters, direct_neighbors, threshold, labels);
        float elapsed = timer.elapsed();
        total += elapsed;
        
        int unconnected_components = SuperpixelTools::relabelConnectedSuperpixels(labels);
        int merged_unconnected_components = SuperpixelTools::enforceMinimumSuperpixelSizeUpTo(image, labels, unconnected_components);
        
        int size = (float) (image.rows*image.cols)/superpixels/10;
        int merged_small_components = SuperpixelTools::enforceMinimumSuperpixelSize(image, labels, size);
        SuperpixelTools::relabelSuperpixels(labels);
        
        if (wordy) {
            std::cout << SuperpixelTools::countSuperpixels(labels) << " superpixels for " << it->first 
                    << " (" << unconnected_components << " not connected; " 
                    << (merged_unconnected_components + merged_small_components) << " merged; "
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
