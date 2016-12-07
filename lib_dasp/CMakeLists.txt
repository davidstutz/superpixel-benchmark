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
cmake_minimum_required (VERSION 2.8)
project (superpixel_benchmark)

# Dependencies.
add_subdirectory(lib_dasp_danvil)
add_subdirectory(lib_dasp_density)
add_subdirectory(lib_dasp_graphseg)
add_subdirectory(lib_dasp_pds)
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)

include_directories(${EIGEN_INCLUDE_DIRS}
    lib_dasp/
    lib_dasp_common/
    lib_dasp_slimage/ 
    lib_dasp_danvil/
    lib_dasp_pds/
    lib_dasp_density/
    lib_dasp_graphseg/
)
add_library(dasp
    dasp_opencv.cpp
    lib_dasp/impl/RepairDepth.cpp
    lib_dasp/impl/Sampling.cpp 
    lib_dasp/eval/Recall.cpp
    lib_dasp/eval/ipq.cpp
    lib_dasp/eval/ev.cpp
    lib_dasp/eval/ce.cpp 
    lib_dasp/eval/misc.cpp
    lib_dasp/eval/use.cpp
    lib_dasp/Neighbourhood.cpp 
    lib_dasp/Plots.cpp
    lib_dasp/Segmentation.cpp
    lib_dasp/Superpixels.cpp
    lib_dasp/IO.cpp
)
target_link_libraries(dasp
    ${OpenCV_LIBS}
    dasp_pds
    dasp_graphseg
    boost_thread
)
