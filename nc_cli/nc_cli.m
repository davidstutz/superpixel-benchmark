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

function [] = nc_cli(folder, superpixels, superpixels_coarse, eigenvectors, csv_folder, vis_folder, prefix, wordy)
% Compute superpixels using NC.
%
% INPUT:
% folder:               folder to process
% superpixels:          number of superpixels
% superpixels_coarse:   coarse superpixels, if bigger than zero, this is used for 
%                       normalized cuts and the reamining superpixels are computed 
%                       by kmeans clustering the coarse ones
% eigenvectors:         number of eigenvectors
% csv_folder:           folder to output superpixels as CSV files
% vis_folder:           folder to output visualizations
% prefix:               prefix for output files
% wordy:                verbose output
%
    if exist('imncut_sp') == 0
        error('The lib_nc/imncut_sp function could not be found, was lib_nc added? Exiting nc_cli!')
        return
    end
    
    if exist('imnb') == 0
        error('The lib_nc/imncut_sp function could not be found, was lib_nc added? If lib_nc was added, was it compiled? Use lib_nc/make.m! Exiting nc_cli!')
        return
    end
            
    if exist('pbCGTG') == 0
        error('The lib_pbedges/pbCGTG function could not be found, was lib_pbedges added?')
        return
    end
    
    % Default parameters for reference.
    if nargin < 2
        superpixels = 400;
    end;
    
    if nargin < 3
        superpixels_coarse = 0;
    end;
    
    if nargin < 4
        eigenvectors = 40;
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
    
    % Turn off warnings!
    warning('off', 'all');
    
    files = dir([folder '/*.jpg']);
    files = [files; dir([folder '/*.png'])];
    
    total = 0;
    for i = 1: size(files, 1)
        if i > 100
            break;
        end;
            
        filepath = [folder '/' files(i).name];
        I = imread(filepath);
        image = im2double(I);

        [height, width] = size(image);

        diag_length = sqrt(height*height + width*width);
        par = imncut_sp;
        par.int = 0;
        par.pb_ic = 1;
        par.sig_pb_ic = 0.05;
        par.sig_p = ceil(diag_length/50);
        par.verbose = 0;
        par.nb_r = ceil(diag_length/60);
        par.rep = -0.005;  % stability?  or proximity?
        par.sample_rate = 0.2;
        par.nv = eigenvectors;
        par.verbose = 0;
        par.pb_timing = 0;
        
        % If a coarse number of superpixels is given, we run NC on the
        % coarse number of superpixels and then cluster using k-means
        % (see lib_nc/sp_demo), otherwise we only use NC.
        if superpixels_coarse > 0
            par.sp = superpixels_coarse;
        else
            par.sp = superpixels;
        end;
        
        tic;
        [emag , ephase] = pbWrapper(image, par.pb_timing);
        emag = pbThicken(emag);
        par.pb_emag = emag;
        par.pb_ephase = ephase;
        clear emag ephase;
        
        labels = zeros(height, width);
        if superpixels_coarse > 0
            labels_coarse = imncut_sp(image, par);
            labels = clusterLocations(labels_coarse, height*width/superpixels/2);
        else
            labels = imncut_sp(image, par);
        end;
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
            contour_image = sp_contours(I, labels);
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
    
    % Turn on again, in case something is done afterwards ...
    warning('on', 'all');
end
