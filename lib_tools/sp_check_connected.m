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

function unconnected_components = sp_check_connected(given_labels)
% Check if the labels represent connected superpixels. This is merely
% a utility. Instead of checking it is more efficient to simply relabel
% the labels to be connected using sp_fast_connected_relabel.
%
% INPUT:
% given_labels:             superpixel labels with superpixel indeices
%
% OUTPUT:
% unconnected_components:   number of unconnected components
%

    labels = given_labels + 1;
    max_label = max(max(labels));
    
    superpixels = zeros(max_label, 1);
    for i = 1: size(labels, 1)
        for j = 1: size(labels, 2)
            superpixels(labels(i, j), 1) = superpixels(labels(i, j), 1) + 1;
        end;
    end;
    
    unconnected_components = 0;
    for k = 1: max_label
        if superpixels(k, 1) > 0
            label_k = (labels == k);
            cc = bwconncomp(label_k);
            
            if cc.NumObjects > 1
                unconnected_components = unconnected_components + 1;
            end;
        end;
    end;
end

