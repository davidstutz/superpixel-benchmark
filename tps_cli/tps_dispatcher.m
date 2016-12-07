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

lib_path = [pwd '/../lib_tps/'];
if exist(lib_path) == 0
    warning('The lib_tps directory could not be found on the same level as tps_cli!');
    warning('Quitting!');
    quit
else
    addpath(genpath(lib_path));
end

tools_path = [pwd '/../lib_tools/'];
if exist(tools_path) == 0
    warning('The lib_tools directory could not be found on the same level as tps_cli!');
    warning('Quitting!');
    quit
else
    addpath(genpath(tools_path));
end

sfedges_path = [pwd '/../lib_sfedges/'];
if exist(tools_path) == 0
    warning('The lib_sfedges directory could not be found on the same level as tps_cli!');
    warning('Quitting!');
    quit
else
    addpath(genpath(sfedges_path));
end
        
if exist('debug') ~= 1
    debug = 0;
end;

if debug > 0
    fprintf('Running TPS:\n');
    fprintf('superpixels: %d\n', superpixels);
    fprintf('gamma: %f\n', gamma);
    fprintf('sigma: %f\n', sigma);
    fprintf('fair: %d\n', fair);
    fprintf('csv_folder: %s\n', csv_folder);
    fprintf('vis_folder: %s\n', vis_folder);
    fprintf('prefix: %s\n', prefix);
    fprintf('wordy: %d\n', wordy);
end;
    
if exist('edgesTrain') == 0
    warning('lib_sfedges was added, however, edgesTrain could not be found. Is lib_sfedges complete? Check the repository again, or download the edge detection toolbox from https://github.com/pdollar/edges and the corresponding image processing toolbox from https://github.com/pdollar/toolbox.');
    quit
end

opts = edgesTrain();
opts.modelDir = [pwd '/../lib_sfedges//models/'];
opts.modelFnm = 'modelBsds';
opts.nPos = 5e5; 
opts.nNeg = 5e5;
opts.useParfor = 0;
opts.bsdsDir = folder;

model = edgesTrain(opts);

model.opts.multiscale = 0;
model.opts.sharpen = 2;
model.opts.nTreesEval = 4;
model.opts.nThreads =4;
model.opts.nms = 0;

%try
    tps_cli(folder, model, superpixels, gamma, sigma, fair, csv_folder, vis_folder, prefix, wordy);
%catch e
%    fprintf([e.message '\n'])
%end