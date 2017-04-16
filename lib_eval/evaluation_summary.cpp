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

#include <sstream>
#include <fstream>
#include <limits>
#include <glog/logging.h>
#include "visualization.h"
#include "evaluation.h"
#include "io_util.h"
#include "evaluation_summary.h"

////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////

EvaluationSummary::EvaluationSummary(boost::filesystem::path sp_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path img_directory)
        : compute_correlation(false), sp_directory(sp_directory), gt_directory(gt_directory), 
        img_directory(img_directory) {
    
    results_file = sp_directory / boost::filesystem::path("results.csv");
    correlation_file = sp_directory / boost::filesystem::path("correlation.csv");
    summary_file = sp_directory / boost::filesystem::path("summary.csv");
    vis_directory = sp_directory / boost::filesystem::path("vis/");
    
    if (!boost::filesystem::is_directory(vis_directory) && !vis_directory.empty()
            && superpixel_visualizations.any()) {
        boost::filesystem::create_directories(vis_directory);
    }
}

EvaluationSummary::EvaluationSummary(boost::filesystem::path sp_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path img_directory,
        EvaluationMetrics evaluation_metrics, EvaluationStatistics evaluation_statistics)
        : evaluation_metrics(evaluation_metrics), evaluation_statistics(evaluation_statistics), 
        compute_correlation(false), sp_directory(sp_directory), gt_directory(gt_directory), 
        img_directory(img_directory) {
    
    results_file = sp_directory / boost::filesystem::path("results.csv");
    correlation_file = sp_directory / boost::filesystem::path("correlation.csv");
    summary_file = sp_directory / boost::filesystem::path("summary.csv");
    vis_directory = sp_directory / boost::filesystem::path("vis/");
    
    if (!boost::filesystem::is_directory(vis_directory) && !vis_directory.empty()
            && superpixel_visualizations.any()) {
        boost::filesystem::create_directories(vis_directory);
    }
}

EvaluationSummary::EvaluationSummary(boost::filesystem::path sp_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path img_directory,
        EvaluationMetrics evaluation_metrics, EvaluationStatistics evaluation_statistics,
        SuperpixelVisualizations superpixel_visualizations)
        : evaluation_metrics(evaluation_metrics), evaluation_statistics(evaluation_statistics),
        superpixel_visualizations(superpixel_visualizations), compute_correlation(false),
        sp_directory(sp_directory), gt_directory(gt_directory), img_directory(img_directory){
    
    results_file = sp_directory / boost::filesystem::path("results.csv");
    correlation_file = sp_directory / boost::filesystem::path("correlation.csv");
    summary_file = sp_directory / boost::filesystem::path("summary.csv");
    vis_directory = sp_directory / boost::filesystem::path("vis/");
    
    if (!boost::filesystem::is_directory(vis_directory) && !vis_directory.empty()
            && superpixel_visualizations.any()) {
        boost::filesystem::create_directories(vis_directory);
    }
}

////////////////////////////////////////////////////////////////////////////////
// countMetrics
////////////////////////////////////////////////////////////////////////////////

int EvaluationSummary::countMetrics() {
    
    int count = 0;
    if (evaluation_metrics.ue) {
        ++count;
    }
    if (evaluation_metrics.oe) {
        ++count;
    }
    if (evaluation_metrics.rec) {
        ++count;
    }
    if (evaluation_metrics.pre) {
        ++count;
    }
    if (evaluation_metrics.ue_np) {
        ++count;
    }
    if (evaluation_metrics.ue_levin) {
        ++count;
    }
    if (evaluation_metrics.asa) {
        ++count;
    }
    if (evaluation_metrics.sse_rgb) {
        ++count;
    }
    if (evaluation_metrics.sse_xy) {
        ++count;
    }
    if (evaluation_metrics.co) {
        ++count;
    }
    if (evaluation_metrics.ev) {
        ++count;
    }
    if (evaluation_metrics.mde) {
        ++count;
    }
    if (evaluation_metrics.icv) {
        ++count;
    }
    if (evaluation_metrics.cd) {
        ++count;
    }
    if (evaluation_metrics.reg) {
        ++count;
    }
    if (evaluation_metrics.sp) {
        ++count;
    }
    if (evaluation_metrics.sp_size) {
        ++count; // average
        ++count; // variance
    }
    
    return count;
}

