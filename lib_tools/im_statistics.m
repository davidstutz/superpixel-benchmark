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

function [count, avg_height, avg_width, min_height, min_width, max_height, max_width] = im_statistics(directory)
% Compute statistics over the directory containing several images;
% average, minimum and maximum height/width will be calculated.
%
% INPUT:
% directory:        directory containing the images
%
% OUTPUT:
% count:            number of images
% avg_height:       average height
% avg_width:        average width
% min_height:       minimum height
% min_width:        minimum width
% max_height:       maximum height
% max_width:        maximum width
%

    files = dir([directory '/*.jpg']);
    files = [files; dir([directory '/*.png'])];
    
    count = size(files, 1);
    avg_height = 0;
    avg_width = 0;
    min_height = 0;
    min_width = 0;
    max_height = 0;
    max_width = 0;
    
    for i = 1: size(files, 1)
        image = imread([directory '/' files(i).name]);
        [height, width, ~] = size(image);
        
        avg_height = avg_height + height;
        avg_width = avg_width + width;
        
        if height > max_height
            max_height = height;
        end;
        
        if width > max_width
            max_width = width;
        end;
        
        if height < min_height || min_height == 0
            min_height = height;
        end;
        
        if width < min_width || min_width == 0
            min_width = width;
        end;
    end;
    
    if size(files, 1) > 0
        avg_height = avg_height / size(files, 1);
        avg_width = avg_width / size(files, 1);
    end;
end

