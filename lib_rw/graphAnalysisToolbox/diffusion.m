function out=diffusion(L,input,iterations)
%function out=diffusion(L,input,iterations) computes anisotropic diffusion
%for iterations number of time steps with a time step given by timestep
%
%Inputs:    L - Laplacian (weighted) matrix
%           input - Input vector to be diffused
%           timestep - The time step used in diffusion
%           iterations - The number of iterations to diffuse
%
%Outputs:   out - The diffused column vector normalized to the same range
%                 as the input vector
%
%
%4/11/03 - Leo Grady
%Note 1: A scale of more than 3-4 when determining weights is likely to be
%too great to allow any real diffusion.
%Note 2: Iterations on the order of 10-20 are sufficient to produce
%reasonably blurred results

% Copyright (C) 2002, 2003 Leo Grady <lgrady@cns.bu.edu>
%   Computer Vision and Computational Neuroscience Lab
%   Department of Cognitive and Neural Systems
%   Boston University
%   Boston, MA  02215
%
% This program is free software; you can redistribute it and/or
% modify it under the terms of the GNU General Public License
% as published by the Free Software Foundation; either version 2
% of the License, or (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
%
% Date - $Id: diffusion.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Constants
timestep=.1; %Larger timesteps may result in instability.  
%Smaller timesteps may produce no effects

%Inputs
[a b]=size(input);
if b > a %Error catch for column/row vector
    input=input';
end
%input=input(:); %Error catch for column/row vector
minInput=min(min(input));
maxInput=max(max(input));

%Iterate
out=input;
for k=1:iterations
    out = out - timestep*L*out;
end

%Renormalize
out=out-min(min(out))+minInput;
out=out*maxInput./max(max(out));
