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
        
function [] = seaw_cli(folder, level, dist_func, sigma, csv_folder, vis_folder, prefix, wordy)
% Compute superpixels using SEAW.
%
% INPUT:
% folder:       folder to process
% level:        number of levels
% dist_func:    distance function to use, see paper
% sigma:        sigma parameter, see paper
% csv_folder:   folder to output superpixels as CSV files
% vis_folder:   folder to output visualizations
% prefix:       prefix for output files
% wordy:        verbose output
%
    
    
    
    % Default parameters for reference!
    if nargin < 2
        level = 4;
    end;

    if nargin < 3
        dist_func = 1;
    end;

    if nargin < 4
        sigma = 1;
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
        double_image = double(image) / 255;

        wave_type = 1;
        
        % Number of octaves that may be computed; implicitly defining the 
        % possible numbers of superpixels.
        % Number of superpixels is 2^level*floor(H/2^(levels - 1))*2^level*floor(W/2^(levels-1)).
        tic;
        levels = floor(log2(min(size(double_image(:, :, 1)))));
        [A, W] = EAW(double_image(:,:,1), levels, wave_type, dist_func, sigma);
        
        for j = levels: -1: level

            s = j + 1;

            im = zeros(size(A{s}));
            imcell = cell(numel(im), 1);
            for k = 1: numel(im)

                im(k) = 1;

                for l = 1: j
                    ss = cell(2,1);
                    ss{2} = im;
                    ss{1} = zeros(size(A{s-l}));
                    w = cell(1,1);
                    w{1} = W{s-l};
                    im = igEAW(ss, w, wave_type); 
                end;
                
                imcell{k} = im;
                im = zeros(size(A{s}));
            end;
        end;
        
        m = zeros([size(imcell{1}, 1) size(imcell{1}, 2) numel(im)]);
        for c = 1: numel(im)
            m(:,:,c) = imcell{c};
        end

        [~, labels] = max(m,[],3);
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

