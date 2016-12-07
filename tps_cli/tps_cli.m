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
        
function [] = tps_cli(folder, model, superpixels, gamma, sigma, fair, csv_folder, vis_folder, prefix, wordy)
% Compute superpixels using TPS.
%
% INPUT:
% folder:           folder to process
% model:            structured forest edge detection model, see
%                   lib_sfedeges or poise_lic/poise_dispatcher.m
% superpixels:      number of superpixels
% gamma:            gamma parameter, see paper
% sigma:            sigma parameter, see paper
% fair:             whether to use for fair evaluation, then region height and width used to determine
%                   the number of superpixels take the same value
% csv_folder:       folder to output superpixels as CSV files
% vis_folder:       folder to output visualizations
% prefix:           prefix for output files
% wordy:            verbose output
%

    if exist('edgesDetect') == 0
        error('The lib_sfedges/edgesDetect function could not be found. Make sure to add lib_sfedges to the path!')
        return
    end
    
    if exist('imPadMex') == 0
        error('lib_sfedges was included, but the imPadMex function not found, whch means lib_sfedges was not compiled. Use lib_sfedges/make.m or download the compiled version from https://github.com/pdollar/edges and https://github.com/pdollar/toolbox.')
        return
    end
    
    if exist('Get_Regular_SP_alt') == 0
        error('Could not find method lib_tps/Get_Regular_SP_alt.m, was lib_tps added?')
        return
    end
            
    % Default parameters for reference!
    if nargin < 2
        superpixels = 400;
    end;

    if nargin < 3
        type = 2;
    end;

    if nargin < 4
        gamma = 0.5;
    end;

    if nargin < 5
        sigma = 0.1;
    end;

    if nargin < 6
        fair = 0;
    end;
            
    if nargin < 7
        csv_folder = '';
    end;
    
    if nargin < 8
        vis_folder = '';
    end;
    
    if nargin < 9
        prefix = '';
    end;
    
    if nargin < 10
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

        [height, width, ~] = size(image);
        
        [region_height, region_width] = sp_height_width_superpixels(height, ...
                width, superpixels);
        if fair > 0
            region_height = sp_region_size_superpixels(height, ...
                    width, superpixels);
            region_width = region_height;
        end;
        
        tic;
        edge_map = edgesDetect(image, model);
        
        labels = Get_Regular_SP_alt(image, edge_map, region_height, region_width, gamma, sigma);
        elapsed = toc;
        total = total + elapsed;
        
        labels = sp_fast_connected_relabel(labels);
        
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
