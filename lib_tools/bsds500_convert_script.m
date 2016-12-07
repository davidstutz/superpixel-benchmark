% Copyright (c) 2016, David Stutz
% Contact: david.stutz@rwth-aachen.de, davidstutz.de
% All rights reserved.
% 
% Redistribution and use in source and binary forms, with or without modification,
% are permitted provided that the following conditions are met:
% 
% 1. Redistributions of source code must retain the above copyright notice,
%    this list of conditions and the following disclaimer.
% 
% 2. Redistributions in binary form must reproduce the above copyright notice,
%    this list of conditions and the following disclaimer in the
%    documentation and/or other materials provided with the distribution.
% 
% 3. Neither the name of the copyright holder nor the names of its contributors
%    may be used to endorse or promote products derived from this software
%    without specific prior written permission.
% 
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
% ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
% THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
% DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
% LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
% ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
% OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
% OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

% This scrip is intended to convert the BSDS500 dataset to the used format.
% In the case of the BSDS500, only the ground truth segmentations need to
% be converted from .mat to .csv files.

% INSTRUCTIONS:
% 1. Download the BSDS500 dataset from 
%    https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/resources.html
% 2. Extract the BSR/BSDS500/data folder into data/BSDS500 (overwriting the
%    provided examples in data/BSDS500/images.
%    Also note that afterwards there are three folders: groundTruth,
%    csv_groundTruth and images.
% 3. Adapt the path to the directory below, i.e. set BSDS500_DIR
%    correctly.
% 4. Run this script.

% NOTE THAT THIS SCRIPT MAY TAKE SOME TIME TO FINSIH.
% On 8GB RAM, i5, Ubuntu 12.04, roughly 200 seconds were needed without
% computing edges.

% BSDS500_DIR is the base directory of the BSDS500 containing the "images"
% and "groundTruth" subdirectories.
BSDS500_DIR = '/home/david/superpixels/release/data/BSDS500/'; % With trailing "/"!

fprintf('Converting ground truth set ... ');
tic;
bsds500_convert([BSDS500_DIR 'groundTruth/val'], [BSDS500_DIR 'csv_groundTruth/val']);
bsds500_convert([BSDS500_DIR 'groundTruth/test'], [BSDS500_DIR 'csv_groundTruth/test']);
bsds500_convert([BSDS500_DIR 'groundTruth/train'], [BSDS500_DIR 'csv_groundTruth/train']);
elapsed = toc;
fprintf('done (%f).\n', elapsed);