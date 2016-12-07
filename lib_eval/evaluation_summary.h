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

#ifndef EVALUATION_SUMMARY_H
#define	EVALUATION_SUMMARY_H

#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

/** \brief Given a directory of superpixel segmentations and a directory of
 * ground truth segmentations, this class is used to generate a CSV file of 
 * statistics of different metrics.
 * \author David Stutz
 */
class EvaluationSummary {
public:
    
    /** \brief Available superpixel visualizations, see visualization.h.
     */
    struct SuperpixelVisualizations {
        SuperpixelVisualizations() : contour(false), random(false), perturbed_mean(false),
                mean(false), pre_rec(false), ue(false) {};
        
        /** \brief Whether to visualize contours. */
        bool contour;
        /** \brief Whether to visualize superpixels as randomly colored. */
        bool random;
        /** \brief Whether to visualize superpixels using a slightly perturbed mean color. */
        bool perturbed_mean;
        /** \brief Whether to visualize superpixels using mean color. */
        bool mean;
        /** \brief Whether to visualize false positives and negatives in the Recall/Precision sense.*/
        bool pre_rec;
        /** \brief Whether to color the undersegmentation error. */
        bool ue;
        
        /** \brief Decide if any visualization is requested.
         * \return true if some of the above is true
         */
        bool any() {
            return contour || random || perturbed_mean || mean || pre_rec || ue;
        }
    };
    
    /** \brief Available metrics, see evaluation.h.
     */
    struct EvaluationMetrics {
        EvaluationMetrics() : ue(true), oe(true), rec(true), pre(true),
                ue_np(true), ue_levin(true), asa(true), sse_rgb(true), sse_xy(true),
                co(true), ev(true), mde(true), icv(true), cd(true),
                reg(true), sp(true), sp_size(true) {};
        
        /** \brief Whether to use Undersegmentation Error. */
        bool ue;
        /** \brief Whether to use Oversegmentation Error. */
        bool oe;
        /** \brief WHether to use Boundary Recal. */
        bool rec;
        /** \brief Whether to use Boundary Precision. */
        bool pre;
        /** \brief Whether to use Neubert and Protzel's definition of the Undersegmentation Error. */
        bool ue_np;
        /** \brief Whether to use Levinshtein's definition of the Undersegmentation Error. */
        bool ue_levin;
        /** \brief Whether to use Achievable Segmentation Accuracy. */
        bool asa;
        /** \brief Whether to use the sum-of-squared error on RGB color. */
        bool sse_rgb;
        /** \brief Whether to use the sum-of-squared error on xy coordinates. */
        bool sse_xy;
        /** \brief Whether to use Compactness. */
        bool co;
        /** \brief Whether to use Explained Variation. */
        bool ev;
        /** \brief Whether to use Mean Distance to Edge. */
        bool mde;
        /** \brief Whether to use Intra-Cluster Variation. */
        bool icv;
        /** \brief Whether to use Contour Density. */
        bool cd;
        /** \brief Whether to use Regularity. */
        bool reg;
        /** \brief Whether to compute the number of superpixels. */
        bool sp;
        /** \brief Whether to compute superpixel size. */
        bool sp_size;
    };
    
    /** \brief Available statistics that may be computed for each metric.
     */
    struct EvaluationStatistics {
        
        EvaluationStatistics() : mean(true), median_and_quartiles(true), 
                min_and_max(true), mid_range(true), std(true) {};
        
        /** \brief Compute the mean over all images.*/
        bool mean;
        /** \brief Compute median, first and third quantile over all images. */
        bool median_and_quartiles;
        /** \brief Compute minimum and maximum over all images. */
        bool min_and_max;
        /** \brief Whether to compute the mid range. */
        bool mid_range;
        /** \brief Whether to compute the standard deviation. */
        bool std;
    };
    
