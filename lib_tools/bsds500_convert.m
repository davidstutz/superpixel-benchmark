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

function [] = bsds500_convert(gt_dir, out_dir)
% Convert the BSDS500 ground truth .mat files to .csv files.
%
% INPUT:
% gt_dir:   directory containing .mat ground truth files
% out_dir:  directory to write .csv files
%

    system(sprintf('mkdir -p %s', out_dir));
    
    files = dir([gt_dir '/*.mat']);
    for i = 1: size(files, 1)
        file_path = [gt_dir '/' files(i).name];
        load(file_path, 'groundTruth');
        
        for j = 1: size(groundTruth, 2)
            out_file = [out_dir '/' files(i).name(1, 1:size(files(i).name, 2) - 4) '-' sprintf('%d', j - 1) '.csv'];
            
            ground_truth = groundTruth{1, j}.Segmentation;
            ground_truth = bsds500_relabel(ground_truth);
            
            csvwrite(out_file, ground_truth);
        end;
    end;
end

