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
#include "seeds2.h"
#include "io_util.h"
#include "superpixel_tools.h"
#include "visualization.h"

/** \brief Command line tool for running SEEDS.
 * Usage:
 * \code{sh}
 *   $ ../bin/seeds_cli --help
 *   Allowed options:
 *     -h [ --help ]                         produce help message
 *     -i [ --input ] arg                    the folder to process (can also be 
 *                                           passed as positional argument)
 *     -s [ --superpixels ] arg (=400)       number of superpixels
 *     -b [ --bins ] arg (=5)                number of bins
 *     -c [ --confidence ] arg (=0.100000001)
 *                                           minimum confidence used for block 
 *                                           update
 *     -p [ --prior ] arg (=1)               use prior: > 0 for prior, = 0 for non 
 *                                           prior
 *     -m [ --means ] arg (=1)               use mean pixel updates: > 0 for yes, = 
 *                                           0 for no
 *     -t [ --iterations ] arg (=2)          iterations at each level
 *     -r [ --color-space ] arg (=1)         color space: 0 = RGB, 1 = Lab, 2 = HSV
 *     -f [ --fair ]                         for a fair comparison with other 
 *                                           algorithms, quadratic blocks are used 
 *                                           for initialization
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
        ("input,i", boost::program_options::value<std::string>(), "the folder to process (can also be passed as positional argument)")
        ("superpixels,s", boost::program_options::value<int>()->default_value(400), "number of superpixels")
        ("bins,b", boost::program_options::value<int>()->default_value(5), "number of bins")
        ("confidence,c", boost::program_options::value<float>()->default_value(0.1), "minimum confidence used for block update")
        ("prior,p", boost::program_options::value<int>()->default_value(1), "use prior: > 0 for prior, = 0 for non prior")
        ("means,m", boost::program_options::value<int>()->default_value(1), "use mean pixel updates: > 0 for yes, = 0 for no")
        ("iterations,t", boost::program_options::value<int>()->default_value(2), "iterations at each level")
        ("color-space,r", boost::program_options::value<int>()->default_value(1), "color space: 0 = RGB, 1 = Lab, 2 = HSV")
        ("fair,f", "for a fair comparison with other algorithms, quadratic blocks are used for initialization")
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
    int iterations = parameters["iterations"].as<int>();
    int bins = parameters["bins"].as<int>();
    float confidence = parameters["confidence"].as<float>();
    int prior_int = parameters["prior"].as<int>();
    bool prior = prior_int > 0 ? true : false;
    int means_int = parameters["means"].as<int>();
    bool means = means_int > 0 ? true : false;
    int color_space = parameters["color-space"].as<int>();
    
    if (color_space < 0 || color_space > 2) {
        std::cout << "Invalid color space." << std::endl;
        return 1;
    }
    
    std::multimap<std::string, boost::filesystem::path> images;
    std::vector<std::string> extensions;
    IOUtil::getImageExtensions(extensions);
    IOUtil::readDirectory(input_dir, extensions, images);
    
    float total = 0;
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = images.begin(); 
            it != images.end(); ++it) {
        
        cv::Mat image = cv::imread(it->first);
        
        int region_width = 2;
        int region_height = 2;
        int levels = 2;
        
        if (parameters.find("fair") != parameters.end()) {
            SuperpixelTools::computeRegionSizeLevels(image, superpixels, 
                    region_width, levels);
            region_height = region_width;
        }
        else {
            SuperpixelTools::computeHeightWidthLevelsFromSuperpixels(image, 
                    superpixels, region_height, region_width, levels);
        }
        
        boost::timer timer;
        SEEDS seeds(image.cols, image.rows, image.channels(), bins, 0, 
                confidence, prior, means, color_space);
        seeds.initialize(image, region_width, region_height, levels);
        seeds.iterate(iterations);
        float elapsed = timer.elapsed();
        total += elapsed;
        
        cv::Mat labels(image.rows, image.cols, CV_32SC1, cv::Scalar(0));
        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
                labels.at<int>(i, j) = seeds.labels[levels - 1][j + image.cols*i];
            }
        }
        
        int unconnected_components = SuperpixelTools::relabelConnectedSuperpixels(labels);
        
        if (wordy) {
            std::cout << SuperpixelTools::countSuperpixels(labels) << " superpixels for " << it->first 
                    << " (" << unconnected_components << " not connected; " 
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
