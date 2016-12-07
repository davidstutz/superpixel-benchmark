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

% Script demonstrating the use of sp_fast_connected_components.
superpixel_directory = '../data/';
image_directory = '../../data/BSDS500/images/test';
output_directory = '../../output/matlab_connected_components';

addpath('../../lib_tools');

if exist(output_directory) == 0
    mkdir(output_directory);
end;

superpixel_files = dir([superpixel_directory '/*.csv']);
for i = 1: size(superpixel_files, 1)
    name = superpixel_files(i).name(1, 1: size(superpixel_files(i).name, 2) - 4);
    labels = dlmread([superpixel_directory '/' superpixel_files(i).name]);
    image = imread([image_directory '/' name '.jpg']);
    
    number_superpixels = sp_count(labels);
    image_random = sp_random(labels);
    
    labels_connected = sp_fast_connected_relabel(labels);
    number_superpixels_connected = sp_count(labels_connected);
    image_random_connected = sp_random(labels_connected);
    
    imwrite(image_random, [output_directory '/' name '_random.png']);
    imwrite(image_random_connected, [output_directory '/' name '_random_connected.png']);
    
    fprintf('Image %s: %d superpixels before / %d superpixels after.\n', ...
        name, number_superpixels, number_superpixels_connected);
end;