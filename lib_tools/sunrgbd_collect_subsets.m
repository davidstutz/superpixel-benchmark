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

function [] = sunrgbd_collect_subsets(train_list, test_list, image_dir, gt_dir, depth_dir, intrinsics_dir)
% Collect the training and testing subsets for the SUNRGBD dataset.
%
% INPUT:
% train_list:       list of training examples (i.e. the corresponding
%                   indices), from reading sunrgbd_train.txt using dlmread
% test_list:        list of test examples (i.e. the corresponding
%                   indices), from reading sunrgbd_test.txt using dlmread
% image_dir:        directory containing images
% gt_dir:           directory containing ground truths
% depth_dir:        directory containing depth images
% intrinsics_dir:   directory containing intrinsic parameters
%
    
    system(sprintf('mkdir -p %s', [image_dir '/train']));
    system(sprintf('mkdir -p %s', [image_dir '/test']));
    
    system(sprintf('mkdir -p %s', [gt_dir '/train']));
    system(sprintf('mkdir -p %s', [gt_dir '/test']));
    
    system(sprintf('mkdir -p %s', [depth_dir '/train']));
    system(sprintf('mkdir -p %s', [depth_dir '/test']));
    
    system(sprintf('mkdir -p %s', [intrinsics_dir '/train']));
    system(sprintf('mkdir -p %s', [intrinsics_dir '/test']));
    
    for i = 1: size(train_list, 1)
        name = sprintf('%08d', train_list(i));
        target_name = sprintf('%08d', train_list(i));
        
        image_path = [image_dir '/' name '.png'];
        image_dest = [image_dir '/train/' target_name '.png'];
        
        copyfile(image_path, image_dest);
        delete(image_path);
        
        gt_path = [gt_dir '/' name '.csv'];
        gt_dest = [gt_dir '/train/' target_name '.csv'];
        
        copyfile(gt_path, gt_dest);
        delete(gt_path);
        
        depth_path = [depth_dir '/' name '.png'];
        depth_dest = [depth_dir '/train/' target_name '.png'];
        
        copyfile(depth_path, depth_dest);
        delete(depth_path);
        
        intrinsics_path = [intrinsics_dir '/' name '.csv'];
        intrinsics_dest = [intrinsics_dir '/train/' target_name '.csv'];
        
        copyfile(intrinsics_path, intrinsics_dest);
        delete(gt_path);
    end;
    
    for i = 1: size(test_list, 1)
        name = sprintf('%08d', test_list(i));
        target_name = sprintf('%08d', test_list(i));
        
        image_path = [image_dir '/' name '.png'];
        image_dest = [image_dir '/test/' target_name '.png'];
        
        copyfile(image_path, image_dest);
        delete(image_path);
        
        gt_path = [gt_dir '/' name '.csv'];
        gt_dest = [gt_dir '/test/' target_name '.csv'];
        
        copyfile(gt_path, gt_dest);
        delete(gt_path);
        
        depth_path = [depth_dir '/' name '.png'];
        depth_dest = [depth_dir '/test/' target_name '.png'];
        
        copyfile(depth_path, depth_dest);
        delete(depth_path);
        
        intrinsics_path = [intrinsics_dir '/' name '.csv'];
        intrinsics_dest = [intrinsics_dir '/test/' target_name '.csv'];
        
        copyfile(intrinsics_path, intrinsics_dest);
    end;
end

