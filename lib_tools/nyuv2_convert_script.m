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

% This script is intended to convert the NYUV2 dataset into the used
% format.

% INSTRUCTIONS:
% 1. Download the dataset from http://cs.nyu.edu/~silberman/datasets/nyu_depth_v2.html
%    Make sure that the downloaded file is nyu_depth_v2_labeled.mat.
% 2. Put the file in data/NYUV2.
% 3. Make sure that data/NYUV2 contains nyuv2_test_subset.txt,
% nyuv2_train_subset.txt, nyuv2_test.txt and nyuv2_train.txt.
% 4. Adapt the variables below, in particular NYUV2_DIR to point to the
%    data/NYUV2 directory.
% 5. Run this script.

% NOTE THAT THIS SCRIPT MAY TAKE SOME TIME AND RESOURCES TO FINISH.
% On a 8GB RAM, i5 Ubuntu 12.04 machine, it took ~1000 seconds and ~2 GB
% RAM.

% NYUV2_DIR is the direcotry were the training and testing sets are put.
% NYUV2_MAT is the path to the .mat file provided by the dataset, it should
% be put in NYUV2_DIR.
% NYUV2_LIST_TRAIN is the path to nyuv2_train_subset.txt containing the
% images for the training set.
% NYUV2_LIST_TEST is the path to nyuv2_test_subset.txt containing the 
% images for the testing set.
% FACTOR is the size factor used for the images, should be 1!
NYUV2_DIR = '/home/david/superpixels/release/data/NYUV2/'; % With trailing "/"!
NYUV2_MAT = [NYUV2_DIR 'nyu_depth_v2_labeled.mat'];
NYUV2_LIST_TRAIN = [NYUV2_DIR 'nyuv2_train_subset.txt'];
NYUV2_LIST_TEST = [NYUV2_DIR 'nyuv2_test_subset.txt'];
FACTOR = 1;

list_train = textread(NYUV2_LIST_TRAIN);
list_test = textread(NYUV2_LIST_TEST);

% Generate images.
fprintf('Generating images ... ');
tic;
load(NYUV2_MAT, 'images');
nyuv2_generate_images(images, list_train, [NYUV2_DIR '/images/train'], FACTOR);
nyuv2_generate_images(images, list_test, [NYUV2_DIR '/images/test'], FACTOR);
clear images
elapsed = toc;
fprintf('done (%f).\n', elapsed);

% Generate depth.
fprintf('Generating depth ... ');
tic;
load(NYUV2_MAT, 'depths');
nyuv2_generate_depth(depths, list_train, [NYUV2_DIR '/depth/train'], FACTOR);
nyuv2_generate_depth(depths, list_test, [NYUV2_DIR '/depth/test'], FACTOR);
clear depths
elapsed = toc;
fprintf('done (%f).\n', elapsed);

% Generate ground truth in BSDS500 format.
fprintf('Generating ground truth (may take longer) ... ');
tic;
load(NYUV2_MAT, 'images', 'labels', 'instances');
mask_border = nyuv2_find_border(images);
nyuv2_generate_ground_truth(labels, instances, mask_border, list_train, [NYUV2_DIR '/groundTruth/train'], FACTOR);
nyuv2_generate_ground_truth(labels, instances, mask_border, list_test, [NYUV2_DIR '/groundTruth/test'], FACTOR);
clear labels
clear instances
elapsed = toc;
fprintf('done (%f).\n', elapsed);

% Convert ground truth to CSV.
fprintf('Converting ground truth (may take longer) ... ');
tic;
nyuv2_convert([NYUV2_DIR '/groundTruth/train/'], [NYUV2_DIR '/csv_groundTruth/train/']);
nyuv2_convert([NYUV2_DIR '/groundTruth/test/'], [NYUV2_DIR '/csv_groundTruth/test/']);
elapsed = toc;
fprintf('done (%f).\n', elapsed);

% Crop everything.
fprintf('Cropping images ... ');
tic;
nyuv2_crop_images([NYUV2_DIR 'images/train'], 16);
nyuv2_crop_images([NYUV2_DIR 'images/test'], 16);
elapsed = toc;
fprintf('done (%f).\n', elapsed);

fprintf('Cropping depth ... ');
tic;
nyuv2_crop_images([NYUV2_DIR 'depth/train'], 16);
nyuv2_crop_images([NYUV2_DIR 'depth/test'], 16);
elapsed = toc;
fprintf('done (%f).\n', elapsed);

fprintf('Cropping ground truth ... ');
tic;
nyuv2_crop_ground_truth([NYUV2_DIR 'csv_groundTruth/train'], 16);
nyuv2_crop_ground_truth([NYUV2_DIR 'csv_groundTruth/test'], 16);
elapsed = toc;
fprintf('done (%f).\n', elapsed);