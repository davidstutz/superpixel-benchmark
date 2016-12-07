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

% Script demonstrating visualization capabilities provided in lib_tools.
ground_truth_directory = '../../data/BSDS500/csv_groundTruth/test';
image_directory = '../../data/BSDS500/images/test';
output_directory = '../../output/matlab_visualize';

addpath('../../lib_tools');

if exist(output_directory) == 0
    mkdir(output_directory);
end;

image_files = dir([image_directory '/*.jpg']);
for i = 1: size(image_files, 1)
    name = image_files(i).name(1, 1:size(image_files(i).name, 2) - 4);
    labels = dlmread([ground_truth_directory '/' name '-0.csv']);
    
    image = imread([image_directory '/' image_files(i).name]);
    image_random = sp_random(labels);
    image_contours = sp_contours(image, labels);
    
    imwrite(image_random, [output_directory '/' name '_random.png']);
    imwrite(image_contours, [output_directory '/' name '_contours.png']);
end;