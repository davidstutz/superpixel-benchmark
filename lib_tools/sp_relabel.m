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

function [relabeled_labels] = sp_relabel(given_labels)
% Relabel the superpixel segmentation.
%
% INPUT:
% given_labels:     superpixel labels
%
% OUTPUT:
% relabeled_labels: relabeled superpixel labels
%

    labels = given_labels + 1;
    max_label = max(max(labels));
    
    superpixels = zeros(max_label, 1);
    for i = 1: size(labels, 1)
        for j = 1: size(labels, 2)
            superpixels(labels(i, j), 1) = superpixels(labels(i, j), 1) + 1;
        end;
    end;
    
    label = 1;
    label_correspondence = zeros(max_label, 1);
    
    relabeled_labels = zeros(size(labels, 1), size(labels, 2));
    for i = 1: size(labels, 1)
        for j = 1: size(labels, 2)
            if superpixels(labels(i, j), 1) > 0
                if label_correspondence(labels(i, j), 1) == 0
                    label_correspondence(labels(i, j), 1) = label;
                    label = label + 1;
                end;
                
                relabeled_labels(i, j) = label_correspondence(labels(i, j), 1);
            end;
        end;
    end;
    
    min_label = min(min(relabeled_labels));
    assert(min_label >= 1);
    
    relabeled_labels = relabeled_labels - 1;
end

