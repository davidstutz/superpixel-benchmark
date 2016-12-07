function vals=rgbimg2vals(img,normFlag)
%Function vals=rgbimg2vals(img,normFlag) inputs an RGB image and 
%outputs an Nx3 vector of vals
%
%Inputs:    img - The image to be vectorized
%           normFlag - Optional argument specifying a normalized output
%
%Outputs:   vals - An Nx3 vector of RGB vals when N=X*Y, [X Y Z]=size(img)
%
%
%5/25/03 - Leo Grady

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
% Date - $Id: rgbimg2vals.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Read arguments
if nargin < 2
    normFlag=0;
end

%Decompose image into channels
redChannel=img(:,:,1);
greenChannel=img(:,:,2);
blueChannel=img(:,:,3);

%Compose vals
vals=[redChannel(:),greenChannel(:),blueChannel(:)];

%Perform normalization
if normFlag
    vals=normalize(vals);
end
