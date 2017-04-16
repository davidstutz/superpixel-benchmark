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

# Example of using and evaluating ETPS on the BSDS500.
# Supposed to be run from within examples/.

# ../bin/etps_cli is the command line tool for ETPS.
# ../bin/eval_summary_cli takes the generated superpixel labels as CSV, the original
# images and ground truths as CSV to evaluate the superpixels.
# ../bin/eval_visualization_cli takes the generated superpixel labels as CSV, the
# original images and creates the specified visualizations (i.e. --contours).

../bin/etps_cli --input ../data/BSDS500/images/test/ --superpixels 1200 --regularization-weight 0.01 --length-weight 0.1 --size-weight 1 --iterations 25 -o ../output/etps -w
../bin/eval_summary_cli --sp-directory ../output/etps --img-directory ../data/BSDS500/images/test --gt-directory ../data/BSDS500/csv_groundTruth/test
../bin/eval_visualization_cli --csv ../output/etps --images ../data/BSDS500/images/test --contours --vis ../output/etps