    /** \brief Create an evaluation summary for the given directory.
     * 
     * The directory is assumed to contain several superpixel segmentations as
     * CSV files. The files are assumed to be named according to the
     * ground truth files, see below.
     * 
     * In addition, a directory containing ground truth segmentations as
     * CSV files needs to be provided.
     * 
     * \param[in] sp_directory directory containing superpixel segmentations as CSV files
     * \param[in] gt_directory directory containing ground truth segmentations as CSV files
     * \param[in] img_directory directory containing the corresponding images as JPEG or PNG
     */
    EvaluationSummary(boost::filesystem::path sp_directory, 
            boost::filesystem::path gt_directory, boost::filesystem::path img_directory);
    
    /** \brief Constructor.
     * 
     * \param[in] sp_directory directory containing superpixel segmentations as CSV files
     * \param[in] gt_directory directory containing ground truth segmentations as CSV files
     * \param[in] img_directory directory containing the corresponding images as JPEG or PNG
     * \param[in] evaluation_metrics evaluation metrics to use
     * \param[in] evaluation_statistics evaluation statistics to compute
     */
    EvaluationSummary(boost::filesystem::path sp_directory, 
            boost::filesystem::path gt_directory, boost::filesystem::path img_directory,
            EvaluationMetrics evaluation_metrics, EvaluationStatistics evaluation_statistics);
    
    /** \brief Constructor. 
     * 
     * \param[in] sp_directory directory containing superpixel segmentations as CSV files
     * \param[in] gt_directory directory containing ground truth segmentations as CSV files
     * \param[in] img_directory directory containing the corresponding images as JPEG or PNG
     * \param[in] evaluation_metrics evaluation metrics to use
     * \param[in] evaluation_statistics evaluation statistics to compute
     * \param[in] superpixel_visualizations visualizations to create
     */
    EvaluationSummary(boost::filesystem::path sp_directory, 
            boost::filesystem::path gt_directory, boost::filesystem::path img_directory,
            EvaluationMetrics evaluation_metrics, EvaluationStatistics evaluation_statistics,
            SuperpixelVisualizations superpixel_visualizations);
    
    /** \brief Summarize the results; this is the main entrance point.
     * \param[out] gt_max the maxmimum number of ground truth used, for BSDS 5 for all other 1
     */
    void computeSummary(int &gt_max);
    
    /** \brief Add CSV file to append CSV output to.
     * \param[in] append_file path to CSV file to append to
     */
    void setAppendFile(const boost::filesystem::path &append_file);
    
    /** \brief Set the evaluation statistics to compute.
     * \param[in] evaluation_statistics evaluation statistics to compute
     */
    void setEvaluationStatistics(const EvaluationStatistics &evaluation_statistics);
    
    /** \brief Set the evaluation metrics to compute.
     * \param[in] evaluation_metrics evaluation metrics to compute
     */
    void setEvaluationMetrics(const EvaluationMetrics &evaluation_metrics);
    
    /** \brief Set the superpixel visualizations to compute.
     * \param[in] superpixel_visualizations visualizations to compute
     */
    void setSuperpixelVisualizations(const SuperpixelVisualizations &superpixel_visualizations);
    
    /** \brief Set whether to compute correlation.
     * \param[in] compute_correlation whether to compute correlation matrix
     */
    void setComputeCorrelation(bool compute_correlation);
    
    /** \brief Get the evaluation statistics used.
     * \param[out] evaluation_statistics evaluation statistics used
     */
    void getEvaluationStatistics(EvaluationSummary::EvaluationStatistics &evaluation_statistics);

    /** \brief Get the evaluation metrics used.
     * \param[out] evaluation_metrics evaluation metrics used
     */
    void getEvaluationMetrics(EvaluationSummary::EvaluationMetrics &evaluation_metrics);

    /** \brief Get the visualizations used.
     * \param[out] superpixel_visualizations visualizations used
     */
    void getSuperpixelVisualizations(EvaluationSummary::SuperpixelVisualizations &superpixel_visualizations);

