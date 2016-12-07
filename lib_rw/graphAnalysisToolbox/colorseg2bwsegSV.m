function outVals=colorseg2bwsegSV(vals)
%Function outVals=colorseg2bwsegSV(vals) takes RGB values with a red
%color segment outline and returns an image with a black segment outline 
%(i.e., suitable for publication)
%
%Inputs:    vals - An Nx3 matrix of RGB values for each node with color 
%                  only on the outline of segments (red)
%
%Outputs:   outVals - An Nx1 vector of the whitened image with a black 
%                  border around segments
%
%
%Note: Function whitens image so as to accentuate borders
%
%6/2/03 - Leo Grady

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
% Date - $Id: colorseg2bwsegSV.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Find outlines
redSegs=find(vals(:,1)>.6 & vals(:,2)<.4);

%Whiten image
outVals=vals(:,2);
outVals=outVals./max(max(outVals));
outVals=outVals.^.45;

%Black out segments
outVals(redSegs)=0;
