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

// Dirty but simple ...
std::string FAIR = "-f ";
std::string RELATIVE_PATH = ".";
std::string MATLAB_EXECUTABLE = "/home/david/MATLAB/R2014b/bin/matlab";
std::string JAVA_EXECUTABLE = "/home/david/jdk-1.8.0_45/release/java";

////////////////////////////////////////////////////////////////////////////////
// CCS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of CCS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_CCS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size (); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory, 
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/ccs_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("compactness", "--compactness", std::vector<int>{25, 50, 100, 250, 500}); // 5
        tool.addFloatParameter("iterations", "--iterations", std::vector<float>{1, 5, 25, 50}); // 4
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// CIS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of CIS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_CIS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {

    for (unsigned int k = 0; k < superpixels.size (); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory, 
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/cis_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("lambda", "--lambda", std::vector<int>{1, 3, 5, 7, 10}); // 5
        tool.addFloatParameter("sigma", "--sigma", std::vector<float>{0.1f, 1.0f, 5.0f, 10.0f, 25.0f}); // 5
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 3}); // 2
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// CRS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of CRS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_CRS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {

    for (unsigned int k = 0; k < superpixels.size (); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/crs_cli", FAIR);

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("compactness", "--compactness", std::vector<float>{0.001f, 0.005f, 0.01f, 0.05f, 0.1f}); // 5
        tool.addFloatParameter("clique-cost", "--clique-cost", std::vector<float>{0.01f, 0.05f, 0.1f, 0.25f, 0.5f, 1.0f}); // 6
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 3}); // 2
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// CW
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of CW.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_CW(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/cw_cli", FAIR);

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("compactness", "--compactness", std::vector<float>{0.01f, 0.05f, 0.1f, 0.5f, 1.0f, 5.0f, 10.0f}); // 7

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// DASP
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of DASP.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] depth_directory
 * \param[in] intrinsics_directory
 * \param[in] superpixels
 */
void connector_DASP(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        boost::filesystem::path depth_directory, boost::filesystem::path intrinsics_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/dasp_cli", "");
        tool.useDepth(depth_directory);

        if (!intrinsics_directory.empty()) {
            tool.useIntrinsics(intrinsics_directory);
        }

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("seed-mode", "--seed-mode", std::vector<int>{0, 1}); // 2
        tool.addFloatParameter("spatial-weight", "--spatial-weight", std::vector<float>{0.001f, 0.005f, 0.01f, 0.05f, 0.1f, 0.5f}); // 6
        tool.addFloatParameter("normal-weight", "--normal-weight", std::vector<float>{0.0f, 0.25f, 0.5f}); // 3
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{5, 10, 25}); // 3

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// EAMS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of EAMS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 * \param[in] superpixel_tolerances
 */
void connector_EAMS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels, std::vector<int> superpixel_tolerances) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/eams_cli/eams_dispatcher.sh", 
                "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/eams_cli > /dev/null");
        tool.addSuperpixelTolerance(superpixels[k], superpixel_tolerances[k]);

        tool.addIntegerParameter("bandwidth", "-b", std::vector<int>{1, 3, 5, 9, 13}); // 5
        tool.addIntegerParameter("minimum-size", "-m", std::vector<int>{10, 25, 50, 250}); // 4
        tool.addIntegerParameter("color-space", "-r", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// ERGC
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of ERGC.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_ERGC(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/ergc_cli", FAIR);

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("perturb-seeds", "--perturb-seeds", std::vector<int>{0, 1}); // 2
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1}); // 2
        tool.addIntegerParameter("compacity", "--compacity", std::vector<int>{0, 1, 2, 5, 10, 25}); // 6

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// ERS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of ERS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_ERS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/ers_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("lambda", "--lambda", std::vector<float>{0.1f, 0.5f, 1.0f, 2.5f, 5.0f, 10.0f}); // 6
        tool.addFloatParameter("sigma", "--sigma", std::vector<float>{0.1f, 0.5f, 1.0f, 2.5f, 5.0f, 10.0f}); // 6

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// ETPS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of ETPS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_ETPS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/etps_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{400});
        tool.addFloatParameter("regularization-weight", "--regularization-weight", std::vector<float>{0.01f, 0.05f, 0.1f, 0.5f, 1.f, 5.f, 10.f}); // 7
        tool.addFloatParameter("length-weight", "--length-weight", std::vector<float>{0.1f, 1.f, 10.f}); // 3
        tool.addFloatParameter("size-weight", "--size-weight", std::vector<float>{1.f}); // 1
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 5, 10, 25}); // 4

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// FH
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of FH.
 * 
 * @param img_directory
 * @param gt_directory
 * @param base_directory
 */