    /** \brief Get whether correlation is computed.
     * \return whether to compute correlation
     */
    bool getComputeCorrelation();
    
protected:
    
    /** \brief Count number of metrics used.
     * \return number of metrics to compute
     */
    int countMetrics();
    
    /** \brief Add header to output.
     * \param[in] output the header of the CSV file to create
     * \param[out] metric_order the order of the metrics
     */
    void evaluateHeader(std::stringstream &output, std::vector<std::string> &metric_order);
    
    /** \brief Actually do the evaluation.
     * \param[in] sp_segmentation superpixel labels as int image
     * \param[in] gt_segmentation ground truth segmentation as int image
     * \param[in] image corresponding image
     * \param[in] data data matrix to append results to
     * \param[in] output CSV file stream to append results to
     */
    void evaluate(const cv::Mat &sp_segmentation, const cv::Mat &gt_segmentation, 
            const cv::Mat &image, cv::Mat &data, std::stringstream &output);
    
    /** \brief Visualize given segmentation.
     * \param[in] sp_segmentation superpixel labels as int image
     * \param[in] gt_segmentation ground truth segmentation as int image
     * \param[in] image corresponding image
     * \param[in] name name or identifier of image to visualize
     * \param[in] t ground truth number
     */
    void visualize(const cv::Mat &sp_segmentation, const cv::Mat &gt_segmentation,
            const cv::Mat &image, std::string name, int t = 0);
    
    /** \brief Compute correlation between all metrics.
     * \param[in] mat_results matrix containing results in order to compute correlatiom
     * \param[in] metric_order order of metrix used in mat_results
     * \param[out] mat_correlation correlation matrix
     * \param[out] csv_correlation correlation matrix as CSV string
     */
    void correlate(const cv::Mat &mat_results, const std::vector<std::string> &metric_order,
            cv::Mat &mat_correlation, std::string &csv_correlation);
    
    /** \brief Count the number of statistics to be used.
     * \param[in] gt_max maximum number of ground truth segmentations to consider
     * \return number of statistics
     */
    int countStatistics(int gt_max);
    
    /**\brief Validate and correct statistics.
     */
    void validateStatistics();
    
    /** \brief Add the summary header to output.
     * \param[in] gt ground truth indices to determine the number of ground truth segmentations
     */
    void summaryHeader(const std::vector<int> &gt, std::stringstream &output);
    
    /** \brief Summarize a specific column of the evaluation data (corresponding to a
     * particular metric).
     * \param[in] gt ground truth indices to determine the number of ground truth segmentations
     * \param[in] data data to summarize
     * \param[in] j the column
     * \param[out] mat_sumamry summary as matrix
     * \param[out] output summary as CSV string
     */
    void summarize(const std::vector<int> &gt, const cv::Mat &data, 
            int j, cv::Mat &mat_summary, std::stringstream &output);
    
    /** \brief Compute mean statistic.
     * \param[in] gt ground truth indices to determine the number of ground truth segmentations
     * \param[in] data data to summarize
     * \param[in] j the column
     * \param[out] mean computed means as vector for each ground truth segmentation
     * \param[out] mean_min minimum of means in case of multiple ground truth segmentations per image
     * \param[out] mean_max maximum of means in case of multiple ground truth segmentations per image
     */
    float computeMean(const std::vector<int> gt, const cv::Mat &data, int j, 
            std::vector<float> &mean, float &mean_min, float &mean_max);
    
    /** \brief Compute median statistic.
     * \param[in] gt ground truth indices to determine the number of ground truth segmentations
     * \param[in] data data to summarize
     * \param[in] j the column
     * \param[out] median computed medians as vector for each ground truth segmentation
     * \param[out] median_min minimum of medians in case of multiple ground truth segmentations per image
     * \param[out] median_max maximum of medians in case of multiple ground truth segmentations per image
     */
    float comuteMedian(const std::vector<int> gt, const cv::Mat &data, int j, 
            std::vector<float> &median, float &median_min, float &median_max);
    
