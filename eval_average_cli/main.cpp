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
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>
#include <glog/logging.h>

#include "io_util.h"
#include "evaluation.h"

/** \brief Compute average metrics given a CSV file containing several evaluation summaries.
 * Use evaluation_summary_cli with the --append-file option to gather multiple
 * summaries in one file.
 * 
 * Usage:
 * \code[sh]
 * $ ../bin/eval_average_cli --help
 * Allowed options:
 *   --summary-file arg                    CSV summary file
 *   -o [ --output-file ] arg (=average.csv)
 *                                         output file
 *   --help                                produce help message
 * \endcode
 * \author David Stutz
 */
int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("summary-file", boost::program_options::value<std::string>(), "CSV summary file")
        ("output-file,o", boost::program_options::value<std::string>()->default_value("average.csv"), "output file")
        ("help", "produce help message");

    boost::program_options::positional_options_description positionals;
    positionals.add("summary-file", 1);
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path summary_file(parameters["summary-file"].as<std::string>());
    if (!boost::filesystem::is_regular_file(summary_file)) {
        std::cout << "CSV summary file not found." << std::endl;
        return 1;
    }
    
    std::vector<std::string> row_headers;
    std::vector<std::string> col_headers;
    cv::Mat data;
    
    IOUtil::readCSVSummary(summary_file, row_headers, col_headers, data);
    
    std::string rec_key = "rec";
    std::string rec_statistic = "mean_min";
    std::vector<float> rec;
    
    std::string ue_key = "ue_np";
    std::string ue_statistic = "mean_max";
    std::vector<float> ue;
    
    std::string ev_key = "ev";
    std::string ev_statistic = "mean_min";
    std::vector<float> ev;
    
    std::vector<float> superpixels;
    for (int i = 0; i < data.rows; i++) {
        for (int j = 0; j < data.cols; j++) {
            if (col_headers[j + 1] == rec_statistic && row_headers[i + 1] == rec_key) {
                rec.push_back(data.at<float>(i, j));
            }
            if (col_headers[j + 1] == ue_statistic && row_headers[i + 1] == ue_key) {
                ue.push_back(1 - data.at<float>(i, j));
            }
            if (col_headers[j + 1] == ev_statistic && row_headers[i + 1] == ev_key) {
                ev.push_back(data.at<float>(i, j));
            }
            if (col_headers[j + 1] == "mean_min" && row_headers[i + 1] == "sp") {
                superpixels.push_back(data.at<float>(i, j));
            }
        }
    }
    
    LOG_IF(FATAL, superpixels.size() != rec.size() || superpixels.size() != ue.size() || superpixels.size() != ev.size())
            << "Read invalid number of values.";
    
    std::string output_file = parameters["output-file"].as<std::string>();
    std::ofstream file_stream(output_file.c_str());
    std::setprecision(5);
    
    file_stream << "K" << "," << "Rec" << "," << "1 - UE" << "," << "EV" << "\n";
    std::cout << std::setw(10) << "K" << " " << std::setw(10) << "Rec" << " " 
            << std::setw(10) << "1 - UE" << " " << std::setw(10) << "EV" << std::endl;
    
    for (unsigned int i = 0; i < rec.size(); i++) {
        std::cout << std::setw(10) << superpixels[i] << " " 
                << std::setw(10) << rec[i] << " " << std::setw(10) 
                << ue[i] << " " << std::setw(10) << ev[i] << std::endl;
        file_stream << superpixels[i] << "," << rec[i] 
                << "," << ue[i] << "," << ev[i] << "\n";
    }
        
    float avgRec = Evaluation::computeAverageMetric(rec, superpixels);
    float avgUe = Evaluation::computeAverageMetric(ue, superpixels);
    float avgEv = Evaluation::computeAverageMetric(ev, superpixels);
    
    std::cout << "---------- ---------- ---------- ----------" << std::endl;
    std::cout << "           " << std::setw(10) << avgRec << " " 
            << std::setw(10) << avgUe << " " << std::setw(10) << avgEv << std::endl;
    file_stream << "," << avgRec << "," << avgUe << "," << avgEv << "\n";
    file_stream.close();
    
    return 0;
}