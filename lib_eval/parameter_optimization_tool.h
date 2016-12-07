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

#ifndef PARAMETER_OPTIMIZATION_TOOL_H
#define	PARAMETER_OPTIMIZATION_TOOL_H

#include <tuple>
#include "evaluation_summary.h"

/** \brief Tool to guide parameter optimization using grid search.
 * \author David Stutz
 */
class ParameterOptimizationTool {
public:
    
    /** \brief Constant to indicate a float parameter. */
    static const int FLOAT_PARAMETER = 1;
    /** \brief Constant to indicate an integer parameter. */
    static const int INTEGER_PARAMETER = 2;
    
    /** \brief Constructor.
     * 
     * The command line tool to use has to provide several options, including -i to
     * specify the input directory, -o to specify the CV output directory,
     * -d for depth directory. See README.md for details.
     * 
     * \param[in] img_directory directory containing the images
     * \param[in] gt_directory directories containing the ground truth segmentations
     * \param[in] base_directory base directory to operate in, i.e. save superpixel labels and summaries
     * \param[in] command_line command to use to run the algorithm to optimize prameters for
     * \param[in] command_line_parameters command line parameters to use with each run.
     */
    ParameterOptimizationTool(boost::filesystem::path img_directory, 
            boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
            std::string command_line, std::string command_line_parameters);
    
    /** \brief Add post-processing script (e.g. boundaries to segmentation conversion) 
     * which is applied on the CSV segmentation output of the algorithms.
     * \param[in] command_line command used for post processing, it is called with -i for the superpixel labels directory and -m for the image directory
     */
    void addPostProcessingCommandLine(std::string command_line);
    
    /** \brief Some algorithms do not allow to control superpixels, therefore,
     * optimization may aim for a specific range of superpixel numbers.
     * \param[in] superpixels desired number of superpixels
     * \param[in] tolerance number of superpixels we allow to deviate from the desired number
     */
    void addSuperpixelTolerance(int superpixels, int tolerance);
    
    /** \brief Add float parameter to optimize.
     * \param[in] name name of the parameter
     * \param[in] parameter parameter for the command line
     * \param[in] values vector of values to consider
     */
    void addFloatParameter(std::string name, std::string parameter, 
            std::vector<float> values);
    
    /** \brief Add integer parameter to optimize.
     * \param[in] name name of the parameter
     * \param[in] parameter parameter for the command line
     * \param[in] values vector of values to consider
     */
    void addIntegerParameter(std::string name, std::string parameter, 
            std::vector<int> values);
    
    /** \brief Use depth as additional cue.
     * \param[in] depth_directory directory containing depth files
     */
    void useDepth(boost::filesystem::path depth_directory);
    
    /** \brief Use intrinsic parameters from given directory for depth.
     * \param[in] intrinsics_directory directory containing the intrinsic parameters of each image
     */
    void useIntrinsics(boost::filesystem::path intrinsics_directory);
    
    /** \brief Set parameter optimization verbose. Per default output is written to
     * std::cout.
     * \param[in] stream stream to output to
     */
    void setVerbose(std::ostream &stream = std::cout);
    
    /** \brief Count parameter combinations.
     * \return the number of combinations of all parameter values
     */
    int numCombinations();
    
    /** \brief Jointly optimize for Boundary Recall and Undersegmentation Error.
     * \param[in] weight weight between boundary recall and undersegmentation error
     * \param[in] weight_ue alternative (secondary) optimization weight for undersegmentation error
     * \param[in] weight_co alternative (secondary) optimization weight for compactness
     */
    void optimize(float weigh = 0.5, float weight_ue = 0.33, float weight_co = 0.33);
    
protected:
    
    /** \brief Removes all unnecessary CSV files in base folder.
     * \param[in] so_directory clean up the directory containing the superpixel labels
     */
    void cleanUp(const boost::filesystem::path &sp_directory);
    
    /** \brief The command used to runt he algorithm. */
    std::string command_line;
    /** \brief Command line parameters always to append. */
    std::string command_line_parameters;
    /** \brief Command to use for post-processing. */
    std::string post_processing_command_line;
    
    /** \brief Directory containing the images. */
    boost::filesystem::path img_directory;
    /** \brief Directory containing the ground truth segmentations. */
    boost::filesystem::path gt_directory;
    /** \brief Directory containing the depth images. */
    boost::filesystem::path depth_directory;
    /** \brief Directory containing the intrinsic parameters for each image. */
    boost::filesystem::path intrinsics_directory;
    /** \brief Base directory to optimize parameters in and put temporary files. */
    boost::filesystem::path base_directory;
    
    /** \brief Evaluation metrics to use. */
    EvaluationSummary::EvaluationMetrics evaluation_metrics;
    /** \brief Evaluation statistics to use. */
    EvaluationSummary::EvaluationStatistics evaluation_statistics;
    
    /** \brief The parameters, with name, command line parameter, type and index in the respective vector. */
    std::vector< std::tuple<std::string, std::string, int, int> > parameters;
    /** \brief Vector containing all integer parameters. */
    std::vector< std::tuple<std::vector<int>, int, int, int> > integer_parameters;
    /** \brief Vector containing all float parameters. */
    std::vector< std::tuple<std::vector<float>, int, float, float> > float_parameters;
    
    /** \brief Minimum number of superpixels. */
    int superpixels_min;
    /** brief Maximum number of superpixels. */
    int superpixels_max;
    
};

#endif	/* PARAMETER_OPTIMIZATION_TOOL_H */

