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

/** \brief Example of parameter optimization for an adapted W as used in the
 * example for submitting a new algorithm to the benchmark in docs/EXAMPLE.md.
 * $ ../bin/example_parameter_optimization --help
 * \code{sh}
 * Allowed options:
 *   --img-directory arg   image directory
 *   --gt-directory arg    ground truth directory
 *   --base-directory arg  base directory
 *   --help                produce help message
 * \endcode
 * \author David Stutz
 */
int main(int argc, const char** argv) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("img-directory", boost::program_options::value<std::string>(), "image directory")
        ("gt-directory", boost::program_options::value<std::string>(), "ground truth directory")
        ("base-directory", boost::program_options::value<std::string>(), "base directory")
        ("help", "produce help message");

    boost::program_options::positional_options_description positionals;
    positionals.add("img-directory", 1);
    positionals.add("gt-directory", 1);
    positionals.add("base-directory", 1);

    boost::program_options::variables_map parameters;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
    boost::program_options::notify(parameters);

    if (parameters.find("help") != parameters.end()) {
        std::cout << desc << std::endl;
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
    
    boost::filesystem::path base_directory(parameters["base-directory"].as<std::string>());
    if (!boost::filesystem::is_directory(base_directory)) {
        boost::filesystem::create_directories(base_directory);
    }

    std::vector<int> superpixels = {400, 1200, 3600};

    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                "./bin/example_command_line_tool", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("sigma", "--sigma", std::vector<float>{0.5f, 1.f, 1.5f});
        tool.optimize();
    }

    return 0;
}