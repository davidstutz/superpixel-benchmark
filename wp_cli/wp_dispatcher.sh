#!/bin/bash
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

# Usage:
# $ ../wp_cli/wp_dispatcher.sh -h
# Allowed options:
# -i folder to process
# -s number of superpixels
# -w compactness parameter
# -f filter option
# -o output folder
# -v folder for visualizatioN
# -x prefix of output files
# -w verbose output
# -e path to MatLab executable
# -a path to add, usually the lib_seaw path

DEBUG=0
INPUT=""
# How to compute step from superpixel number:
#   floor(0.5f + sqrt(image.rows*image.cols) / superpixels);
SUPERPIXELS=400
WEIGHT=1
VIS=""
PREFIX=""
FILTER=0
OUTPUT=""
while getopts ":i:s:w:f:o:v:x:e:a:h" opt; do
    case $opt in
        i)
            INPUT=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Input (-i) set to: $OPTARG"
            fi
            ;;
        s)
            SUPERPIXELS=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Superpixels (-s) set to: $OPTARG"
            fi
            ;;
        w)
            WEIGHT=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Weight (-w) set to: $OPTARG"
            fi
            ;;
        f)
            FILTER=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Filter (-f) set to: $OPTARG"
            fi
            ;;
        o)
            OUTPUT=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Output (-o) set to: $OPTARG"
            fi
            ;;
        v)
            VIS=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Vis (-v) set to: $OPTARG"
            fi
            ;;
        x)
            PREFIX=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Prefix (-v) set to: $OPTARG"
            fi
            ;;
        e)
            MATLAB=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "MatLab executable (-e) set to: $OPTARG"
            fi
            ;;
        a)
            ADDPATH=$OPTARG
            if [ $DEBUG -gt 0 ] ; then
                echo "Add path (-a) set to: $OPTARG"
            fi
            ;;
        h)
            echo "Allowed options:"
            echo "-i folder to process"
            echo "-s number of superpixels"
            echo "-w compactness parameter"
            echo "-f filter option"
            echo "-o output folder"
            echo "-v folder for visualizatioN"
            echo "-x prefix of output files"
            echo "-w verbose output"
            echo "-e path to MatLab executable"
            echo "-a path to add, usually the lib_seaw path"
            exit
            ;;
        \?)
            if [ $DEBUG -gt 0 ] ; then
                echo "Invalid option: -$OPTARG."
            fi
            exit 1
            ;;
        :)
            if [ $DEBUG -gt 0 ] ; then
                echo "Invalid option: -$OPTARG."
            fi
            exit 1
            ;;
    esac
done

if [ "$FILTER" -gt 0 ]; then
    FILTER="--filter_ori"
else
    FILTER="--no-filter_ori"
fi

if [ -n "$VIS" ]; then
    if [ -n "$OUTPUT" ]; then
        # Nothing here
        "" > /dev/null
    else
        OUTPUT=$VIS
    fi
fi

if [ -z $INPUT ] ; then
    "Use -i to provide the input directory."
    exit 1
fi

mkdir -p $OUTPUT

for f in $INPUT/*
do
    if [ $DEBUG -gt 0 ] ; then
        echo "Executing: python ./lib_wp/demo_waterpixels_smil_with_parser.py --original_image $f --superpixels $SUPERPIXELS --weight $WEIGHT $FILTER --output $OUTPUT"
    fi
    if [ -n "$PREFIX" ]; then
        python ./lib_wp/demo_waterpixels_smil_with_parser.py --original_image $f --superpixels $SUPERPIXELS --weight $WEIGHT $FILTER --output $OUTPUT --prefix $PREFIX
    else
        python ./lib_wp/demo_waterpixels_smil_with_parser.py --original_image $f --superpixels $SUPERPIXELS --weight $WEIGHT $FILTER --output $OUTPUT
    fi
done