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

#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <glog/logging.h>
#include <sys/time.h>
#include "io_util.h"
#include "evaluation_summary.h"
#include "parameter_optimization_tool.h"

#define TUPLE(tuple, i) std::get<i>(tuple)

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////

ParameterOptimizationTool::ParameterOptimizationTool(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory, 
        std::string command_line, std::string command_line_parameters) 
        : command_line(command_line), command_line_parameters(command_line_parameters), 
        img_directory(img_directory), gt_directory(gt_directory), 
        base_directory(base_directory) {
    
    evaluation_metrics.ue = false; // Undersegmentation Error
    evaluation_metrics.oe = false; // Oversegmentation Error
    // evaluation_metrics.rec = false; // Boundary Recall
    evaluation_metrics.pre = false; // Boundary Precision
    // evaluation_metrics.ue_np = false; // Undersegmentation Error (Neubert, Protzel))
    evaluation_metrics.ue_levin = false; // Undersegmentation Error (Levinshtein et al.)
    evaluation_metrics.asa = false; // Achievable Segmentation Accuracy
    evaluation_metrics.sse_rgb = false; // Sum-of-Squared Error RGB
    evaluation_metrics.sse_xy = false; // Sum-of-Squared Error XY
    evaluation_metrics.co = true; // Compactness 
    evaluation_metrics.ev = false; // Explained Variation
    evaluation_metrics.mde = false; // Mean Distance to Edge
    evaluation_metrics.icv = false; // Intra-Cluster Variation
    evaluation_metrics.cd = false; // Contour Density
    evaluation_metrics.reg = false; // Regularity
    evaluation_metrics.sp = true; // # Superpixels
    evaluation_metrics.sp_size = false; // Superpixel Size
    
    // evaluation_statistics.mean = false; // Mean/average
    evaluation_statistics.median_and_quartiles = false; //Median, first and third quartile
    evaluation_statistics.min_and_max = false; // Minimum and maximum
    evaluation_statistics.mid_range = false; // Mid range
    evaluation_statistics.std = false; // Standard deviation
    
    superpixels_min = 0;
    superpixels_max = std::numeric_limits<int>::max();
}

////////////////////////////////////////////////////////////////////////////////
// addPostProcessingCommandLine
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool::addPostProcessingCommandLine(std::string command_line) {
    post_processing_command_line = command_line;
}

////////////////////////////////////////////////////////////////////////////////
// addSuperpixelTolerance
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool::addSuperpixelTolerance(int superpixels, int tolerance) {
    superpixels_min = superpixels - tolerance;
    superpixels_max = superpixels + tolerance;
}

