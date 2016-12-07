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

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "io_util.h"
#include "visualization.h"

/** \brief Example of visualizing segmentations or superpixel segmentations using lib_eval.
 * Usage:
 *  $ ../bin/example_visualization --help
 *   Allowed options:
 *     --help                                produce help message
 *     --csv arg (=../data/BSDS500/csv_groundTruth/test/14037-0.csv)
 *                                           segmentation to visualize
 *     --image arg (=../data/BSDS500/images/test/14037.jpg)
 *                                       corresponding image
 * 
 * \author David Stutz
 */
int main(int argc, char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("csv", boost::program_options::value<std::string>()->default_value("../data/BSDS500/csv_groundTruth/test/14037-0.csv"), "segmentation to visualize")
        ("image", boost::program_options::value<std::string>()->default_value("../data/BSDS500/images/test/14037.jpg"), "corresponding image");
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path csv_path(parameters["csv"].as<std::string>());
    if (!boost::filesystem::is_regular_file(csv_path)) {
        std::cout << "Could not find CSV file " << csv_path.string() << "." << std::endl;
        return 1;
    }
    
    boost::filesystem::path image_path(parameters["image"].as<std::string>());
    if (!boost::filesystem::is_regular_file(image_path)) {
        std::cout << "Could not find image file " << image_path.string() << "." << std::endl;
        return 1;
    }
    
    cv::Mat segmentation;
    IOUtil::readMatCSVInt(csv_path, segmentation);
    cv::Mat image = cv::imread(image_path.string());
    
    cv::Mat contours;
    Visualization::drawContours(image, segmentation, contours);
    
    cv::Mat random;
    Visualization::drawRandom(segmentation, random);
    
    cv::Mat means;
    Visualization::drawMeans(image, segmentation, means);
    
    cv::Mat perturbedMeans;
    Visualization::drawPerturbedMeans(image, segmentation, perturbedMeans);
    
    cv::imwrite("contours.png", contours);
    std::cout << "Wrote contours.png" << std::endl;
    
    cv::imwrite("random.png", random);
    std::cout << "Wrote random.png" << std::endl;
    
    cv::imwrite("means.png", means);
    std::cout << "Wrote means.png" << std::endl;
    
    cv::imwrite("perturbedMeans.png", perturbedMeans);
    std::cout << "Wrote perturbedMeans.png" << std::endl;
    
    return 0;
}