////////////////////////////////////////////////////////////////////////////////
// evaluateHeader
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::evaluateHeader(std::stringstream &output, 
        std::vector<std::string> &metric_order) {
    
    output << "image" << "," << "ground_truth"; 
    if (evaluation_metrics.ue) {
        output << "," << "ue";
        
        metric_order.push_back("ue");
    }
    if (evaluation_metrics.oe) {
        output << "," << "oe";
        metric_order.push_back("oe");
    }
    if (evaluation_metrics.rec) {
        output << "," << "rec";
        metric_order.push_back("rec");
    }
    if (evaluation_metrics.pre) {
        output << "," << "pre";
        metric_order.push_back("pre");
    }
    if (evaluation_metrics.ue_np) {
        output << "," << "ue_np";
        metric_order.push_back("ue_np");
    }
    if (evaluation_metrics.ue_levin) {
        output << "," << "ue_levin";
        metric_order.push_back("ue_levin");
    }
    if (evaluation_metrics.asa) {
        output << "," << "asa";
        metric_order.push_back("asa");
    }
    if (evaluation_metrics.sse_rgb) {
        output << "," << "sse_rgb";
        metric_order.push_back("sse_rgb");
    }
    if (evaluation_metrics.sse_xy) {
        output << "," << "sse_xy";
        metric_order.push_back("sse_xy");
    }
    if (evaluation_metrics.co) {
        output << "," << "co";
        metric_order.push_back("co");
    }
    if (evaluation_metrics.ev) {
        output << "," << "ev";
        metric_order.push_back("ev");
    }
    if (evaluation_metrics.mde) {
        output << "," << "mde";
        metric_order.push_back("mde");
    }
    if (evaluation_metrics.icv) {
        output << "," << "icv";
        metric_order.push_back("icv");
    }
    if (evaluation_metrics.cd) {
        output << "," << "cd";
        metric_order.push_back("cd");
    }
    if (evaluation_metrics.reg) {
        output << "," << "reg";
        metric_order.push_back("reg");
    }
    if (evaluation_metrics.sp) {
        output << "," << "sp";
        metric_order.push_back("sp");
    }
    if (evaluation_metrics.sp_size) {
        output << "," << "sp_size" << "," << "sp_size_variation";
        metric_order.push_back("sp_size");
        metric_order.push_back("sp_size_variation");
    }
    
    output << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// evaluate
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::evaluate(const cv::Mat &sp_segmentation, const cv::Mat &gt_segmentation, 
        const cv::Mat &image, cv::Mat &data, std::stringstream &output) {
    
    int i = 0;
    cv::Mat row(1, countMetrics(), CV_32FC1, cv::Scalar(0));
    
    std::string separator = "";
    if (evaluation_metrics.ue) {
//        LOG(INFO) << "... Computing Undersegmentation Error.";
        row.at<float>(0, i) = Evaluation::computeUndersegmentationError(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.oe) {
//        LOG(INFO) << "... Computing Oversegmentation Error.";
        row.at<float>(0, i) = Evaluation::computeOversegmentationError(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.rec) {
//        LOG(INFO) << "... Computing Boundary Recall.";
        row.at<float>(0, i) = Evaluation::computeBoundaryRecall(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.pre) {
//        LOG(INFO) << "... Computing Boundary Precision.";
        row.at<float>(0, i) = Evaluation::computeBoundaryPrecision(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.ue_np) {
//        LOG(INFO) << "... Computing NP Undersegmentation Error.";
        row.at<float>(0, i) = Evaluation::computeNPUndersegmentationError(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.ue_levin) {
//        LOG(INFO) << "... Computing Levin Undersegmentation Error.";
        row.at<float>(0, i) = Evaluation::computeLevinUndersegmentationError(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.asa) {
//        LOG(INFO) << "... Computing Achievable Segmentation Accuracy.";
        row.at<float>(0, i) = Evaluation::computeAchievableSegmentationAccuracy(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.sse_rgb) {
//        LOG(INFO) << "... Computing Sum-Of-Squared Error RGB.";
        row.at<float>(0, i) = Evaluation::computeSumOfSquaredErrorRGB(sp_segmentation, 
                image);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.sse_xy) {
//        LOG(INFO) << "... Computing Sum-Of-Squared Error XY.";
        row.at<float>(0, i) = Evaluation::computeSumOfSquaredErrorXY(sp_segmentation,
                image);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.co) {
//        LOG(INFO) << "... Computing Compactness.";
        row.at<float>(0, i) = Evaluation::computeCompactness(sp_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.ev) {
//        LOG(INFO) << "... Computing Explained Variation.";
        row.at<float>(0, i) = Evaluation::computeExplainedVariation(sp_segmentation, 
                image);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.mde) {
//        LOG(INFO) << "... Computing Mean Distance To Edge.";
        row.at<float>(0, i) = Evaluation::computeMeanDistanceToEdge(sp_segmentation, 
                gt_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.icv) {
//        LOG(INFO) << "... Computing Intra Cluster Variation.";
        row.at<float>(0, i) = Evaluation::computeIntraClusterVariation(sp_segmentation, 
                image);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.cd) {
//        LOG(INFO) << "... Computing Contour Density.";
        row.at<float>(0, i) = Evaluation::computeContourDensity(sp_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.reg) {
//        LOG(INFO) << "... Computing Regularity.";
        row.at<float>(0, i) = Evaluation::computeRegularity(sp_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.sp) {
//        LOG(INFO) << "... Computing Superpixels.";
        row.at<float>(0, i) = Evaluation::computeSuperpixels(sp_segmentation);
        
        output << separator << row.at<float>(0, i);
        separator = ",";
        ++i;
    }
    if (evaluation_metrics.sp_size) {
//        LOG(INFO) << "... Computing Superpixel Sizes.";
        
        float average_size;
        int min_size;
        int max_size;
        float size_variation;
        Evaluation::computeSuperpixelSizes(sp_segmentation, 
                average_size, min_size, max_size, size_variation);
        
        row.at<float>(0, i) = average_size;
        output << separator << row.at<float>(0, i);
        ++i;
        
        row.at<float>(0, i) = size_variation;
        output << separator << row.at<float>(0, i);
        ++i;
        
        separator = ",";
    }
    
    output << "\n";
//    if (data.empty()) {
//        data = row;
//    }
//    else {
//        cv::vconcat(data, row, data);
//    }
    data.push_back(row);
}

////////////////////////////////////////////////////////////////////////////////
// visualize
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::visualize(const cv::Mat &sp_segmentation, const cv::Mat &gt_segmentation,
        const cv::Mat &image, std::string name, int t) {
    
    if (t == 0) {
        if (superpixel_visualizations.contour) {
            cv::Mat contours;
            Visualization::drawContours(image, sp_segmentation, contours);
            cv::imwrite(vis_directory.string() + name + "_contour.png", contours);
        }
        if (superpixel_visualizations.mean) {
            cv::Mat means;
            Visualization::drawMeans(image, sp_segmentation, means);
            cv::imwrite(vis_directory.string() + name + "_mean.png", means);
        }
        if (superpixel_visualizations.random) {
            cv::Mat random;
            Visualization::drawRandom(sp_segmentation, random);

            cv::imwrite(vis_directory.string() + name + "_random.png", random);
        }
        if (superpixel_visualizations.perturbed_mean) {
            cv::Mat perturbed_mean;
            Visualization::drawPerturbedMeans(image, sp_segmentation, perturbed_mean);

            cv::imwrite(vis_directory.string() + name + "_perturbed_means.png", perturbed_mean);
        }
    }
    
    if (superpixel_visualizations.pre_rec) {
        cv::Mat pre_rec;
            Visualization::drawContours(image, sp_segmentation, pre_rec);
        Visualization::drawPrecisionRecall(pre_rec, sp_segmentation, 
                gt_segmentation, pre_rec);
        cv::imwrite(vis_directory.string() + name + "-" + std::to_string(t) + "_pre_rec.png", pre_rec);
    }
    if (superpixel_visualizations.ue) {
        cv::Mat ue;
        Visualization::drawUndersegmentationError(image, sp_segmentation, 
                gt_segmentation, ue);
        Visualization::drawContours(ue, sp_segmentation, ue);
        
        cv::imwrite(vis_directory.string() + name + "-" + std::to_string(t) + "_ue.png", ue);
    }
}

////////////////////////////////////////////////////////////////////////////////
// correlate
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::correlate(const cv::Mat &mat_results, const std::vector<std::string> &metric_order,
        cv::Mat &mat_correlation, std::string &csv_correlation) {
    
    LOG_IF(FATAL, mat_results.cols != (int) metric_order.size()) 
            << "Invalid metric order:" << mat_results.cols << " != " << metric_order.size();
    
    std::vector<float> sums(mat_results.cols, 0);
    std::vector< std::vector<float> > squared_sums(mat_results.cols);
    for (int j = 0; j < mat_results.cols; j++) {
        squared_sums[j].resize(mat_results.cols, 0);
    }
    
    for (int i = 0; i < mat_results.rows; i++) {
        for (int j = 0; j < mat_results.cols; j++) {
            sums[j] += mat_results.at<float>(i, j);
            
            for (int jj = 0; jj < mat_results.cols; jj++) {
                squared_sums[j][jj] += mat_results.at<float>(i, j)*mat_results.at<float>(i, jj);
            }
        }
    }
    
    for (int j = 0; j < mat_results.cols; j++) {
        sums[j] /= mat_results.rows;
    }
    
    for (int j = 0; j < mat_results.cols; j++) {
        for (int jj = 0; jj < mat_results.cols; jj++) {
            squared_sums[j][jj] /= mat_results.rows;
            squared_sums[j][jj] -= sums[j]*sums[jj];
        }
    }
    
    for (int j = 0; j < mat_results.cols; j++) {
        for (int jj = 0; jj < mat_results.cols; jj++) {
            LOG_IF(ERROR, squared_sums[j][jj] != squared_sums[jj][j] || std::isnan(squared_sums[j][jj])) << squared_sums[j][jj] << " != " << squared_sums[jj][j];
        }
    }
    
    csv_correlation = "";
    for (int j = 0; j < mat_results.cols; j++) {
        csv_correlation += "," + metric_order[j];
    }
    
    csv_correlation += "\n";
    
    mat_correlation.create(mat_results.cols, mat_results.cols, CV_32FC1);
    for (int j = 0; j < mat_results.cols; j++) {
        csv_correlation += metric_order[j];
        
        for (int jj = 0; jj < mat_results.cols; jj++) {
            mat_correlation.at<float>(j, jj) = squared_sums[j][jj] / (std::sqrt(squared_sums[j][j])*std::sqrt(squared_sums[jj][jj]));
            csv_correlation += "," + std::to_string(mat_correlation.at<float>(j, jj));
        }
        
        csv_correlation += "\n";
    }
}

////////////////////////////////////////////////////////////////////////////////
// countStatistics
////////////////////////////////////////////////////////////////////////////////

int EvaluationSummary::countStatistics(int gt_max) {
    
    int count = 0;
    if (evaluation_statistics.mean) {
        count += gt_max + 2;
        
        if (evaluation_statistics.std) {
            count += gt_max + 2;
        }
    }
    if (evaluation_statistics.median_and_quartiles) {
        count += 3*gt_max + 6;
    }
    if (evaluation_statistics.min_and_max) {
        count += 2*gt_max + 4;
    }
    
    return count;
}

////////////////////////////////////////////////////////////////////////////////
// validateStatistics
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::validateStatistics() {
    if (evaluation_statistics.std) {
        evaluation_statistics.mean = true;
    }
    if (evaluation_statistics.mid_range) {
        evaluation_statistics.min_and_max = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// summaryHeader
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::summaryHeader(const std::vector<int> &gt, std::stringstream &output) {
    
    int max = *std::max_element(gt.begin(), gt.end());
    
    output << "metric";
    if (evaluation_statistics.mean) {
        for (unsigned int i = 0; i < max + 1; ++i) {
            output << "," << "mean[" << i << "]";
        }
        
        output << "," << "mean_min";
        output << "," << "mean_max";
        
        if (evaluation_statistics.std) {
            for (unsigned int i = 0; i < max + 1; ++i) {
                output << "," << "std[" << i << "]";
            }
            
            output << "," << "std_min";
            output << "," << "std_max";
        }
    }
    if (evaluation_statistics.median_and_quartiles) {
        for (unsigned int i = 0; i < max + 1; ++i) {
            output << "," << "median[" << i << "]";
            output << "," << "first_q[" << i << "]";
            output << "," << "third_q[" << i << "]";
        }
        
        output << "," << "median_min";
        output << "," << "first_q_min";
        output << "," << "third_q_min";
        
        output << "," << "median_max";
        output << "," << "first_q_max";
        output << "," << "third_q_max";
    }
    if (evaluation_statistics.min_and_max) {
        for (unsigned int i = 0; i < max + 1; ++i) {
            output << "," << "min[" << i << "]";
            output << "," << "max[" << i << "]";
        }
        
        output << "," << "min_min";
        output << "," << "max_min";
        
        output << "," << "min_max";
        output << "," << "max_max";
    }
    
    output << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// summarize
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::summarize(const std::vector<int> &gt, const cv::Mat &data, 
        int j, cv::Mat &mat_summary, std::stringstream &output) {
    
    LOG_IF(FATAL, gt.size() != data.rows) << "The ground truth indices do not match the number of values after evaluation.";
    
    int k = 0;
    int gt_max = *std::max_element(gt.begin(), gt.end());
    cv::Mat row(1, countStatistics(gt_max + 1), CV_32FC1, cv::Scalar(0));
    
    std::string separator = "";
    if (evaluation_statistics.mean) {
//        LOG(INFO) << "... Computing mean.";
        
        std::vector<float> mean;
        float mean_min;
        float mean_max;
        computeMean(gt, data, j, mean, mean_min, mean_max);
        
        for (unsigned int i = 0; i < mean.size(); ++i) {
            output << separator << mean[i];
            row.at<float>(0, k) = mean[i];
            ++k;
            
            if (i == 0) {
                separator = ",";
            }
        }
        
        output << separator << mean_min;
        output << separator << mean_max;
        
        row.at<float>(0, k) = mean_min;
        ++k;
        row.at<float>(0, k) = mean_max;
        ++k;
        
        if (evaluation_statistics.std) {
            std::vector<float> std;
            float std_min;
            float std_max;
            computeStandardDeviation(gt, data, j, mean, std, std_min, std_max);

            for (unsigned int i = 0; i < mean.size(); ++i) {
                output << separator << std[i];
                row.at<float>(0, k) = std[i];
                ++k;

                if (i == 0) {
                    separator = ",";
                }
            }

            output << separator << std_min;
            output << separator << std_max;

            row.at<float>(0, k) = std_min;
            ++k;
            row.at<float>(0, k) = std_max;
            ++k;
        }
    }
    if (evaluation_statistics.median_and_quartiles) {
//        LOG(INFO) << "... Computing median and quartiles.";
        
        std::vector<float> median;
        std::vector<float> first_q;
        std::vector<float> third_q;
        float median_min;
        float first_q_min;
        float third_q_min;
        float median_max;
        float first_q_max;
        float third_q_max;
        
        comuteMedianFirstAndThirdQuartile(gt, data, j, median, first_q, 
                third_q, median_min, first_q_min, third_q_min, 
                median_max, first_q_max, third_q_max);
        
        LOG_IF(FATAL, median.size() != first_q.size()) << "Computed #medians do not match the computed #first quartiles.";
        LOG_IF(FATAL, median.size() != first_q.size()) << "Computed #medians do not match the computed #third quartiles.";
        
        for (unsigned int i = 0; i < median.size(); ++i) {
            output << separator << median[i];
            
            if (i == 0) {
                separator = ",";
            }
            
            output << separator << first_q[i];
            output << separator << third_q[i];
            
            row.at<float>(0, k) = median[i];
            ++k;
            row.at<float>(0, k) = first_q[i];
            ++k;
            row.at<float>(0, k) = third_q[i];
            ++k;
        }
        
        output << separator << median_min;
        output << separator << first_q_min;
        output << separator << third_q_min;
        
        row.at<float>(0, k) = median_min;
        ++k;
        row.at<float>(0, k) = first_q_min;
        ++k;
        row.at<float>(0, k) = third_q_min;
        ++k;
        
        output << separator << median_max;
        output << separator << first_q_max;
        output << separator << third_q_max;
        
        row.at<float>(0, k) = median_max;
        ++k;
        row.at<float>(0, k) = first_q_max;
        ++k;
        row.at<float>(0, k) = third_q_max;
        ++k;
    }
    if (evaluation_statistics.min_and_max) {
//        LOG(INFO) << "... Computing min and max.";
        
        std::vector<float> min;
        std::vector<float> max;
        float min_min;
        float max_min;
        float min_max;
        float max_max;
        comuteMinMax(gt, data, j, min, max, min_min, max_min, min_max, max_max);
        
        LOG_IF(FATAL, min.size() != max.size()) << "Computed #maxima do not match the computed #minima.";
        
        for (unsigned int i = 0; i < min.size(); ++i) {
            output << separator << min[i];
            
            if (i == 0) {
                separator = ",";
            }
            
            output << separator << max[i];
            
            row.at<float>(0, k) = min[i];
            ++k;
            row.at<float>(0, k) = max[i];
            ++k;
        }
        
        output << separator << min_min;
        output << separator << max_min;
        
        row.at<float>(0, k) = min_min;
        ++k;
        row.at<float>(0, k) = max_min;
        ++k;
        
        output << separator << min_max;
        output << separator << max_max;
        
        row.at<float>(0, k) = min_max;
        ++k;
        row.at<float>(0, k) = max_max;
        ++k;
    }
    
    output << "\n";
    mat_summary.push_back(row);
}

////////////////////////////////////////////////////////////////////////////////
// computeMean
////////////////////////////////////////////////////////////////////////////////

float EvaluationSummary::computeMean(const std::vector<int> gt, const cv::Mat &data, 
        int j, std::vector<float> &mean, float &mean_min, float &mean_max) {
    
    LOG_IF(FATAL, gt.size() != data.rows) << "The ground truth indices do not match the number of values after evaluation.";
    LOG_IF(FATAL, j < 0 || j >= data.cols) << "Cannot summarize invalid column.";
    
    int max_gt = *std::max_element(gt.begin(), gt.end());
    std::vector<int> count(max_gt + 1, 0);
    mean.resize(max_gt + 1, 0);
    
    for (int i = 0; i < data.rows; ++i) {
        mean[gt[i]] += data.at<float>(i, j);
        ++count[gt[i]];
    }
    
    for (int i = 0; i < max_gt + 1; ++i) {
        if (count[i] > 0) {
            mean[i] /= count[i];
        }
    }
    
    mean_min = -1;
    mean_max = -1;
    
    if (max_gt > 0) {
        
        int count_0 = 0;
        
        mean_min = 0;
        mean_max = 0;
        
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        
        for (int i = 0; i < data.rows; ++i) {
            
            // New data point, so reset min/max and save old ones.
            if (gt[i] == 0 && i != 0) {
                mean_min += min;
                mean_max += max;
                
                min = std::numeric_limits<float>::max();
                max = std::numeric_limits<float>::min();
                
                ++count_0;
            }
            
            if (data.at<float>(i, j) < min) {
                min = data.at<float>(i, j);
            }
            if (data.at<float>(i, j) > max) {
                max = data.at<float>(i, j);
            }
        }
        
        mean_min /= count_0;
        mean_max /= count_0;
    }
    
    return mean[0];
}

////////////////////////////////////////////////////////////////////////////////
// computeMedianFirstAndThirdQuantile
////////////////////////////////////////////////////////////////////////////////

float pQuartile(float p, const std::vector<float> data) {
    LOG_IF(FATAL, p > 1 || p < 0) << "Cannot compute p-quartile for p > 1 or p < 0.";
    
    int size = data.size();
    if (p == 0.5) {
        if (size%2 == 0) {
            return (data[size/2] + data[size/2 + 1])/2;
        }
        else {
            return data[size/2];
        }
    }
    else {
        return (data[std::floor(p*size)] + data[std::ceil(p*size)])/2;
    }
}

void EvaluationSummary::comuteMedianFirstAndThirdQuartile(const std::vector<int> gt, 
        const cv::Mat &data, int j, std::vector<float> &median, std::vector<float> &first_q, 
        std::vector<float> &third_q, float &median_min, float &first_q_min, float &third_q_min, 
        float &median_max, float &first_q_max, float &third_q_max) {
    
    LOG_IF(FATAL, gt.size() != data.rows) << "The ground truth indices do not match the number of values after evaluation.";
    LOG_IF(FATAL, j < 0 || j >= data.cols) << "Cannot summarize invalid column.";
    
    int max_gt = *std::max_element(gt.begin(), gt.end());

    // Save per gt a sorted list of values.
    // Last two elements store min and max respectively.
    std::vector< std::vector<float> > gt_sorted(max_gt + 1);
    for (int i = 0; i < data.rows; ++i) {
        gt_sorted[gt[i]].push_back(data.at<float>(i, j));
    }
    
    if (max_gt > 0) {
        std::vector<float> min_sorted;
        std::vector<float> max_sorted;
        
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        
        for (int i = 0; i < data.rows; ++i) {
            
            // New data point, so reset min/max and save old ones.
            if (gt[i] == 0 && i != 0) {
                min_sorted.push_back(min);
                max_sorted.push_back(max);
                
                min = std::numeric_limits<float>::max();
                max = std::numeric_limits<float>::min();
            }
            
            if (data.at<float>(i, j) < min) {
                min = data.at<float>(i, j);
            }
            if (data.at<float>(i, j) > max) {
                max = data.at<float>(i, j);
            }
        }
        
        gt_sorted.push_back(min_sorted);
        gt_sorted.push_back(max_sorted);
    }
    
    median.clear();
    first_q.clear();
    third_q.clear();
    
    median_min = -1;
    first_q_min = -1;
    third_q_min = -1;
    median_max = -1;
    first_q_max = -1;
    third_q_max = -1;
    
    for (int i = 0; i < gt_sorted.size(); ++i) {
        
        // Sort all values belonging to the i-th ground truth.
        std::sort(gt_sorted[i].begin(), gt_sorted[i].end());
        
        float median_i = pQuartile(0.5, gt_sorted[i]);
        float first_q_i = pQuartile(0.25, gt_sorted[i]);
        float third_q_i = pQuartile(0.75, gt_sorted[i]);
        
        if (i <= max_gt) {
            median.push_back(median_i);
            first_q.push_back(first_q_i);
            third_q.push_back(third_q_i);
        }
        else if (i == max_gt + 1){
            median_min = median_i;
            first_q_min = first_q_i;
            third_q_min = third_q_i;
        }
        else if (i == max_gt + 2) {
            median_max = median_i;
            first_q_max = first_q_i;
            third_q_max = third_q_i;
        }
        else {
            LOG(FATAL) << "This should not happen!";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// computeMinMax
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::comuteMinMax(const std::vector<int> gt, const cv::Mat &data, int j,
        std::vector<float> &min, std::vector<float> &max, float &min_min,
        float &max_min, float &min_max, float&max_max) {
    
    LOG_IF(FATAL, gt.size() != data.rows) << "The ground truth indices do not match the number of values after evaluation.";
    LOG_IF(FATAL, j < 0 || j >= data.cols) << "Cannot summarize invalid column.";
    
    int max_gt = *std::max_element(gt.begin(), gt.end());
    min.resize(max_gt + 1, std::numeric_limits<float>::max());
    max.resize(max_gt + 1, std::numeric_limits<float>::min());
    
    for (int i = 0; i < data.rows; ++i) {
        if (data.at<float>(i, j) > max[gt[i]]) {
            max[gt[i]] = data.at<float>(i, j);
        }
        if (data.at<float>(i, j) < min[gt[i]]) {
            min[gt[i]] = data.at<float>(i, j);
        }
    }
    
    min_min = -1;
    max_min = -1;
    min_max = -1;
    max_max = -1;
    
    if (max_gt > 0) {
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        
        for (int i = 0; i < data.rows; ++i) {
            
            // New data point, so reset min/max and save old ones.
            if (gt[i] == 0 && i != 0) {
                if (min < min_min) {
                    min_min = min;
                }
                
                if (min > min_max) {
                    min_max = min;
                }
                
                if (max < max_min) {
                    max_min = max;
                }
                
                if (max > max_max) {
                    max_max = max;
                }
                
                min = std::numeric_limits<float>::max();
                max = std::numeric_limits<float>::min();
            }
            
            if (data.at<float>(i, j) < min) {
                min = data.at<float>(i, j);
            }
            if (data.at<float>(i, j) > max) {
                max = data.at<float>(i, j);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// computeStandardDeviation
////////////////////////////////////////////////////////////////////////////////

float EvaluationSummary::computeStandardDeviation(const std::vector<int> gt, const cv::Mat &data, int j,
        const std::vector<float> &mean, std::vector<float> &std, float &std_min,
        float &std_max) {
    
    LOG_IF(FATAL, gt.size() != data.rows) << "The ground truth indices do not match the number of values after evaluation.";
    LOG_IF(FATAL, j < 0 || j >= data.cols) << "Cannot summarize invalid column.";
    
    int max_gt = *std::max_element(gt.begin(), gt.end());
    LOG_IF(FATAL, max_gt + 1 != mean.size()) << "Invalid number of means given.";
    
    std::vector<int> count(max_gt + 1, 0);
    std.resize(max_gt + 1, 0);
    
    for (int i = 0; i < data.rows; ++i) {
        std[gt[i]] += data.at<float>(i, j)*data.at<float>(i, j);
        ++count[gt[i]];
    }
    
    for (int i = 0; i < max_gt + 1; ++i) {
        if (count[i] > 0) {
            std[i] /= count[i];
            std[i] -= mean[i]*mean[i];
            std[i] = std::sqrt(std[i]);
        }
    }
    
    std_min = -1;
    std_max = -1;
    
    if (max_gt > 0) {
        
        int count_0 = 0;
        
        std_min = 0;
        std_max = 0;
        
        float mean_min = 0;
        float mean_max = 0;
        
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        
        for (int i = 0; i < data.rows; ++i) {
            
            // New data point, so reset min/max and save old ones.
            if (gt[i] == 0 && i != 0) {
                std_min += min*min;
                std_max += max*max;
                
                mean_min += min;
                mean_max += max;
                
                min = std::numeric_limits<float>::max();
                max = std::numeric_limits<float>::min();
                
                ++count_0;
            }
            
            if (data.at<float>(i, j) < min) {
                min = data.at<float>(i, j);
            }
            if (data.at<float>(i, j) > max) {
                max = data.at<float>(i, j);
            }
        }
        
        std_min /= count_0;
        std_max /= count_0;
        
        mean_min /= count_0;
        mean_max /= count_0;
        
        std_min -= mean_min*mean_min;
        std_max -= mean_max*mean_max;
        
        std_min = std::sqrt(std_min);
        std_max = std::sqrt(std_max);
    }
    
    return std[0];
}

////////////////////////////////////////////////////////////////////////////////
// computeSummary
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::computeSummary(int &gt_max) {
    
    // Get all superpixel segmentations.
    std::multimap<std::string, boost::filesystem::path> sp_files;
    std::vector<std::string> csv_extensions;
    IOUtil::getCSVExtensions(csv_extensions);
    std::vector<std::string> exclude;
    exclude.push_back("correlation");
    exclude.push_back("results");
    exclude.push_back("summary");
    IOUtil::readDirectory(sp_directory, csv_extensions, sp_files, "", "", exclude);
    
    // Ground truth indices for statistic computation.
    std::vector<int> gt;
//    cv::Mat mat_results(1, countMetrics(), CV_32FC1, cv::Scalar(0));
    cv::Mat mat_results;
    
    std::stringstream csv_results;
    std::vector<std::string> metric_order;
    evaluateHeader(csv_results, metric_order);
    
//    LOG(INFO) << "Computing evaluation metrics.";
    
    int i = 0;
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = sp_files.begin();
            it != sp_files.end(); it++) {
        
        // Exclude created files.
        if (it->second.stem().string() == "results"
                || it->second.stem().string() == "summary") {
            continue;
        }
        
        boost::filesystem::path img_file = img_directory / 
                boost::filesystem::path(it->second.stem().string() + ".png");
        if (!boost::filesystem::is_regular_file(img_file)) {
            img_file = img_directory / 
                boost::filesystem::path(it->second.stem().string() + ".jpg");
        }
        if (!boost::filesystem::is_regular_file(img_file)) {
            img_file = img_directory / 
                boost::filesystem::path(it->second.stem().string() + ".jpeg");
        }
        
        LOG_IF(FATAL, !boost::filesystem::is_regular_file(img_file)) 
                << "Image does not exist (tried .png, .jpg, .jpeg): " 
                << img_file.string() << ".";
        LOG_IF(FATAL, !boost::filesystem::is_regular_file(it->second)) 
                << "Superpixel segmentation does not exist (which is weird): "
                << it->second.string() << ".";
        
        cv::Mat sp_segmentation;
//        IOUtil::readMatCSVInt(it->second, sp_segmentation, image.rows, image.cols);
        IOUtil::readMatCSVInt(it->second, sp_segmentation);
        cv::Mat image = cv::imread(img_file.string(), CV_LOAD_IMAGE_COLOR);
        
//        cv::namedWindow("Image");
//        cv::imshow("Image", image);
//        
//        cv::waitKey(0);
        
        LOG_IF(FATAL, image.rows <= 0 || image.cols <= 0) << "Could not read image: " 
                << img_file.string() << ".";
        LOG_IF(FATAL, image.channels() != 3) << "Currently only 3-channel images are supported: " 
                << image.channels() << " (" << img_file.string() << ").";
        LOG_IF(FATAL, sp_segmentation.rows != image.rows || sp_segmentation.cols != image.cols) 
                << "Superpixel segmentation does not match image size: (" 
                << sp_segmentation.rows << "," << sp_segmentation.cols << ") != (" 
                << image.rows << "," << image.cols << ").";

        // Find at least one ground truth file.
        boost::filesystem::path gt_file = gt_directory / it->second.filename();
        if (boost::filesystem::is_regular_file(gt_file)) {
//            LOG(INFO) << "[" << i << "] One ground truth found for file " 
//                    << i << "/" << sp_files.size() << ".";
            
            // Only one gt_file.
            cv::Mat gt_segmentation;
            IOUtil::readMatCSVInt(gt_file, gt_segmentation);
            
            LOG_IF(FATAL, gt_segmentation.rows != image.rows || gt_segmentation.cols != image.cols) 
                    << "Ground truth does not match image size.";
            
            csv_results << it->second.stem() << ",";
            csv_results << gt_file.stem() << ",";
            
            evaluate(sp_segmentation, gt_segmentation, image, mat_results, csv_results);
            gt.push_back(0);
            
            // Visualizations.
            visualize(sp_segmentation, gt_segmentation, image, it->second.stem().string());
        }
        else {
            for (int t = 0; t < 5; ++t) {
//                LOG(INFO) << "[" << i << "] Processing ground truth " << (t + 1) 
//                        << " found for file " << i << "/" << sp_files.size() << ".";
                
                boost::filesystem::path gt_file_t = gt_directory / 
                        boost::filesystem::path(it->second.stem().string() + "-" + std::to_string(t) + ".csv");
                LOG_IF(ERROR, !boost::filesystem::is_regular_file(gt_file_t)) << "[" << i << "] Ground truth " << (t + 1)
                        << " not found for file " << i << "/" << sp_files.size() << ".";
                
                if (boost::filesystem::is_regular_file(gt_file_t)) {
                    // Found a ground truth file.
                    
                    cv::Mat gt_segmentation;
                    IOUtil::readMatCSVInt(gt_file_t, gt_segmentation);

                    LOG_IF(FATAL, gt_segmentation.rows != image.rows || gt_segmentation.cols != image.cols) 
                            << "Ground truth does not match image size.";

                    csv_results << it->second.stem() << ",";
                    csv_results << gt_file_t.stem() << ",";

                    evaluate(sp_segmentation, gt_segmentation, image, mat_results, csv_results);
                    gt.push_back(t);
                    
                    // Visualizations.
                    visualize(sp_segmentation, gt_segmentation, image, it->second.stem().string(), t);
                }
            }
        }
        
        ++i;
    }
    
    LOG_IF(FATAL, gt.size() == 0) << "No superpixel segmentation files found!";
    gt_max = *std::max_element(gt.begin(), gt.end());
    
    // Save output as CSV file, and save data as cv::Mat.
    std::ofstream csv_results_file(results_file.string());
    csv_results_file << csv_results.str();
    csv_results_file.close();
    
    boost::filesystem::path results_mat_file(results_file.string() + ".txt");
    IOUtil::writeMat(results_mat_file, mat_results);
    
    // Compute correlation if requested.
    if (compute_correlation) {
        cv::Mat mat_correlation;
        std::string csv_correlation;
        correlate(mat_results, metric_order, mat_correlation, 
                csv_correlation);
        
        std::ofstream csv_correlation_file(correlation_file.string());
        csv_correlation_file << csv_correlation;
        csv_correlation_file.close();
        
        boost::filesystem::path correlation_mat_file(correlation_file.string() + ".txt");
        IOUtil::writeMat(correlation_mat_file, mat_correlation);
    }
    
    validateStatistics();
    std::stringstream csv_summary_header;
    summaryHeader(gt, csv_summary_header);
    
    cv::Mat mat_summary;
    
//    LOG(INFO) << "Summarizing results.";
    std::stringstream csv_summary;
    for (int j = 0; j < mat_results.cols; ++j) {
        csv_summary << metric_order[j] << ",";
        
//        LOG(INFO) << "[" << j << "] Summarizing metric " << (j + 1) << "/" << mat_results.cols << ".";
        summarize(gt, mat_results, j, mat_summary, csv_summary);
    }
    
    std::ofstream csv_summary_file(summary_file.string());
    csv_summary_file << csv_summary_header.str() << csv_summary.str();
    csv_summary_file.close();
    
    boost::filesystem::path summary_mat_file(summary_file.string() + ".txt");
    IOUtil::writeMat(summary_mat_file, mat_summary);
    
    if (!append_file.empty()) {
        
        bool exists = boost::filesystem::is_regular_file(append_file);
        std::ofstream csv_append_file(append_file.string(), 
                std::ofstream::out | std::ofstream::app);
        
        if (!exists) {
            // Add header.
            std::ofstream csv_append_file(append_file.string());
            csv_append_file << csv_summary_header.str();
        }
        
        csv_append_file << csv_summary.str();
        csv_append_file.close();
    }
}

////////////////////////////////////////////////////////////////////////////////
// setAppendFile
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::setAppendFile(const boost::filesystem::path &append_file_) {
    append_file = append_file_;
}

////////////////////////////////////////////////////////////////////////////////
// setEvaluationStatistics
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::setEvaluationStatistics(const EvaluationSummary::EvaluationStatistics &evaluation_statistics_) {
    evaluation_statistics = evaluation_statistics_;
}

////////////////////////////////////////////////////////////////////////////////
// setEvaluationMetrics
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::setEvaluationMetrics(const EvaluationSummary::EvaluationMetrics &evaluation_metrics_) {
    evaluation_metrics = evaluation_metrics_;
}

////////////////////////////////////////////////////////////////////////////////
// setSuperpixelVisualizations
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::setSuperpixelVisualizations(const EvaluationSummary::SuperpixelVisualizations &superpixel_visualizations_) {
    superpixel_visualizations = superpixel_visualizations_;
}

////////////////////////////////////////////////////////////////////////////////
// getEvaluationStatistics
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::getEvaluationStatistics(EvaluationSummary::EvaluationStatistics &evaluation_statistics_) {
    evaluation_statistics_ = evaluation_statistics;
}

////////////////////////////////////////////////////////////////////////////////
// getEvaluationMetrics
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::getEvaluationMetrics(EvaluationSummary::EvaluationMetrics &evaluation_metrics_) {
    evaluation_metrics_ = evaluation_metrics;
}

////////////////////////////////////////////////////////////////////////////////
// getSuperpixelVisualizations
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::getSuperpixelVisualizations(EvaluationSummary::SuperpixelVisualizations &superpixel_visualizations_) {
    superpixel_visualizations_ = superpixel_visualizations;
}

////////////////////////////////////////////////////////////////////////////////
// getSuperpixelVisualizations
////////////////////////////////////////////////////////////////////////////////

void EvaluationSummary::setComputeCorrelation(bool compute_correlation_) {
    compute_correlation = compute_correlation_;
}
    
////////////////////////////////////////////////////////////////////////////////
// getSuperpixelVisualizations
////////////////////////////////////////////////////////////////////////////////
    
bool EvaluationSummary::getComputeCorrelation() {
    return compute_correlation;
}