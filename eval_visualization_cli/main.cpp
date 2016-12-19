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

/** \brief Visualize segmentations.
 * Usage:
 * \code{sh}
 *   $ ../bin/eval_visualization_cli --help
 *   Allowed options:
 *     --help                     produce help message
 *     --csv arg                  superpixel segmentation (as CSV)
 *     --images arg               image
 *     --contours                 draw contours
 *     --contours-on-white        draw contours on white image
 *     --means                    draw means
 *     --perturbed-means          draw perturbed means
 *     --random                   randomly color
 *     -v [ --vis ] arg (=output) output folder
 *     -x [ --prefix ] arg        input and output file prefix
 *     -w [ --wordy ]             verbose/wordy/debug
 * \endcode
 * \author David Stutz
 */
int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("csv", boost::program_options::value<std::string>(), "superpixel segmentation (as CSV)")
        ("images", boost::program_options::value<std::string>()->default_value(""), "image")
        ("contours", "draw contours")
        ("contours-on-white", "draw contours on white image")
        ("means", "draw means")
        ("perturbed-means", "draw perturbed means")
        ("random", "randomly color")
        ("vis,v", boost::program_options::value<std::string>()->default_value("output"), "output folder")
        ("prefix,x", boost::program_options::value<std::string>()->default_value(""), "input and output file prefix")
        ("wordy,w", "verbose/wordy/debug");

    boost::program_options::positional_options_description positionals;
    positionals.add("csv", 1);
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path csv_dir(parameters["csv"].as<std::string>());
    if (!boost::filesystem::is_directory(csv_dir)) {
        std::cout << "Superpixel segmentation directory  does not exist." << std::endl;
        return 1;
    }
    
    boost::filesystem::path image_dir(parameters["images"].as<std::string>());
    if (!image_dir.empty()) {
        if (!boost::filesystem::is_directory(image_dir)) {
            std::cout << "Image directory does not exist." << std::endl;
            return 1;
        }
    }
    
    boost::filesystem::path out_dir(parameters["vis"].as<std::string>());
    if (!boost::filesystem::is_directory(out_dir)) {
        boost::filesystem::create_directories(out_dir);
    }
    
    std::string prefix = parameters["prefix"].as<std::string>();
    
    bool wordy = false;
    if (parameters.find("wordy") != parameters.end()) {
        wordy = true;
    }
    
    std::multimap<std::string, boost::filesystem::path> files;
    std::vector<std::string> extensions;
    IOUtil::getCSVExtensions(extensions);
    std::vector<std::string> exclude;
    exclude.push_back("correlation");
    exclude.push_back("results");
    exclude.push_back("summary");
    IOUtil::readDirectory(csv_dir, extensions, files, prefix, "", exclude);
    
    if (wordy) {
        std::cout << "Found " << files.size() << " files." << std::endl;
    }
    
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = files.begin(); 
            it != files.end(); it++) {
        
        cv::Mat sp_segmentation;
        IOUtil::readMatCSVInt(it->second, sp_segmentation);
        
        std::string filename = it->second.stem().string().substr(prefix.length(), 
                it->second.stem().string().length() - prefix.length() + 1);
        boost::filesystem::path image_file = image_dir / 
                boost::filesystem::path(filename + ".png");
        if (!boost::filesystem::is_regular_file(image_file)) {
            image_file = image_dir / 
                boost::filesystem::path(filename + ".jpg");
        }
        
        LOG_IF (FATAL, !boost::filesystem::is_regular_file(image_file) && !image_dir.empty())
                << "Image file not found for: " << it->first << ".";
        
        cv::Mat image;
        if (!image_dir.empty()) {
            image = cv::imread(image_file.string());
        }
        
        if (parameters.find("contours") != parameters.end()
                && !image.empty()) {
            
            cv::Mat contours;
            Visualization::drawContours(image, sp_segmentation, contours);
            
            // Prefix already included!
            boost::filesystem::path contours_file = out_dir / 
                    boost::filesystem::path(it->second.stem().string() + "_contours.png");
            cv::imwrite(contours_file.string(), contours);
        }

        if (parameters.find("contours-on-white") != parameters.end()) {
            
            cv::Mat white_image(sp_segmentation.rows, sp_segmentation.cols, 
                    CV_8UC3, cv::Scalar(255, 255, 255));
            
            cv::Mat contours;
            Visualization::drawContours(white_image, sp_segmentation, contours);
            
            // Prefix already included!
            boost::filesystem::path contours_file = out_dir / 
                    boost::filesystem::path(it->second.stem().string() + "_contours_white.png");
            cv::imwrite(contours_file.string(), contours);
        }
        
        if (parameters.find("means") != parameters.end()
                && !image.empty()) {
            
            cv::Mat means;
            Visualization::drawMeans(image, sp_segmentation, means);
            
            // Prefix already included!
            boost::filesystem::path means_file = out_dir / 
                    boost::filesystem::path(it->second.stem().string() + "_means.png");
            cv::imwrite(means_file.string(), means);
        }
        
        if (parameters.find("random") != parameters.end()) {
            cv::Mat random;
            Visualization::drawRandom(sp_segmentation, random);
            
            boost::filesystem::path random_file = out_dir / 
                    boost::filesystem::path(it->second.stem().string() + "_random.png");
            cv::imwrite(random_file.string(), random);
        }
        
        if (parameters.find("perturbed-means") != parameters.end()
                && !image.empty()) {
            
            cv::Mat perturbed_means;
            Visualization::drawPerturbedMeans(image, sp_segmentation, perturbed_means);
            
            boost::filesystem::path perturbed_means_file = out_dir / 
                    boost::filesystem::path(it->second.stem().string() + "_perturbed_means.png");
            cv::imwrite(perturbed_means_file.string(), perturbed_means);
        }
    }
    
    return 0;
}