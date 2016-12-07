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
#include "superpixel_tools.h"
#include "evaluation.h"

/** \brief Example of using the evaluation metrics, demonstrated on the ground
 * truth from the BSDS500; also demonstrates the influence of connected components.
 * Usage:
 * $ ../bin/example_evaluation --help
 *   Allowed options:
 *     --help                                produce help message
 *     --csv arg (=../examples/data/14037.csv)
 *                                           segmentation to evaluate
 *     --ground-truth arg (=../data/BSDS500/csv_groundTruth/test/14037-0.csv)
 *                                           ground truth segmentation
 *     --image arg (=../data/BSDS500/images/test/14037.jpg)
 *                                           image
 * \author David Stutz
 */
int main(int argc, char** argv) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("csv", boost::program_options::value<std::string>()->default_value("../examples/data/14037.csv"), "segmentation to evaluate")
        ("ground-truth", boost::program_options::value<std::string>()->default_value("../data/BSDS500/csv_groundTruth/test/14037-0.csv"), "ground truth segmentation")
        ("image", boost::program_options::value<std::string>()->default_value("../data/BSDS500/images/test/14037.jpg"), "image");
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path csvPath(parameters["csv"].as<std::string>());
    if (!boost::filesystem::is_regular_file(csvPath)) {
        std::cout << "Could not find CSV file " << csvPath.string() << "." << std::endl;
        return 1;
    }
    
    boost::filesystem::path groundTruthPath(parameters["ground-truth"].as<std::string>());
    if (!boost::filesystem::is_regular_file(groundTruthPath)) {
        std::cout << "Could not find CSV file " << groundTruthPath.string() << std::endl;
        return 1;
    }
    
    boost::filesystem::path imagePath(parameters["image"].as<std::string>());
    if (!boost::filesystem::is_regular_file(imagePath)) {
        std::cout << "Could not find image " << imagePath.string() << std::endl;
        return 1;
    }
    
    cv::Mat image = cv::imread(imagePath.string());
    
    cv::Mat segmentation;
    IOUtil::readMatCSVInt(csvPath, segmentation);
    
    cv::Mat groundTruth;
    IOUtil::readMatCSVInt(groundTruthPath, groundTruth);
    
    int k = Evaluation::computeSuperpixels(segmentation);
    float asa = Evaluation::computeAchievableSegmentationAccuracy(segmentation, groundTruth);
    float ue_np = Evaluation::computeNPUndersegmentationError(segmentation, groundTruth);
    float ue_levin = Evaluation::computeLevinUndersegmentationError(segmentation, groundTruth);
    float rec = Evaluation::computeBoundaryRecall(segmentation, groundTruth);
    float ev = Evaluation::computeExplainedVariation(segmentation, image);
    float icv = Evaluation::computeIntraClusterVariation(segmentation, image);
    float sse_rgb = Evaluation::computeSumOfSquaredErrorRGB(segmentation, image);
    float sse_xy = Evaluation::computeSumOfSquaredErrorXY(segmentation, image);
    float co = Evaluation::computeCompactness(segmentation);
    float cd = Evaluation::computeContourDensity(segmentation);
    
    std::cout << "WITHOUT running connected components:" << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "Superpixels (K): " << k << std::endl;
    std::cout << "Achievable Segmentation Accuracy (ASA): " << asa << std::endl;
    std::cout << "Undersegmentation Error of Neubert, Protzel (UE_NP): " << ue_np << std::endl;
    std::cout << "Undersegmentation Error of Levinshtein (UE_Levin): " << ue_levin << std::endl;
    std::cout << "Boundary Recall (Rec): " << rec << std::endl;
    std::cout << "Explained Variation (EV): " << ev << std::endl;
    std::cout << "Intra-Cluster Variation (ICV): " << icv << std::endl;
    std::cout << "Sum-of-squared Error RGB (SSE_RGB): " << sse_rgb << std::endl;
    std::cout << "Sum-of-squared Error XY (SSE_XY): " << sse_xy << std::endl;
    std::cout << "Compactness (CO): " << co << std::endl;
    std::cout << "Contour Density (CD): " << cd << std::endl;
    
    SuperpixelTools::relabelConnectedSuperpixels(segmentation);
    
    k = Evaluation::computeSuperpixels(segmentation);
    asa = Evaluation::computeAchievableSegmentationAccuracy(segmentation, groundTruth);
    ue_np = Evaluation::computeNPUndersegmentationError(segmentation, groundTruth);
    ue_levin = Evaluation::computeLevinUndersegmentationError(segmentation, groundTruth);
    rec = Evaluation::computeBoundaryRecall(segmentation, groundTruth);
    ev = Evaluation::computeExplainedVariation(segmentation, image);
    icv = Evaluation::computeIntraClusterVariation(segmentation, image);
    sse_rgb = Evaluation::computeSumOfSquaredErrorRGB(segmentation, image);
    sse_xy = Evaluation::computeSumOfSquaredErrorXY(segmentation, image);
    co = Evaluation::computeCompactness(segmentation);
    cd = Evaluation::computeContourDensity(segmentation);
    
    std::cout << std::endl << "WITH running connected components:" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Superpixels (K): " << k << std::endl;
    std::cout << "Achievable Segmentation Accuracy (ASA): " << asa << std::endl;
    std::cout << "Undersegmentation Error of Neubert, Protzel (UE_NP): " << ue_np << std::endl;
    std::cout << "Undersegmentation Error of Levinshtein (UE_Levin): " << ue_levin << std::endl;
    std::cout << "Boundary Recall (Rec): " << rec << std::endl;
    std::cout << "Explained Variation (EV): " << ev << std::endl;
    std::cout << "Intra-Cluster Variation (ICV): " << icv << std::endl;
    std::cout << "Sum-of-squared Error RGB (SSE_RGB): " << sse_rgb << std::endl;
    std::cout << "Sum-of-squared Error XY (SSE_XY): " << sse_xy << std::endl;
    std::cout << "Compactness (CO): " << co << std::endl;
    std::cout << "Contour Density (CD): " << cd << std::endl;
    
    return 0;
}

