function imgOut=colorseg2bwseg(img,whiteout)
%Function imgOut=colorseg2bwseg(img,whiteout) takes images with a red color 
%segment outline and returns an image with a black segment outline 
%(i.e., suitable for publication)
%
%Inputs:    img - An image with color only on the outline of segments 
%           whiteout - Optional number in the interval [0,1] indicating the level
%               of "whiting out" in the image
%
%Outputs:   imgOut - An image with a black border around segments
%
%
%Note: Function whitens image so as to accentuate borders
%
%5/24/03 - Leo Grady

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
% Date - $Id: colorseg2bwseg.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

if nargin < 2
    whiteout=.3;
end

%Find outlines
redSegs=find(img(:,:,1)>.6 & img(:,:,2)<.4);

%Whiten image
imgOut=img(:,:,2);
imgOut=imgOut./max(max(imgOut));
imgOut=imgOut.^whiteout;

%Black out segments
imgOut(redSegs)=0;
