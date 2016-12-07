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

function [] = fash_generate_groundtruth(truths, gt_dir)
% Generate the ground truth .csv files for the Fashionista dataset.
%
% INPUT:
% truths:       ground truth matrices obtained fromthe Fashionista .mat
% git_dir:      directory to put the ground truth .csv files
%

    system(sprintf('mkdir -p %s', gt_dir));

    for l = 1: size(truths, 2)
        sp_segmentation = fash_imdecode(truths(l).annotation.superpixel_map);
        sp_labels = truths(l).annotation.superpixel_labels;
        
        labels = zeros(size(sp_segmentation, 1), size(sp_segmentation, 2));
        
        for i = 1: size(labels, 1)
            for j = 1: size(labels, 2)
                labels(i, j) = sp_labels(sp_segmentation(i, j), 1);
            end;
        end;
        
        % Now get connected components to resolve semantic labels
        max_label = max(max(labels));
        current_label = max_label + 1;
        
        for i = 1: max_label
            label_i = (labels == i);
            
            cc = bwconncomp(label_i, 4);
            if cc.NumObjects > 1
                for j = 2: cc.NumObjects
                    ids = cc.PixelIdxList{j};
                    
                    for k = 1: size(ids, 1)
                        [ii, jj] = ind2sub(cc.ImageSize, ids(k));
                        labels(ii, jj) = current_label;
                    end;
                    
                    current_label = current_label + 1;
                end;
            end;
        end;
        
        csvwrite([gt_dir '/' sprintf('%03d', l) '.csv'], labels);
    end;
end