    /** \brief Compute median, first quartile and third quartile
     * \param[in] gt ground truth indices to determine the number of ground truth segmentations
     * \param[in] data data to summarize
     * \param[in] j the column
     * \param[out] median computed medians as vector for each ground truth segmentation
     * \param[out] first_q computed first quantiles as vector for each ground truth segmentation
     * \param[out] thrid_q computed third quantiles as vector for each ground truth segmentation
     * \param[out] median_min minimum of medians in case of multiple ground truth segmentations per image
     * \param[out] first_q_min minimum of first quantiles in case of multiple ground truth segmentations per image
     * \param[out] third_q_min minimum of quantiles in case of multiple ground truth segmentations per image
     * \param[out] median_max maximum of medians in case of multiple ground truth segmentations per image
     * \param[out] first_q_max maximum of first quantiles in case of multiple ground truth segmentations per image
     * \param[out] second_q_max maximum of quantiles in case of multiple ground truth segmentations per image
     */
    void comuteMedianFirstAndThirdQuartile(const std::vector<int> gt, const cv::Mat &data, int j, 
            std::vector<float> &median, std::vector<float> &first_q, std::vector<float> &thrid_q, 
            float &median_min, float &first_q_min, float &third_q_min, float &median_max,
            float &first_q_max, float &second_q_max);
    
    /** \brief Compute min/max over all ground truth segmentations.
     * \param[in] gt ground truth indices to determine the number of ground truth segmentations
     * \param[in] data data to summarize
     * \param[in] j the column
     * \param[out] min minimum as vector for each ground truth segmentation
     * \param[out] max maximum as vector for each ground truth segmentation
     * \param[out] min_min minimum of minimum over all ground truth segmentations
     * \param[out] max_min maximum of minimum over all ground truth segmentations
     * \param[out] min_max minimum of maximum over all ground truth segmentations
     * \param[out] max_max maximum of maximum over all ground truth segmentations
     */
    void comuteMinMax(const std::vector<int> gt, const cv::Mat &data, int j,
            std::vector<float> &min, std::vector<float> &max, float &min_min,
            float &max_min, float &min_max, float&max_max);
    
    /** \brief Compute standard deviation over all ground truth segmentations.
     * \param[in] gt ground truth indices to determine the number of ground truth segmentations
     * \param[in] data data to summarize
     * \param[in] j the column
     * \param[out] mean means as vector for each ground truth segmentation
     * \param[out] std std as vector for each ground truth segmentation
     * \param[out] std_min minimum of std over all ground truth segmentations
     * \param[out] std_max maximum of std over all ground truth segmentations
     */
    float computeStandardDeviation(const std::vector<int> gt, const cv::Mat &data, int j,
        const std::vector<float> &mean, std::vector<float> &std, float &std_min,
        float &std_max);
    
    /** \brief Evaluation metrics to use. */
    EvaluationMetrics evaluation_metrics;
    /** \brief Evaluation statistics to use. */
    EvaluationStatistics evaluation_statistics;
    /** \brief Visualizations to compute. */
    SuperpixelVisualizations superpixel_visualizations;
    
    /** \brief Whether to compute correlation. */
    bool compute_correlation;
    
    /** \brief Directory of superpixel segmentations. */
    boost::filesystem::path sp_directory;
    /** \brief Directory of ground truth segmentations. */
    boost::filesystem::path gt_directory;
    /** \brief Directory of images. */
    boost::filesystem::path img_directory;
    /** \brief Path to results file. */
    boost::filesystem::path results_file;
    /** \brief Path to correlation file. */
    boost::filesystem::path correlation_file;
    /** \brief Path to summary file. */
    boost::filesystem::path summary_file;
    /** \brief Path to visualization directory. */
    boost::filesystem::path vis_directory;
    /** \brief Path to file to append summary to. */
    boost::filesystem::path append_file;
};

#endif	/* EVALUATION_SUMMARY_H */

