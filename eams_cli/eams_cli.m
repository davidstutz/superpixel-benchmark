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
        
function [] = eams_cli(folder, bandwidth, minimum_size, color_space, csv_folder, vis_folder, prefix, wordy)
% Compute superpixels using EAMS.
%
% Note that lib_eams is expected to be in the search path such that
% lib_eams/edison_wrapper can be found.
%
% Also assumes that lib_tools was added to the path!
%
% INPUT:
% folder:       folder to process
% bandwidth:    bandwidth to use
% minimum_size: minimum size of superpixels
% color_space:  color space touse, 0 for Luv (default), > 0 for RGB
% csv_folder:   folder to output superpixels as CSV files
% vis_folder:   folder to output visualizations
% prefix:       prefix for output files
% wordy:        verbose output
%

    if exist('edison_wrapper') == 0
        error('The lib_eams/edison_wrapper function could not be found, was lib_eams added? Exiting eams_cli!')
        return
    end
    
    if exist('edison_wrapper_mex') == 0
        error('The lib_eams/edison_wrapper_mex function could not be found, was lib_eams added? If lib_eams was added, was edison_wrapper_mex compiled? Use lib_eams/make.m! Exiting eams_cli!')
        return
    end
    
    % Default parameters for reference!
    if nargin < 2
        bandwidth = 1;
    end;
    
    if nargin < 3
        minimum_size = 20;
    end;
    
    if nargin < 4
        color_space = 0;
    end;

    if nargin < 5
        csv_folder = '';
    end;

    if nargin < 6
        vis_folder = '';
    end;
    
    if nargin < 7
        prefix = '';
    end;
    
    if nargin < 8
        wordy = 0;
    end;
    
    mkdir(csv_folder);
    
    if isempty(vis_folder) == 0
        mkdir(vis_folder);
    end;
    
    files = dir([folder '/*.jpg']);
    files = [files; dir([folder '/*.png'])];
    
    total = 0;
    for i = 1: size(files, 1)
        filepath = [folder '/' files(i).name];
        image = imread(filepath);
        
        tic;
        if color_space > 0
            [fimage, labels, modes, regSize, grad, conf] = edison_wrapper(image, ...
                    @RGB2RGB, 'SpatialBandWidth', bandwidth, 'MinimumRegionArea', ...
                    minimum_size);
        else
            [fimage, labels, modes, regSize, grad, conf] = edison_wrapper(image, ...
                    @RGB2Luv, 'SpatialBandWidth', bandwidth, 'MinimumRegionArea', ...
                    minimum_size);
        end;
        elapsed = toc;
        total = total + elapsed;
        
        labels = sp_fast_connected_relabel(double(labels));
        
        if wordy > 0
            count = sp_count(labels);
            unconnected_count = sp_check_connected(labels);
            fprintf('%d superpixels for %s (%d not connected; %f).\n', count, filepath, unconnected_count, elapsed);
        end;
        
        if isempty(csv_folder) == 0
            csvwrite([csv_folder '/' prefix files(i).name(1:end - 4) '.csv'], labels);
        end;
        
        if isempty(vis_folder) == 0
            contour_image = sp_contours(image, labels);
            imwrite(contour_image, [vis_folder '/' prefix files(i).name(1:end - 4) '.png']);
        end;
    end;
    
    if wordy > 0
        fprintf('Average time: %f.\n', total / size(files, 1));
    end;
    
    if isempty(csv_folder) == 0
        runtime_file = fopen([csv_folder '/' prefix 'runtime.txt'], 'a');
        fprintf(runtime_file, '%f\n', total / size(files, 1));
    end;
end

