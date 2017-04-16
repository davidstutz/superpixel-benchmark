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

# Supposed to be run after building the examples from the root directory
# where the BSDS500 dataset has already been extracted and converted as
# described in docs/EXAMPLE.md.

for SUPERPIXEL in "200" "300" "400" "600" "800" "1000" "1200" "1400" "1600" "1800" "2000" "2400" "2800" "3200" "3600" "4000" "4600" "5200"
do
    ./bin/w_cli data/BSDS500/images/test/ --superpixels $SUPERPIXEL -o experiments/test/$SUPERPIXEL -w
    ./bin/eval_summary_cli experiments/test/$SUPERPIXEL data/BSDS500/images/test data/BSDS500/csv_groundTruth/test --append-file experiments/test/w.csv --vis
    find ./output/w/$SUPERPIXEL -type f -name '*[^summary|correlation|results].csv' -delete
done

./bin/eval_average_cli experiments/test/w.csv -o output/w_average.csv