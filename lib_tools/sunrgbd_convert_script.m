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

% This script is intended to convert the SUNRGBD dataset into the used
% format.

% INSTRUCTIONS:
% 1. Download the SUNRGBD dataset from http://rgbd.cs.princeton.edu/.
% 2. Make sure todownload botht he SUNRGBD V1 dataset and the
%    SUNRGBDtoolbox containing the annotations.
% 3. From the SUNRGBDtoolbox extract SUNRGBD2dseg.mat and SUNRGBDMeta.mat
%    to data/SUNRGBD.
% 4. From the SUNRGBD V1 dataset extract all files into data/SUNRGBD; note
%    that this may take quite some time!
%    It might be wise to extract the contained directories (xtion,
%    realsense, kv1, kv2) separately as this may take some time!
% 5. Adapt the ROOT_DIR variable below to point to the data directory (i.e. the
%    parent directory of the SUNRGBD directory).
% 6. Run this script.

% NOTE THAT THIS SCRIPT MAY TAKE SOME TIME TO FINISH.
% On 8GB RAM, i5 Ubuntu 12.04, it took ~ 17475 + 1290 + 864 + 94 + 56 seconds.

% ROOT_DIR is the root directory for all datasets, i.e. without the SUNRGBD
% directory name.
% MAT_FILE is the path to the SUNRGBD segmentation .mat file.
% META_FILE ist he path to the SUNRGBD meta .mat file.
% SUNRGBD_EDGES should be greater zero to generate edges.
SUNRGBD_ROOT_DIR = '/home/david/superpixels/release/data/'; % Note without the SUNRGBD root dir!
SUNRGBD_MAT_FILE = [SUNRGBD_ROOT_DIR 'SUNRGBD/SUNRGBD2Dseg.mat'];
SUNRGBD_META_FILE = [SUNRGBD_ROOT_DIR 'SUNRGBD/SUNRGBDMeta.mat'];
SUNRGBD_LIST_TRAIN = [SUNRGBD_ROOT_DIR 'SUNRGBD/sunrgbd_train.txt'];
SUNRGBD_LIST_TEST = [SUNRGBD_ROOT_DIR 'SUNRGBD/sunrgbd_test.txt'];

train_list = dlmread(SUNRGBD_LIST_TRAIN);
test_list = dlmread(SUNRGBD_LIST_TEST);

load(SUNRGBD_META_FILE);

fprintf('Generating ground truth (may take some time) ... ');
tic;
sunrgbd_generate_ground_truth(SUNRGBD_MAT_FILE, SUNRGBDMeta, [SUNRGBD_ROOT_DIR 'SUNRGBD/csv_groundTruth']);
elapsed = toc;
fprintf('done (%f).\n', elapsed);

fprintf('Copying images ... ');
tic;
sunrgbd_copy_images(SUNRGBD_ROOT_DIR, SUNRGBDMeta, [SUNRGBD_ROOT_DIR 'SUNRGBD/images']);
elapsed = toc;
fprintf('done (%f).\n', elapsed);

fprintf('Copying depth ... ');
tic;
sunrgbd_copy_depth(SUNRGBD_ROOT_DIR, SUNRGBDMeta, [SUNRGBD_ROOT_DIR 'SUNRGBD/depth']);
elapsed = toc;
fprintf('done (%f).\n', elapsed);

fprintf('Copying intrinsics ... ');
tic;
sunrgbd_copy_intrinsics(SUNRGBD_ROOT_DIR, SUNRGBDMeta, [SUNRGBD_ROOT_DIR 'SUNRGBD/intrinsics']);
elapsed = toc;
fprintf('done (%f).\n', elapsed);

fprintf('Collecting subsets ... ');
tic;
sunrgbd_collect_subsets(train_list, test_list, [SUNRGBD_ROOT_DIR 'SUNRGBD/images'], [SUNRGBD_ROOT_DIR 'SUNRGBD/csv_groundTruth'], ...
        [SUNRGBD_ROOT_DIR 'SUNRGBD/depth'], [SUNRGBD_ROOT_DIR 'SUNRGBD/intrinsics']);
elapsed = toc;
fprintf('done (%f).\n', elapsed);
