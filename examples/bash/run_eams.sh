#!/bin/sh
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

# Example of using and evaluating EAMS on the BSDS500.
# Supposed to be run from within examples/.

# ../eams_cli/eams_dispatcher.sh is the command line tool for EAMS indirectly calling MatLab.
# ../bin/eval_summary_cli takes the generated superpixel labels as CSV, the original
# images and ground truths as CSV to evaluate the superpixels.
# ../bin/eval_visualization_cli takes the generated superpixel labels as CSV, the
# original images and creates the specified visualizations (i.e. --contours).

# Note that both lib_tools and lib_eams need to be compiled using the make.m scripts.
# Note that this assumes the following setup:
# lib_eams/
# |- edison_wrapper.m
# eams_cli/
# |- eams_dispatcher.m
# |- eams_cli.m
# lib_tools/
# |- fast_connected_relabel.cpp

# This should be the path to your matlab executable!
MATLAB="/home/david/MATLAB/R2015b/bin/matlab"

../eams_cli/eams_dispatcher.sh -i ../data/BSDS500/images/test/ -b 2 -m 50 -r 1 -o ../output/eams -w -a ../eams_cli -e $MATLAB
../bin/eval_summary_cli --sp-directory ../output/eams --img-directory ../data/BSDS500/images/test --gt-directory ../data/BSDS500/csv_groundTruth/test
../bin/eval_visualization_cli --csv ../output/eams --images ../data/BSDS500/images/test --contours --vis ../output/eams