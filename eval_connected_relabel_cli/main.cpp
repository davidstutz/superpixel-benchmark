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
#include <boost/program_options.hpp>
#include <glog/logging.h>
#include "superpixel_tools.h"
#include "io_util.h"

/** \brief Relabel superpixels in order to be connected.
 * Usage:
 * \code{sh}
 *   $ ../bin/eval_connected_relabel_cli --help
 *   Allowed options:
 *     --help                     produce help message
 *     -i [ --input-csv ] arg     folder containing the corresponding images to 
 *                                process
 *     -m [ --input-images ] arg  dummy option!
 *     -r [ --overwrite ]         Overwrite original files
 *     -o [ --csv ] arg (=output) save segmentation as CSV file
 *     -w [ --wordy ]             wordy/verbose
 * \endcode
 * \author David Stutz
 */
int main (int argc, char ** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("input-csv,i", boost::program_options::value<std::string>(), "folder containing the corresponding images to process")
        ("input-images,m", boost::program_options::value<std::string>()->default_value(""), "dummy option!")
        ("overwrite,r", "Overwrite original files")
        ("csv,o", boost::program_options::value<std::string>()->default_value("output"), "save segmentation as CSV file")
        ("wordy,w", "wordy/verbose");
    
    boost::program_options::positional_options_description positionals;
    positionals.add("input-csv", 1);
    
    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).allow_unregistered().run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
        return 1;
    }
    
    boost::filesystem::path output_dir(parameters["csv"].as<std::string>());
    if (!output_dir.empty() && parameters.find("overwrite") == parameters.end()) {
        if (!boost::filesystem::is_directory(output_dir)) {
            boost::filesystem::create_directory(output_dir);
        }
    }
    
    boost::filesystem::path labels_dir(parameters["input-csv"].as<std::string>());
    if (!boost::filesystem::is_directory(labels_dir)) {
        std::cout << "Label directory not found ..." << std::endl;
        return 1;
    }
    
    if (output_dir.empty() && parameters.find("overwrite") == parameters.end()) {
        std::cout << "No output directory specified and not allowed to overwrite!" << std::endl;
        return 1;
    }
    
    bool wordy = false;
    if (parameters.find("wordy") != parameters.end()) {
        wordy = true;
    }
    
    std::multimap<std::string, boost::filesystem::path> labels;
    std::vector<std::string> extensions;
    IOUtil::getCSVExtensions(extensions);
    IOUtil::readDirectory(labels_dir, extensions, labels);
    
    for(std::multimap<std::string, boost::filesystem::path>::iterator it = labels.begin(); 
            it != labels.end(); ++it) {
        
        cv::Mat labels;
        IOUtil::readMatCSVInt(it->second, labels);
        
        int superpixels = SuperpixelTools::countSuperpixels(labels);
        int components = SuperpixelTools::relabelConnectedSuperpixels(labels);
        
        if (wordy) {
            std::cout << superpixels << " superpixels / " 
                    << components << " new components for " << it->first << "." << std::endl; 
        }
        
        if (components > 0) {
            if (parameters.find("overwrite") != parameters.end()) {
                boost::filesystem::path label_file(labels_dir 
                        / boost::filesystem::path(it->second.stem().string() + ".csv"));
                IOUtil::writeMatCSV<int>(label_file, labels);
            }
            else {
                boost::filesystem::path label_file(output_dir 
                        / boost::filesystem::path(it->second.stem().string() + ".csv"));
                IOUtil::writeMatCSV<int>(label_file, labels);
            }
        }
    }
    
    return 0;
}