////////////////////////////////////////////////////////////////////////////////
// addFloatParamteer
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool::addFloatParameter(std::string name, std::string parameter, 
        std::vector<float> values) {
    
    LOG_IF(FATAL, values.size() == 0) << "Not enough values.";
    
    // @see http://stackoverflow.com/questions/5391973/undefined-reference-to-static-const-int
    int type = FLOAT_PARAMETER;
    std::tuple<std::string, std::string, int, int> parameter_tuple(name, parameter, 
            type, float_parameters.size());
    
    parameters.push_back(parameter_tuple);    
    
//    std::tuple<std::vector<float>, int, float> float_parameter = std::make_tuple<std::vector<float>, int, float>(values, 0, values[0]);
    float_parameters.push_back(std::make_tuple(values, 0, values[0], values[0]));
    
    std::cout << "Added float parameter " << parameter << ": ";
    for (unsigned int i = 0; i < values.size(); i++) {
        std::cout << values[i];
        
        if (i < values.size() - 1) {
            std::cout << ",";
        }
        else {
            std::cout << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// addIntegerParameter
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool:: addIntegerParameter(std::string name, std::string parameter, 
        std::vector<int> values) {
    
    LOG_IF(FATAL, values.size() == 0) << "Not enough values.";
    
    // @see http://stackoverflow.com/questions/5391973/undefined-reference-to-static-const-int
    int type = INTEGER_PARAMETER;
    std::tuple<std::string, std::string, int, int> parameter_tuple(name, parameter, 
            type, integer_parameters.size());
    
    parameters.push_back(parameter_tuple);
    
//    std::tuple<std::vector<int>, int, int> integer_parameter = std::make_tuple<std::vector<int>, int, int>(values, 0, values[0]);
    integer_parameters.push_back(std::make_tuple(values, 0, values[0], values[0]));
    
    std::cout << "Added integer parameter " << parameter << ": ";
    for (unsigned int i = 0; i < values.size(); i++) {
        std::cout << values[i];
        
        if (i < values.size() - 1) {
            std::cout << ",";
        }
        else {
            std::cout << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// useDepth
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool::useDepth(boost::filesystem::path depth_directory_) {
    depth_directory = depth_directory_;
}

////////////////////////////////////////////////////////////////////////////////
// useIntrinsics
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool::useIntrinsics(boost::filesystem::path intrinsics_directory_) {
    intrinsics_directory = intrinsics_directory_;
}

////////////////////////////////////////////////////////////////////////////////
// optimize
////////////////////////////////////////////////////////////////////////////////

int ParameterOptimizationTool::numCombinations() {
    
    int combinations = 1;
    for (unsigned int p = 0; p < parameters.size(); ++p) {
        std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
        
        switch (std::get<2>(parameter_tuple)) {
            case FLOAT_PARAMETER:
            {
                int float_parameter = std::get<3>(parameter_tuple);
                combinations *= TUPLE(float_parameters[float_parameter], 0).size();
                break;
            }
            case INTEGER_PARAMETER:
            {
                int integer_parameter = std::get<3>(parameter_tuple);
                combinations *= TUPLE(integer_parameters[integer_parameter], 0).size();
                break;
            }
            default:
                LOG(FATAL) << "Invalid parameter type.";
                break;
        }
    }
    
    return combinations;
}

////////////////////////////////////////////////////////////////////////////////
// optimize
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool::optimize(float weight, float weight_ue, float weight_co) {
    LOG_IF(FATAL, weight >= 1.0f) << "Invalid UE weight.";
    LOG_IF(FATAL, weight_ue + weight_co >= 1.0f) << "Invalid UE and CO weights.";
    
    int K = numCombinations();
//    std::cout << "Initializing parameters: " << K << "." << std::endl;
    std::cout << "Total: " << K << " combinations." << std::endl;
    
    // Write header.
    std::stringstream output;
    output << "sp_directory" << ",";
    for (unsigned int p = 0; p < parameters.size(); ++p) {
        output << TUPLE(parameters[p], 0) << ",";
    }
    output << "rec_average" << "," << "ue_np_average" << "," 
            << "co_average" << "," << "score" << ","
            << "co_score" << "," << "sp_average" << "\n";
    
    // To store results and parameters only (all are convertible to float).
    int cols = parameters.size() + 6;
    cv::Mat mat_output(0, cols, CV_32FC1, cv::Scalar(0));
    
    float score_max = 0;
    float co_score_max = 0;
    float average_time = 0;
    
    for (int k = 0; k < K; ++k) {
        
        // For estimating remaining time:
        struct timeval tv;
        struct timeval start_tv;

        gettimeofday(&start_tv, NULL);
        
        bool done = false;
        for (int p = parameters.size() - 1; p >= 0; --p) {
            if (done) {
                break;
            }
            
            std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
            switch (std::get<2>(parameter_tuple)) {
                case FLOAT_PARAMETER:
                {
                    int float_parameter = std::get<3>(parameter_tuple);
                    TUPLE(float_parameters[float_parameter], 1)++;
                    
                    if (TUPLE(float_parameters[float_parameter], 1) >= TUPLE(float_parameters[float_parameter], 0).size()) {
                        TUPLE(float_parameters[float_parameter], 1) = 0;
                    }
                    else {
                        done = true;
                    }
                    
                    break;
                }
                case INTEGER_PARAMETER:
                {
                    int integer_parameter = std::get<3>(parameter_tuple);
                    TUPLE(integer_parameters[integer_parameter], 1)++;
                    
                    if (TUPLE(integer_parameters[integer_parameter], 1) >= TUPLE(integer_parameters[integer_parameter], 0).size()) {
                        TUPLE(integer_parameters[integer_parameter], 1) = 0;
                    }
                    else {
                        done = true;
                    }
                    
                    break;
                }
                default:
                    LOG(FATAL) << "[" << k << "] Invalid parameter type (parameter update).";
                    break;
            }
        }
        
        // Build command line.
        boost::filesystem::path sp_directory = base_directory / 
                boost::filesystem::path(std::to_string(time(NULL)));
        std::string command_line_k = command_line + " -i " + img_directory.string();
        
        if (!depth_directory.empty()) {
            command_line_k += " -d " + depth_directory.string();
        }
        
        // When using different intrinsics, we assume this to be SUNRGBD data
        // - those images have not been cropped!
        if (!intrinsics_directory.empty()) {
            command_line_k += " --intrinsics " + intrinsics_directory.string();
            command_line_k += " --cropping-x 0";
            command_line_k += " --cropping-y 0";
        }
        
        command_line_k += " -o " + sp_directory.string();
        
        for (unsigned p = 0; p < parameters.size(); ++p) {
            std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
            
            command_line_k += " " + std::get<1>(parameter_tuple);
            switch (std::get<2>(parameter_tuple)) {
                case FLOAT_PARAMETER:
                {
                    int float_parameter = std::get<3>(parameter_tuple);
                    std::vector<float> float_parameter_values = TUPLE(float_parameters[float_parameter], 0);
                    
                    std::stringstream float_parameter_ss;
                    float_parameter_ss << std::setprecision(6) << float_parameter_values[TUPLE(float_parameters[float_parameter], 1)];
                    
                    command_line_k += " " + float_parameter_ss.str();
                    break;
                }
                case INTEGER_PARAMETER:
                {
                    int integer_parameter = std::get<3>(parameter_tuple);
                    std::vector<int> integer_parameter_values = TUPLE(integer_parameters[integer_parameter], 0);
                    
                    std::stringstream integer_parameter_ss;
                    integer_parameter_ss << std::setprecision(6) << integer_parameter_values[TUPLE(integer_parameters[integer_parameter], 1)];
                    
                    command_line_k += " " + integer_parameter_ss.str();
                    break;
                }
                default:
                    LOG(FATAL) << "Invalid parameter type.";
                    break;
            }
        }
        
        if (!command_line_parameters.empty()) {
            command_line_k += " " + command_line_parameters;
        }
        
//        LOG(INFO) << "[" << k << "] " << command_line_k;
        
        // Run.
        int status = system(command_line_k.c_str());
        
        if (status != 0) {
            LOG(FATAL) << "Command line was not successful: " << command_line_k;
        }
        
        // Post processing:
        if (!post_processing_command_line.empty()) {
            std::string post_processing_command_line_k = post_processing_command_line 
                    + " -i " + sp_directory.string() + " -m " + img_directory.string();

            int status = system(post_processing_command_line_k.c_str());
            
            if (status != 0) {
                LOG(FATAL) << "Post processing command line was not successful: " << post_processing_command_line_k;
            }
        }
        // Evaluation:
//        LOG(INFO) << "[" << k << "] Running evaluation.";
        EvaluationSummary evaluation_summary(sp_directory, gt_directory, img_directory, 
                evaluation_metrics, evaluation_statistics);
        
        int gt_max = 0;
        evaluation_summary.computeSummary(gt_max);
        
        cv::Mat results;
        IOUtil::readMat(sp_directory / boost::filesystem::path("summary.csv.txt"), results);
        
        // Compute average over all ground truths.
        LOG_IF(FATAL, results.cols != (gt_max + 1) + 2) <<  "Invalid number of columns in evaluation results: " 
                << results.cols << " != " << (gt_max + 1) + 2;
        // Rec on first row, UE on second, superpixel number of third.
        LOG_IF(FATAL, results.rows != 4) <<  "Invalid number of rows in evaluation results: " << results.rows << " != 4";
        
        float rec_average = 0;
        float ue_np_average = 0;
        float co_average = 0;
        float sp_average = 0;
        
        for (int j = 0; j < gt_max + 1; ++j) {
            rec_average += results.at<float>(0, j);
            ue_np_average += results.at<float>(1, j);
            co_average += results.at<float>(2, j);
            sp_average += results.at<float>(3, j);
        }
        
        rec_average /= (gt_max + 1);
        ue_np_average /= (gt_max + 1);
        co_average /= (gt_max + 1);
        sp_average /= (gt_max + 1);
        
        float score = 0;
        float co_score = 0;
        
        // Only consider for best parameters if superpixel tolerance is met!
        if (sp_average >= superpixels_min && sp_average <= superpixels_max) {
            
            score = (1 - weight)*rec_average + weight*(1 - ue_np_average);
            if (score > score_max) {
                score_max = score;

                // Save current parameters.
                for (unsigned p = 0; p < parameters.size(); ++p) {

                    std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
                    switch (std::get<2>(parameter_tuple)) {
                        case FLOAT_PARAMETER:
                        {
                            int float_parameter = std::get<3>(parameter_tuple);
                            std::vector<float> float_parameter_values = TUPLE(float_parameters[float_parameter], 0);

                            // 2!
                            TUPLE(float_parameters[float_parameter], 2) = float_parameter_values[TUPLE(float_parameters[float_parameter], 1)];
                            break;
                        }
                        case INTEGER_PARAMETER:
                        {
                            int integer_parameter = std::get<3>(parameter_tuple);
                            std::vector<int> integer_parameter_values = TUPLE(integer_parameters[integer_parameter], 0);

                            // 2!
                            TUPLE(integer_parameters[integer_parameter], 2) = integer_parameter_values[TUPLE(integer_parameters[integer_parameter], 1)];
                            break;
                        }
                        default:
                            LOG(FATAL) << "[" << k << "] Invalid parameter type (best value).";
                            break;
                    }
                }
            }

            co_score = (1 - weight_ue - weight_co)*rec_average 
                    + weight_ue*(1 - ue_np_average) + weight_co*(co_average);

            if (co_score > co_score_max) {
                co_score_max = co_score;

                // Save current parameters.
                for (unsigned p = 0; p < parameters.size(); ++p) {

                    std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
                    switch (std::get<2>(parameter_tuple)) {
                        case FLOAT_PARAMETER:
                        {
                            int float_parameter = std::get<3>(parameter_tuple);
                            std::vector<float> float_parameter_values = TUPLE(float_parameters[float_parameter], 0);

                            // 3!
                            TUPLE(float_parameters[float_parameter], 3) = float_parameter_values[TUPLE(float_parameters[float_parameter], 1)];
                            break;
                        }
                        case INTEGER_PARAMETER:
                        {
                            int integer_parameter = std::get<3>(parameter_tuple);
                            std::vector<int> integer_parameter_values = TUPLE(integer_parameters[integer_parameter], 0);

                            // 3!
                            TUPLE(integer_parameters[integer_parameter], 3) = integer_parameter_values[TUPLE(integer_parameters[integer_parameter], 1)];
                            break;
                        }
                        default:
                            LOG(FATAL) << "[" << k << "] Invalid parameter type (best value).";
                            break;
                    }
                }
            }
        }
        
//        LOG(INFO) << "[" << k << "] Updating CSV output.";
        output << sp_directory.string() << ",";
        cv::Mat mat_row(1, cols, CV_32FC1, cv::Scalar(0));
        
        for (unsigned p = 0; p < parameters.size(); ++p) {
            
            std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
            switch (std::get<2>(parameter_tuple)) {
                case FLOAT_PARAMETER:
                {
                    int float_parameter = std::get<3>(parameter_tuple);
                    std::vector<float> float_parameter_values = TUPLE(float_parameters[float_parameter], 0);
                        
                    output << float_parameter_values[TUPLE(float_parameters[float_parameter], 1)] << ",";
                    mat_row.at<float>(0, p) = float_parameter_values[TUPLE(float_parameters[float_parameter], 1)];
                    break;
                }
                case INTEGER_PARAMETER:
                {
                    int integer_parameter = std::get<3>(parameter_tuple);
                        std::vector<int> integer_parameter_values = TUPLE(integer_parameters[integer_parameter], 0);
                    
                    output << integer_parameter_values[TUPLE(integer_parameters[integer_parameter], 1)] << ",";
                    mat_row.at<float>(0, p) = integer_parameter_values[TUPLE(integer_parameters[integer_parameter], 1)];
                    break;
                }
                default:
                    LOG(FATAL) << "Invalid parameter type.";
                    break;
            }
        }
        
        output << rec_average << "," << ue_np_average << "," << co_average << "," 
                << score << "," << co_score << "," << sp_average << "\n";
        
        mat_row.at<float>(0, parameters.size()) = rec_average;
        mat_row.at<float>(0, parameters.size() + 1) = ue_np_average;
        mat_row.at<float>(0, parameters.size() + 2) = co_average;
        mat_row.at<float>(0, parameters.size() + 3) = score;
        mat_row.at<float>(0, parameters.size() + 4) = co_score;
        mat_row.at<float>(0, parameters.size() + 5) = sp_average;
        mat_output.push_back(mat_row);
        
        // Clean up superpixel directory!
        this->cleanUp(sp_directory);
        
        gettimeofday(&tv, NULL);
        average_time += (tv.tv_sec - start_tv.tv_sec) +
          (tv.tv_usec - start_tv.tv_usec) / 1000000.0;
        
        if (k%10 == 0) {
            if (k != 0) {
                std::cout << std::endl;
            }
            
            std::cout << "Time remaining: " << average_time/(k + 1)*(K - k - 1) 
                    << " (" << average_time/(k + 1) << ") " << std::flush;
        }
        
        // Show progress ...
        std::cout << "." << std::flush;
    }
    
    // Write best parameters.
    output << "best";
    for (unsigned p = 0; p < parameters.size(); ++p) {

        std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
        switch (std::get<2>(parameter_tuple)) {
            case FLOAT_PARAMETER:
            {
                int float_parameter = std::get<3>(parameter_tuple);
                output << "," << TUPLE(float_parameters[float_parameter], 2); // 2!
                break;
            }
            case INTEGER_PARAMETER:
            {
                int integer_parameter = std::get<3>(parameter_tuple);
                output << "," << TUPLE(integer_parameters[integer_parameter], 2); // 2!
                break;
            }
            default:
                LOG(FATAL) << "Invalid parameter type.";
                break;
        }
    }
    
    output << "\n" << "co_best";
    for (unsigned p = 0; p < parameters.size(); ++p) {

        std::tuple<std::string, std::string, int, int> parameter_tuple = parameters[p];
        switch (std::get<2>(parameter_tuple)) {
            case FLOAT_PARAMETER:
            {
                int float_parameter = std::get<3>(parameter_tuple);
                output << "," << TUPLE(float_parameters[float_parameter], 3); // 3!
                break;
            }
            case INTEGER_PARAMETER:
            {
                int integer_parameter = std::get<3>(parameter_tuple);
                output << "," << TUPLE(integer_parameters[integer_parameter], 3); // 3!
                break;
            }
            default:
                LOG(FATAL) << "Invalid parameter type.";
                break;
        }
    }
    
//    LOG(INFO) << "Writing output to CSV.";
    boost::filesystem::path parameter_optimization_file = base_directory 
            / boost::filesystem::path("parameter_optimization.csv");
    std::ofstream csv_file(parameter_optimization_file.string());
    csv_file << output.str();
    csv_file.close();
    
    boost::filesystem::path parameter_optimization_mat = base_directory
            / boost::filesystem::path("parameter_optimization.csv.txt");
    IOUtil::writeMat(parameter_optimization_mat, mat_output);
    
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// cleanUp
////////////////////////////////////////////////////////////////////////////////

void ParameterOptimizationTool::cleanUp(const boost::filesystem::path &sp_directory) {
    if (boost::filesystem::exists(sp_directory)) {
        
        boost::filesystem::directory_iterator end_it;
        for (boost::filesystem::directory_iterator i(sp_directory); i != end_it; ++i) {
                    
            if (i->path().extension().string() == ".csv") {
                if (i->path().stem().string() != "results" && i->path().stem().string() != "results.csv"
                        && i->path().stem().string() != "summary" && i->path().stem().string() != "summary.csv"
                        && i->path().stem().string() != "correlation" && i->path().stem().string() != "correlation.csv"
                        && i->path().stem().string() != "parameter_optimization") {

                    boost::filesystem::remove(i->path());
                }
            }
        }
    }
}