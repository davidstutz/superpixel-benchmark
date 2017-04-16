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

# Example of evaluating and comparing FH and reFH.
# Supposed to be run from within examples/.

MINIMUM_SIZE=("100" "75" "50" "45" "40" "35" "30" "25" "20" "15" "10" "10" "10" "10" "10" "10" "10" "10")
THRESHOLD=("250" "200" "175" "150" "135" "120" "105" "90" "75" "60" "45" "30" "15" "10" "5" "1" "0.5" "0.1")

for I in `seq 0 17`
do
    # reFH
    ../bin/refh_cli ../data/BSDS500/images/test/ --sigma 0 --threshold "${THRESHOLD[$I]}" --minimum-size "${MINIMUM_SIZE[$I]}" -o ../output/refh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -w
    ../bin/eval_summary_cli ../output/refh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" ../data/BSDS500/images/test ../data/BSDS500/csv_groundTruth/test --append-file ../output/refh.csv --vis
    find ../output/refh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -type f -name '*[^summary|correlation|results|average].csv' -delete
    
    # FH
    ../bin/fh_cli ../data/BSDS500/images/test/ --sigma 0 --threshold "${THRESHOLD[$I]}" --minimum-size "${MINIMUM_SIZE[$I]}" -o ../output/fh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -w
    ../bin/eval_summary_cli ../output/fh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" ../data/BSDS500/images/test ../data/BSDS500/csv_groundTruth/test --append-file ../output/fh.csv --vis
    find ../output/fh/"${MINIMUM_SIZE[$I]}_${THRESHOLD[$I]}" -type f -name '*[^summary|correlation|results|average].csv' -delete
done
