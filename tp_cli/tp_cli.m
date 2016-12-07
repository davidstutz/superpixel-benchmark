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
        
function [] = tp_cli(folder, sp, time_step, max_iterations, sigma_factor, csv_folder, vis_folder, prefix, wordy)
% Compute superpixels using TP.
%
% INPUT:
% folder:           folder to process
% sp:               number of superpixels
% time_step:        time step to use
% max_iterations:   maximum number of iterations
% sigma_factor:     sigma parameter, see paper
% csv_folder:       folder to output superpixels as CSV files
% vis_folder:       folder to output visualizations
% prefix:           prefix for output files
% wordy:            verbose output
%

    if exist('superpixels') == 0
        error('Could not find function lib_tp/superpixels, was lib_tp added? Exiting tp_cli!')
        return
    end
    
    % Default parameters for reference
    if nargin < 2
        sp = 400;
    end;
    
    if nargin < 3
        time_step = 0.5
    end;
    
    if nargin < 4
        max_iterations = 500;
    end;
    
    if nargin < 5
        sigma_factor = 2.5;
    end;
    
    if nargin < 6
        csv_folder = '';
    end;
    
    if nargin < 7
        vis_folder = '';
    end;
    
    if nargin < 8
        prefix = '';
    end;
    
    if nargin < 9
        wordy = 0;
    end;
    
    mkdir(csv_folder);

    if isempty(vis_folder) == 0
        mkdir(vis_folder);
    end;

    current_dir = pwd;
    if strcmp(current_dir(end - 5: end), 'tp_cli')
        addpath('../lib_tp/');
        addpath('../lib_tp/lsmlib');
        addpath('../lib_tools');
    else
        addpath('lib_tp/');
        addpath('lib_tp/lsmlib');
        addpath('lib_tools');
    end;

    files = dir([folder '/*.jpg']);
    files = [files; dir([folder '/*.png'])];
    
    total = 0;
    for i = 1: size(files, 1)
        filepath = [folder '/' files(i).name];
        
        I = imread(filepath);
        image = im2double(I);
        
        tic;
        [phi,boundary,~,~]  = superpixels(image, sp, time_step,  ...
                max_iterations, sigma_factor);
        elapsed = toc;
        total = total + elapsed;
        
        if wordy > 0
            fprintf('Computed boundaries in %f seconds.\n', elapsed);
        end;
        
        boundary(boundary == 1) = -1;
        if isempty(csv_folder) == 0
            csvwrite([csv_folder '/' prefix files(i).name(1:end - 4) '.csv'], boundary);
        end;
        
        if isempty(vis_folder) == 0
            contour_image = sp_contours(I, boundary);
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
