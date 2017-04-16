#
# Copyright (c) 2016, David Stutz 
# Contact: david.stutz@rwth-aachen.de, davidstutz.de
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
cmake_minimum_required(VERSION 2.8)
project(superpixel_benchmark)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS  "-Wall -Wno-sign-compare -g -std=c++0x -O4 -fpermissive")

# "Core algorithms" as recommended:
option(BUILD_ETPS "Build ETPS" ON)
option(BUILD_SLIC "Build SLIC" ON)
option(BUILD_SEEDS "Build SEEDS" ON)
option(BUILD_RESEEDS "Build reSEEDS" ON)
option(BUILD_ERS "Build ERS" ON)
option(BUILD_CRS "Build CRS" ON)
option(BUILD_ERGC "Build ERGC" ON)

# Remaining algorithms:
option(BUILD_FH "Build FH" OFF)
option(BUILD_MSS "Build MSS" OFF)
option(BUILD_PB "Build PB" OFF)
option(BUILD_PRESLIC "Build preSLIC" OFF)
option(BUILD_CW "Build CW" OFF)
option(BUILD_CIS "Build CIS" OFF)
option(BUILD_W "Build W" OFF)
option(BUILD_LSC "Build LSC" OFF)
option(BUILD_VC "Build VC" OFF)
option(BUILD_CCS "Build CCS" OFF)
option(BUILD_VC "Build VC" OFF)
option(BUILD_CCS "Build CCS" OFF)
option(BUILD_DASP "Build DASP" OFF)
option(BUILD_VCCS "Build VCCS" OFF)
option(BUILD_REFH "Build reFH" ON)
option(BUILD_VLSLIC "Build vlSLIC" OFF)

# Examples:
option(BUILD_EXAMPLES "Build examples" ON)

# Toolbox
add_subdirectory(lib_eval)
add_subdirectory(eval_connected_relabel_cli)
add_subdirectory(eval_boundaries2labels_cli)
add_subdirectory(eval_parameter_optimization_cli)
add_subdirectory(eval_summary_cli)
add_subdirectory(eval_average_cli)
add_subdirectory(eval_visualization_cli)
add_subdirectory(eval_visualization_fuse_cli)

if(BUILD_ETPS)
    add_subdirectory(lib_etps)
    add_subdirectory(etps_cli)
endif()

if(BUILD_SLIC)
    add_subdirectory(lib_slic)
    add_subdirectory(slic_cli)
endif()

if(BUILD_SEEDS)
    add_subdirectory(lib_seeds)
    add_subdirectory(seeds_cli)
endif()

if(BUILD_ERS)
    add_subdirectory(lib_ers)
    add_subdirectory(ers_cli)
endif()

if(BUILD_CRS)
    add_subdirectory(crs_cli)
endif()

if(BUILD_ERGC)
    add_subdirectory(ergc_cli)
endif()

if(BUILD_FH)
    add_subdirectory(fh_cli)
endif()

if(BUILD_MSS)
    add_subdirectory(mss_cli)
endif()

if(BUILD_PB)
    add_subdirectory(lib_pb)
    add_subdirectory(pb_cli)
endif()

if(BUILD_CIS)
    add_subdirectory(lib_cis)
    add_subdirectory(cis_cli)
endif()

if(BUILD_PRESLIC)
    add_subdirectory(lib_preslic)
    add_subdirectory(preslic_cli)
endif()

if(BUILD_CW)
    add_subdirectory(lib_cw)
    add_subdirectory(cw_cli)
endif()

if(BUILD_W)
    add_subdirectory(w_cli)
endif()

if(BUILD_LSC)
    add_subdirectory(lsc_cli)
endif()

if(BUILD_VC)
    add_subdirectory(vc_cli)
endif()

if(BUILD_CCS)
    add_subdirectory(lib_ccs)
    add_subdirectory(ccs_cli)
endif()

if(BUILD_VCCS)
    add_subdirectory(lib_vccs)
    add_subdirectory(vccs_cli)
endif()

if(BUILD_DASP)
    add_subdirectory(lib_dasp)
    add_subdirectory(dasp_cli)
endif()

if(BUILD_REFH)
    add_subdirectory(refh_cli)
endif()

if(BUILD_VLSLIC)
    add_subdirectory(lib_vlslic)
    add_subdirectory(vlslic_cli)
endif()

if(BUILD_RESEEDS)
    add_subdirectory(reseeds_cli)
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(examples/cpp)
endif()
