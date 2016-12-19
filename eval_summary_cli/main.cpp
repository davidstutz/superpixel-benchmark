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

#include "io_util.h"
#include "parameter_optimization_tool.h"

/** \brief Compute an evaluation summary.
 * Usage:
 * \code{sh}
 *   $ ../bin/eval_summary_cli --help
 *   Allowed options:
 *     --sp-directory arg    superpixel segmentation directory
 *     --img-directory arg   image directory
 *     --gt-directory arg    ground truth directory
 *     --append-file arg     append file
 *     --vis                 visualize results
 *     --help                produce help message
 * \endcode
 * \author David Stutz
 */
int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("sp-directory", boost::program_options::value<std::string>(), "superpixel segmentation directory")
        ("img-directory", boost::program_options::value<std::string>(), "image directory")
        ("gt-directory", boost::program_options::value<std::string>(), "ground truth directory")
        ("append-file", boost::program_options::value<std::string>()->default_value(""), "append file")
        ("vis", "visualize results")
        ("help", "produce help message");

    boost::program_options::positional_options_description positionals;
    positionals.add("sp-directory", 1);
    positionals.add("img-directory", 1);
    positionals.add("gt-directory", 1);
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path sp_directory(parameters["sp-directory"].as<std::string>());
    if (!boost::filesystem::is_directory(sp_directory)) {
        std::cout << "Superpixel segmentation directory does not exist." << std::endl;
        return 1;
    }
    
    boost::filesystem::path img_directory(parameters["img-directory"].as<std::string>());
    if (!boost::filesystem::is_directory(img_directory)) {
        std::cout << "Image directory does not exist." << std::endl;
        return 1;
    }
    
    boost::filesystem::path gt_directory(parameters["gt-directory"].as<std::string>());
    if (!boost::filesystem::is_directory(gt_directory)) {
        std::cout << "Ground truth directory does not exist." << std::endl;
        return 1;
    }
    
    EvaluationSummary::EvaluationMetrics metrics;
    EvaluationSummary::EvaluationStatistics statistics;
    EvaluationSummary::SuperpixelVisualizations visualizations;
    if (parameters.find("vis") != parameters.end()) {
        visualizations.contour = true;
        visualizations.mean = true;
        visualizations.pre_rec = true;
        visualizations.ue = true;
        visualizations.random = true;
        visualizations.perturbed_mean = true;
    }
    
    EvaluationSummary summary(sp_directory, gt_directory, img_directory,
            metrics, statistics, visualizations);
    summary.setComputeCorrelation(true);
    
    boost::filesystem::path append_file(parameters["append-file"].as<std::string>());
    if (!append_file.empty()) {
        summary.setAppendFile(append_file);
    }
    
    int gt_max = 0;
    summary.computeSummary(gt_max);
    
    return 0;
}