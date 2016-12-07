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

function [edges] = im_sf_edges(image)
% Compute edges on the given image using Dollar's structured forest for
% fast edge detection toolbox.
%
% Note that both of Dollar's toolboxes need to be installed. Install them
% in lib_tools/sf_toolbox and lib_tools/sf_edges respectively.
%
% INPUT:
% image:        image to compute edges on
%
% OUTPUT:
% edges:        edge map computed for the image
%

    if exist('edgesDetect') == 0
        % TODO add in separate lib folder
        addpath(genpath('sf_toolbox'));
        addpath(genpath('sf_edges'));
    end;
    
    opts = edgesTrain(); % default options (good settings)
    
    current_dir = pwd;
    if strcmp(current_dir(end - 9: end), 'lib_tools')
        opts.modelDir = '../sf_edges/models/'; % model will be in models/forest
    else
        opts.modelDir = 'sf_edges/models/'; % model will be in models/forest
    end;
    
    opts.modelFnm = 'modelBsds'; % model name
    opts.nPos = 5e5; 
    opts.nNeg = 5e5; % decrease to speedup training
    opts.useParfor = 0; % parallelize if sufficient memory

    model = edgesTrain(opts); % will load model if already trained

    model.opts.multiscale = 0; % for top accuracy set multiscale=1
    model.opts.sharpen = 2; % for top speed set sharpen=0
    model.opts.nTreesEval = 4; % for top speed set nTreesEval=1
    model.opts.nThreads =4; % max number threads for evaluation
    model.opts.nms = 0; % set to true to enable nms

    edges = edgesDetect(image, model);
end

