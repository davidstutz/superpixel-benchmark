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
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "evaluation_summary.h"

/** \brief Examples for computing evaluaiton summaries.
 * Usage:
 *  $ ../bin/example_evaluation_summary --help
 *   Allowed options:
 *     --help                                produce help message
 *     --sp-directory arg (=../examples/data/)
 *                                           directory with superpixel segmentations
 *                                           as CSV files
 *     --gt-directory arg (=../data/BSDS500/csv_groundTruth/test/)
 *                                           directory with ground truth 
 *                                           segmentations as CSV files
 *     --img-directory arg (=../data/BSDS500/images/test/)
 *                                           directory containing the images as JPG 
 *                                           or PNG files
 * \author David Stutz
 */
int main(int argc, char** argv) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("sp-directory", boost::program_options::value<std::string>()->default_value("../examples/data/"), "directory with superpixel segmentations as CSV files")
        ("gt-directory", boost::program_options::value<std::string>()->default_value("../data/BSDS500/csv_groundTruth/test/"), "directory with ground truth segmentations as CSV files")
        ("img-directory", boost::program_options::value<std::string>()->default_value("../data/BSDS500/images/test/"), "directory containing the images as JPG or PNG files");
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path spDirectory(parameters["sp-directory"].as<std::string>());
    if (!boost::filesystem::is_directory(spDirectory)) {
        std::cout << "Could not find directory " << spDirectory.string() << std::endl;
        return 1;
    }
    
    boost::filesystem::path gtDirectory(parameters["gt-directory"].as<std::string>());
    if (!boost::filesystem::is_directory(gtDirectory)) {
        std::cout << "Could not find directory " << gtDirectory.string() << std::endl;
        return 1;
    }
    
    boost::filesystem::path imgDirectory(parameters["img-directory"].as<std::string>());
    if (!boost::filesystem::is_directory(imgDirectory)) {
        std::cout << "Could not find directory " << imgDirectory.string() << std::endl;
        return 1;
    }
    
    // All visualizations are false by default.
    EvaluationSummary::SuperpixelVisualizations visualizations;
    visualizations.contour = true;
    
    // All metrics are true by default.
    EvaluationSummary::EvaluationMetrics metrics;
    metrics.ue = false;
    metrics.oe = false;
    metrics.rec = true;
    metrics.pre = false;
    metrics.ue_np = true;
    metrics.ue_levin = false;
    metrics.asa = false;
    metrics.sse_rgb = false;
    metrics.sse_xy = false;
    metrics.co = false;
    metrics.ev = false;
    metrics.mde = false;
    metrics.icv = false;
    metrics.cd = false;
    metrics.reg = false;
    metrics.sp = true;
    metrics.sp_size = false;
    
    // All statistics are true by default.
    EvaluationSummary::EvaluationStatistics statistics;
    statistics.mean = true;
    statistics.median_and_quartiles = false;
    statistics.mid_range = false;
    statistics.mid_range = false;
    statistics.min_and_max = false;
    statistics.std = true;
    
    // Note that gtDirectory contains up to five ground truths per
    // superpixel segmentation (as taken from the BSDS500), see
    // data/BSDS500/csv_groundTruth/test.
    EvaluationSummary summary(spDirectory, gtDirectory, imgDirectory, 
            metrics, statistics, visualizations);
    
    int gt_max = 0;
    summary.computeSummary(gt_max); 
    
    // Summaries are saved in spDirectory.
    boost::filesystem::path summaryPath(spDirectory.string() + "/summary.csv");
    std::ifstream summaryFile(summaryPath.string().c_str(), std::ifstream::in);
    
    std::cout << "evalaution.csv" << std::endl;
    std::cout << "--------------" << std::endl;
    
    std::string line;
    while (std::getline(summaryFile, line)) {
        std::cout << line << std::endl;
    }
    
    return 0;
}

