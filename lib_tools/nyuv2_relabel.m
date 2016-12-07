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

function [relabeled_ground_truth] = nyuv2_relabel(ground_truth)
% Relabel the given ground truth.
%
% INPUT.
% ground_truth:             ground truth segmentation containing segment indices
%
% OUTPUT:
% relabeled_ground_truth:   relabeled ground truth
%

    % For zero indexed labels!
    gt = ground_truth + 1;
    % gt = ground_truth;
    
    num_labels = max(max(gt));
    labels = ones(num_labels, 1)*(-1);
    label = 0;
    
    height = size(gt, 1);
    width = size(gt, 2);
    
    relabeled_ground_truth = zeros(height, width);
    for i = 1: height
        for j = 1: width
            
            % Check whether this is a new label:
            if labels(gt(i, j)) == -1
                labels(gt(i, j)) = label;
                label = label + 1;
            end;

            relabeled_ground_truth(i, j) = labels(gt(i, j));
        end;
    end;

end