void connector_FH(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels, std::vector<int> superpixel_tolerances) {
    
    ParameterOptimizationTool tool(img_directory, gt_directory, base_directory,
            RELATIVE_PATH + "/bin/fh_cli", "");

    tool.addFloatParameter("sigma", "--sigma", std::vector<float>{0.0f, 1.0f, 2.0f}); // 3
    tool.addIntegerParameter("minimum-size", "--minimum-size", std::vector<int>{10, 15, 30, 60, 90, 120, 180}); // 7
    tool.addFloatParameter("threshold", "--threshold", std::vector<float>{5, 10, 15, 30, 60, 90}); // 6

    tool.optimize();
}

////////////////////////////////////////////////////////////////////////////////
// MSS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of MSS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_MSS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory, 
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/mss_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("structure-size", "--structure-size", std::vector<int>{3,7,11,15}); // 4
        tool.addFloatParameter("noise", "--noise", std::vector<float>{1.0f, 2.5f, 5.0f, 10.0f, 25.0f, 50.0f}); // 6
        tool.addFloatParameter("tolerance", "--tolerance", std::vector<float>{1.0f, 5.0f, 10.0f, 25.0f}); // 4
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1});

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// PB
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of PB.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_PB(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/pb_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("sigma", "--sigma", std::vector<float>{1.0f, 2.5f, 5.0f, 7.5f, 10.0f, 20.0f}); // 6
        tool.addIntegerParameter("max-flow", "--max-flow", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// POISE
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of POISE.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_POISE(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/poise_cli/poise_dispatcher.sh", 
                "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/poise_cli/ > /dev/null");

        tool.addIntegerParameter("superpixels", "-s", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("sigma", "-g", std::vector<float>{0.1f, 0.5f, 2.5f, 5.f}); // 4
        tool.addFloatParameter("edge-modifier", "-m", std::vector<float>{0.4f, 0.8f, 2.f}); // 3
        tool.addFloatParameter("color-modifier", "-c", std::vector<float>{0.3f, 0.6f, 2.f}); // 3
        tool.addFloatParameter("threshold", "-t", std::vector<float>{0.01f, 0.03f, 0.1f}); // 3

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// preSLIC
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of preSLIC.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_preSLIC(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
        
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/preslic_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("compactness", "--compactness", std::vector<float>{1.0f, 5.0f, 10.0f, 20.0f, 40.0f, 80.0f, 160.0f}); // 9
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 5, 10, 25, 50}); // 5 
        tool.addIntegerParameter("perturb-seeds", "--perturb-seeds", std::vector<int>{0, 1}); // 2
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}



////////////////////////////////////////////////////////////////////////////////
// reSEEDS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of reSEEDS.
 * 
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_reSEEDS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
            RELATIVE_PATH + "/bin/reseeds_cli", FAIR);

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("bins", "--bins", std::vector<int>{1, 3, 5, 7}); // 4
        tool.addIntegerParameter("neighborhood", "--neighborhood", std::vector<int>{0, 1}); // 2
        tool.addFloatParameter("confidence", "--confidence", std::vector<float>{0.1f}); // 1
        tool.addFloatParameter("spatial-weight", "--spatial-weight", std::vector<float>{0.0f, 0.25f, 0.5f}); // 3
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 10, 25}); // 3
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1, 2}); // 3

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// SEAW
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of SEAW.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_SEAW(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                    base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/seaw_cli/seaw_dispatcher.sh", 
                "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/seaw_cli/ > /dev/null");

        if (superpixels[k] < 800) {
            tool.addIntegerParameter("level", "-l", std::vector<int>{5}); // 2
        }
        else if (superpixels[k] >= 800 && superpixels[k] < 2400) {
            tool.addIntegerParameter("level", "-l", std::vector<int>{4}); // 2
        }
        else {
            tool.addIntegerParameter("level", "-l", std::vector<int>{3}); // 2
        }

        tool.addFloatParameter("sigma", "-g", std::vector<float>{1.0f, 5.0f, 10.0f}); // 3
        tool.addIntegerParameter("dist-func", "-c", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// SEEDS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of SEEDS.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_SEEDS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/seeds_cli", FAIR);

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("bins", "--bins", std::vector<int>{1, 3, 5, 7, 9}); // 5
        tool.addIntegerParameter("prior", "--prior", std::vector<int>{0, 1}); // 2
        tool.addFloatParameter("confidence", "--confidence", std::vector<float>{0.1f}); // 2
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 2, 10, 25}); // 6
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1, 2}); // 3
        tool.addIntegerParameter("means", "--means", std::vector<int>{1}); // 1

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// SLIC
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of SLIC.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_SLIC(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/slic_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("compactness", "--compactness", std::vector<float>{1.0f, 5.0f, 10.0f, 20.0f, 40.0f, 80.0f, 160.0f}); // 9
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 5, 10, 25, 50}); // 5 
        tool.addIntegerParameter("perturb-seeds", "--perturb-seeds", std::vector<int>{0, 1}); // 2
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// TP
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of TP.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_TP(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/tp_cli/tp_dispatcher.sh", 
                "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/tp_cli/ > /dev/null");
        tool.addPostProcessingCommandLine(RELATIVE_PATH + "/bin/boundaries_to_labels_cli --overwrite");

        tool.addIntegerParameter("superpixels", "-s", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("time-step", "-m", std::vector<float>{0.5f, 1.0f}); // 2
        tool.addFloatParameter("sigma", "-g", std::vector<float>{1.0f, 2.5f, 5.0f, 10.0f, 15.0f}); // 5
        tool.addIntegerParameter("max-iterations", "-t", std::vector<int>{50, 100, 250, 500}); // 4

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// TPS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of TPS.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_TPS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/tps_cli/tps_dispatcher.sh", 
                FAIR + "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/tps_cli/ > /dev/null");

        tool.addIntegerParameter("superpixels", "-s", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("type", "-t", std::vector<int>{0, 1, 2}); // 3
        tool.addFloatParameter("gamma", "-m", std::vector<float>{0.25f, 0.5f, 0.75f}); // 3
        tool.addFloatParameter("sigma", "-g", std::vector<float>{0.25f, 0.5f, 0.75f}); // 3

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// vlSLIC
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of vlSLIC.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_vlSLIC(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/vlslic_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("minimum-region-size", "--minimum-region-size", std::vector<int>{20});
        tool.addFloatParameter("compactness", "--compactness", std::vector<float>{1.0f, 5.0f, 10.0f, 20.0f, 40.0f, 80.0f, 160.0f}); // 9
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 5, 10, 25, 50}); // 5

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// W
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of W.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_W(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/w_cli", "");

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// WP
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of WP.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_WP(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/wp_cli/wp_dispatcher.sh", "> /dev/null");
        tool.addPostProcessingCommandLine(RELATIVE_PATH + "/bin/connected_relabel_cli --overwrite");

        tool.addIntegerParameter("superpixels", "-s", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("weight", "-w", std::vector<float>{0.1f, 1.0f, 2.5f, 5.0f, 10.0f, 25.0f, 50.0f}); // 7

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// PF
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of PF.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_PF(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/pf_cli/pf_dispatcher.sh", 
                "-e " + JAVA_EXECUTABLE + " -j " + RELATIVE_PATH + "/lib_pf/PathFinder.jar");
        tool.addPostProcessingCommandLine(RELATIVE_PATH + "/bin/connected_relabel_cli --overwrite");

        tool.addIntegerParameter("superpixels", "-s", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("radius", "-r", std::vector<int>{1, 3, 5, 7, 9, 11, 31, 51}); // 8

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// LSC
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of LSC.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_LSC(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/lsc_cli", FAIR);      

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("ratio", "--ratio", std::vector<float>{0.0f, 0.01f, 0.05f, 0.1f, 0.25f, 0.5f}); // 6
        tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 5, 10 ,25, 50}); // 5
        tool.addIntegerParameter("threshold", "--threshold", std::vector<int>{10}); // 1
        tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// RW
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of RW.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_RW(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < 1; k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/rw_cli/rw_dispatcher.sh", 
                FAIR + "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/rw_cli > /dev/null");

        tool.addIntegerParameter("superpixels", "-s", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("beta", "-b", std::vector<float>{1, 5, 25, 50, 100, 250}); // 6

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// QS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of QS.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 * \param[in] superpixel_tolerances
 */
void connector_QS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels, std::vector<int> superpixel_tolerances) {
    
    for (unsigned int k = 0; k < 2; k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/qs_cli/qs_dispatcher.sh",
                "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/qs_cli > /dev/null");
        tool.addSuperpixelTolerance(superpixels[k], superpixel_tolerances[k]);

        tool.addFloatParameter("ratio", "-c", std::vector<float>{0.0f, 0.5f, 1.0f}); // 3
        tool.addIntegerParameter("kernel-size", "-k", std::vector<int>{1, 3, 5, 7}); // 4
        tool.addIntegerParameter("max-distance", "-k", std::vector<int>{6, 10, 14}); // 3
        tool.addIntegerParameter("color-space", "-r", std::vector<int>{0, 1}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// NC
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of NC.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_NC(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < 1; k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/nc_cli/nc_dispatcher.sh",
                "-e " + MATLAB_EXECUTABLE + " -a " + RELATIVE_PATH + "/nc_cli > /dev/null");

        tool.addIntegerParameter("superpixels", "-s", std::vector<int>{superpixels[k]});
        tool.addIntegerParameter("coarse-superpixels", "-c", std::vector<int>{0, superpixels[k]/4, superpixels[k]/2}); // 3
        tool.addIntegerParameter("eigenvectors", "-g", std::vector<int>{40, 200}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// VCCS
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of VCCS.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] depth_directory
 * \param[in] intrinsics_directory
 * \param[in] superpixels
 */
void connector_VCCS(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        boost::filesystem::path depth_directory, boost::filesystem::path intrinsics_directory,
        std::vector<int> superpixels) {
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/vccs_cli", "");
        tool.useDepth(depth_directory);

        if (!intrinsics_directory.empty()) {
            tool.useIntrinsics(intrinsics_directory);
        }

        tool.addFloatParameter("voxel-resolution", "--voxel-resolution", std::vector<float>{0.002f, 0.004f, 0.008f}); // 3

        if (superpixels[k] < 800) {
            tool.addFloatParameter("seed-resolution", "--seed-resolution", std::vector<float>{0.14f, 0.16f}); // 2
        }
        else if (superpixels[k] >= 800 && superpixels[k] < 2400) {
            tool.addFloatParameter("seed-resolution", "--seed-resolution", std::vector<float>{0.1f, 0.11f}); // 2
        }
        else {
            tool.addFloatParameter("seed-resolution", "--seed-resolution", std::vector<float>{0.05f, 0.06f}); // 2
        }

        tool.addFloatParameter("spatial-weight", "--spatial-weight", std::vector<float>{0.0f, 0.25f, 0.5f}); // 3
        tool.addFloatParameter("normal-weight", "--normal-weight", std::vector<float>{0.0f, 0.5f}); // 2

        tool.optimize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// VC
////////////////////////////////////////////////////////////////////////////////

/** \brief Connector for parameter optimization of VC.
 *
 * \param[in] img_directory
 * \param[in] gt_directory
 * \param[in] base_directory
 * \param[in] superpixels
 */
void connector_VC(boost::filesystem::path img_directory, 
        boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
        std::vector<int> superpixels) {
    
    
    for (unsigned int k = 0; k < superpixels.size(); k++) {
        ParameterOptimizationTool tool(img_directory, gt_directory,
                base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/vc_cli", "");      

        tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
        tool.addFloatParameter("weight", "--weight", std::vector<float>{10, 25, 50, 100, 250}); // 5
        tool.addIntegerParameter("radius", "--radius", std::vector<int>{3, 15}); // 2
        tool.addIntegerParameter("threshold", "--threshold", std::vector<int>{10});
        tool.addIntegerParameter("neighboring-clusters", "--neighboring-clusters", std::vector<int>{200, 400}); // 2
        tool.addIntegerParameter("direct-neighbors", "--direct-neighbors", std::vector<int>{4, 16}); // 2

        tool.optimize();
    }
}

/** \brief Optimize parameters for the different algorithms. 
 * Usage:
 * \code{sh}
 *   $ ../bin/eval_parameter_optimization_cli --help
 *   Allowed options:
 *     --img-directory arg                   image directory
 *     --gt-directory arg                    ground truth directory
 *     --base-directory arg                  base directory
 *     --algorithm arg                       algorithm to optimize: reseeds, 
 *     --depth-directory arg                 depth directory
 *     --intrinsics-directory arg            intrinsics directory
 *     --matlab-executable arg (=../../MATLAB/R2014b/release/matlab)
 *                                           matlab executable path
 *     --java-executable arg (=../../jdk-1.8.0_45/release/java)
 *                                           java executable
 *     --not-fair                            do not use fair parameters
 *     --help                                produce help message
 * \endcode
 * \author David Stutz
 */
int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("img-directory", boost::program_options::value<std::string>(), "image directory")
        ("gt-directory", boost::program_options::value<std::string>(), "ground truth directory")
        ("base-directory", boost::program_options::value<std::string>(), "base directory")
        ("algorithm", boost::program_options::value<std::string>(), "algorithm to optimize: reseeds, ")
        ("depth-directory", boost::program_options::value<std::string>()->default_value(""), "depth directory")
        ("intrinsics-directory", boost::program_options::value<std::string>()->default_value(""), "intrinsics directory")
        ("matlab-executable", boost::program_options::value<std::string>()->default_value("../../MATLAB/R2014b/release/matlab"), "matlab executable path")
        ("java-executable", boost::program_options::value<std::string>()->default_value("../../jdk-1.8.0_45/release/java"), "java executable")
        ("not-fair", "do not use fair parameters")
        ("help", "produce help message");

    boost::program_options::positional_options_description positionals;
    positionals.add("algorithm", 1);
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
    
    MATLAB_EXECUTABLE = parameters["matlab-executable"].as<std::string>();
    JAVA_EXECUTABLE = parameters["java-executable"].as<std::string>();
    
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
    
    boost::filesystem::path depth_directory(parameters["depth-directory"].as<std::string>());
    if (!depth_directory.empty()) {
        if (!boost::filesystem::is_directory(depth_directory)) {
            std::cout << "Depth directory does not exist." << std::endl;
            return 1;
        }
    }
    
    boost::filesystem::path intrinsics_directory(parameters["intrinsics-directory"].as<std::string>());
    if (!intrinsics_directory.empty()) {
        if (!boost::filesystem::is_directory(intrinsics_directory)) {
            std::cout << "Intrinsics directory does not exist." << std::endl;
            return 1;
        }
    }
    
    if (parameters.find("not-fair") != parameters.end()) {
        FAIR = "";
    }
        
    std::string algorithm = parameters["algorithm"].as<std::string>();
    std::transform(algorithm.begin(), algorithm.end(), algorithm.begin(), 
            ::tolower);
    
    std::vector<int> superpixels = {400, 1200, 3600};
    std::vector<int> superpixel_tolerances = {250, 500, 750};
    
    if (algorithm == "ccs") {
        connector_CCS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "cis") {
        connector_CIS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "crs") {
        connector_CRS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "cw") {
        connector_CW(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "dasp") {
        if (depth_directory.empty()) {
            std::cout << "DASP requires a depth directory to be given." << std::endl;
            return 1;
        }
        if (intrinsics_directory.empty()) {
            std::cout << "Using default intrinsics for NYUV2." << std::endl;
        }
        
        connector_DASP(img_directory, gt_directory, base_directory, depth_directory, 
                intrinsics_directory, superpixels);
    }
    else if (algorithm == "eams") {
        connector_EAMS(img_directory, gt_directory, base_directory, superpixels, 
                superpixel_tolerances);
    }
    else if (algorithm == "ergc") {
        connector_ERGC(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "ers") {
        connector_ERS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "etps") {
        connector_ETPS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "fh") {
        connector_FH(img_directory, gt_directory, base_directory, superpixels, 
                superpixel_tolerances);
    }
    else if (algorithm == "mss") {
        connector_MSS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "pb") {
        connector_PB(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "poise") {
        connector_POISE(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "preslic") {
        connector_preSLIC(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "reseeds") {
        connector_reSEEDS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "seaw") {
        connector_SEAW(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "seeds") {
        connector_SEEDS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "slic") {
        connector_SLIC(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "tp") {
        connector_TP(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "tps") {
        connector_TPS(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "vlslic") {
        connector_vlSLIC(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "w") {
        connector_W(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "wp") {
        connector_WP(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "pf") {
        connector_PF(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "lsc") {
        connector_LSC(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "rw") {
        connector_RW(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "qs") {
        connector_QS(img_directory, gt_directory, base_directory, superpixels, 
                superpixel_tolerances);
    }
    else if (algorithm == "nc") {
        connector_NC(img_directory, gt_directory, base_directory, superpixels);
    }
    else if (algorithm == "vccs") {
        if (depth_directory.empty()) {
            std::cout << "VCCS requires a depth directory to be given." << std::endl;
            return 1;
        }
        if (intrinsics_directory.empty()) {
            std::cout << "Using default intrinsics for NYUV2." << std::endl;
        }
        
        connector_VCCS(img_directory, gt_directory, base_directory, depth_directory, 
                intrinsics_directory, superpixels);
    }
    else if (algorithm == "vc") {
        connector_VC(img_directory, gt_directory, base_directory, superpixels);
    }
    else {
        std::cout << "Invalid algorithm." << std::endl;
        return 1;
    }
    
    return 0;
}
