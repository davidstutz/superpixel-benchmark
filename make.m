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

% Build script for all MatLab algorithms and tools.
% Use the variables below to select the components to build.
% Also see the README.md to add further components, e.g. the
% pB edge detection toolbox or Dollar's edge detection toolbox
% to use with TPS.
%
% David Stutz <david.stutz@rwth-aachen.de>

% Adapt these variables to select components
BUILD_TOOLS = 1;
BUILD_EAMS = 0;
BUILD_NC = 0;
BUILD_QS = 0;
BUILD_SEAW = 0;
BUILD_TP = 0;
BUILT_TPS = 0;

home = pwd;
if BUILD_TOOLS > 0
    cd('lib_tools')
    
    try
        make;
    catch e
        fprintf('An error ocurred when building TOOLS. Check lib_tools/make.m for details.\n');
        fprintf('Error message: %s\n', e.message);
    end
    
    cd(home)
end

if BUILD_EAMS > 0
    cd('lib_eams')
    
    try
        make;
    catch e
        fprintf('An error ocurred when building EAMS. Check lib_eams/make.m for details.\n');
        fprintf('Error message: %s\n', e.message);
    end
    
    cd(home)
end

if BUILD_NC > 0
    cd('lib_nc')
    
    try
        make;
    catch e
        fprintf('An error ocurred when building NC. Check lib_nc/make.m for details.\n');
        fprintf('Error message: %s\n', e.message);
    end
    
    cd(home)
end

if BUILD_QS > 0
    cd('lib_qs')
    
    try
        make;
    catch e
        fprintf('An error ocurred when building QS. Check lib_qs/make.m for details.\n');
        fprintf('Error message: %s\n', e.message);
    end
    
    cd(home)
end

if BUILD_SEAW > 0
    cd('lib_seaw')
    
    try
        make;
    catch e
        fprintf('An error ocurred when building SEAW. Check lib_seaw/make.m for details.\n');
        fprintf('Error message: %s\n', e.message);
    end
    
    cd(home)
end

if BUILD_TP > 0
    cd('lib_tp')
    
    try
        make;
    catch e
        fprintf('An error ocurred when building TP. Check lib_tp/make.m for details.\n');
        fprintf('Error message: %s\n', e.message);
    end
    
    cd(home)
end