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

function [] = sunrgbd_make_subsets(image_dir, gt_dir, depth_dir)
% Randomly select images for training and testing set.
% Note that this will overwrite sungrbd_train.txt and sunrgbd_test.txt
%
% INPUT:
% image_dir:            directory for the images
% gt_dir:               directory for the ground truths
% depth_dir:            directory for the depth images
%

    % 8886 image sin total
    delete('sunrgbd_train.txt');
    delete('sunrgbd_test.txt');

    list_train = fopen('sunrgbd_train.txt', 'w');
    list_test = fopen('sunrgbd_test.txt', 'w');
    
    system(sprintf('mkdir -p %s', [image_dir '/train']));
    system(sprintf('mkdir -p %s', [image_dir '/test']));
    
    system(sprintf('mkdir -p %s', [gt_dir '/train']));
    system(sprintf('mkdir -p %s', [gt_dir '/test']));
    
    system(sprintf('mkdir -p %s', [depth_dir '/train']));
    system(sprintf('mkdir -p %s', [depth_dir '/test']));
    
    train_count = 0;
    test_count = 0;
    
    files = dir([image_dir '/*.png']);
    for i = 1: size(files, 1)
        image_path = [image_dir '/' files(i).name];
        gt_path = [gt_dir '/' files(i).name(1: size(files(i).name, 2) - 4) '.csv'];
        depth_path = [depth_dir '/' files(i).name];
        
        r = rand();
        if r <= 0.025 && train_count < 200
            %image_dest = [image_dir '/train/' files(i).name];
            %copyfile(image_path, image_dest);
            
            %gt_dest = [gt_dir '/train/' files(i).name(1: size(files(i).name, 2) - 4) '.csv'];
            %copyfile(gt_path, gt_dest);
            
            %depth_dest = [depth_dir '/train/' files(i).name];
            %copyfile(depth_path, depth_dest);
            
            %delete(image_path);
            %delete(gt_path);
            %delete(depth_path);
            
            fprintf(list_train, '%s\n', files(i).name(1: size(files(i).name, 2) - 4));
            train_count = train_count + 1;
        else if r >= (1 - 0.05) && test_count < 400
            %image_dest = [image_dir '/test/' files(i).name];
            %copyfile(image_path, image_dest);
            
            %gt_dest = [gt_dir '/test/' files(i).name(1: size(files(i).name, 2) - 4) '.csv'];
            %copyfile(gt_path, gt_dest);
            
            %depth_dest = [depth_dir '/test/' files(i).name];
            %copyfile(depth_path, depth_dest);
            
            %delete(image_path);
            %delete(gt_path);
            %delete(depth_path);
            
            fprintf(list_test, '%s\n', files(i).name(1: size(files(i).name, 2) - 4));
            test_count = test_count + 1;
        end;
    end;
end

