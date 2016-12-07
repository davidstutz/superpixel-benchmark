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

function [image_contours] = sp_contours(image, labels, color)
% Draw superpixel contours into the given image using the given contour
% color.
%
% INPUT.
% image:            image to draw contours in
% labels:           superpixel labels
% color:            color for contours (default is black)
%
% OUTPUT:
% image_contours:   image with superpixel contours
%

    if nargin < 3
        color = [0, 0, 0];
    end;
    
    image_contours = image;
    [height, width, ~] = size(image);

    for i = 1: height
        for j = 1: width
            if is_boundary_pixel(labels, i, j)
                image_contours(i, j, :) = color;
            end;
        end;
    end;
end

function [boundary] = is_boundary_pixel(labels, i, j)

    [height, width] = size(labels);
    
    boundary = 0;
    if labels(i, j) ~= labels(min(i + 1, height), j)
        boundary = 1;
    end;
    if labels(i, j) ~= labels(max(i - 1, 1), j)
        boundary = 1;
    end;
    if labels(i, j) ~= labels(i, min(j + 1, width))
        boundary = 1;
    end;
    if labels(i, j) ~= labels(i, max(j - 1, 1))
        boundary = 1;
    end;
    
    if labels(i, j) ~= labels(min(i + 1, height), min(j + 1, width))
        boundary = 1;
    end;
    if labels(i, j) ~= labels(max(i - 1, 1), max(j - 1, 1))
        boundary = 1;
    end;
    if labels(i, j) ~= labels(max(i - 1, 1), min(j + 1, width))
        boundary = 1;
    end;
    if labels(i, j) ~= labels(min(i + 1, height), max(j - 1, 1))
        boundary = 1;
    end;
end