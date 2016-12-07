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

function [] = sunrgbd_copy_depth(root_dir, meta, depth_dir)
% Copy the depth files.
%
% INPUT:
% root_dir:         root directory (one directory up of the SUNRGBD
%                   directory)
% meta:             meta information from the meta .mat file
% depth_dir:        directory to put depth images
%

    mkdir(depth_dir);

    index = 0;
    for i = 1: 10335
        if strcmp(meta(1, i).sequenceName(13:19), 'NYUdata') ~= 1
            % This is not an NYUV2 image ...
            depth_path = strrep(meta(1, i).depthpath, '/n/fs/sun3d/data/', '');
            depth_path = strrep(depth_path, '/depth/', '/depth_bfx/');
            depth = imread([root_dir '/' depth_path]);
            depth = bitor(bitshift(depth, -3), bitshift(depth, 16 - 3));
            %depth = single(depth)/1000; 
            % depth(depth > 8000) = 8000;
            
            imwrite(uint16(depth), [depth_dir '/' sprintf('%08d', index) '.png']);
            index = index + 1;
        end;
    end;
end

