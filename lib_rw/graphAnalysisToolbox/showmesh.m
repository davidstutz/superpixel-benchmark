function showmesh(points,vals,faces)
%Function showmesh(points,vals,faces) displays an image of the current values 
%of a graph, interpolated across the faces
%
%Inputs:    points - The Nx2 node set
%           vals - An Nx1 or Nx3 list of image values (i.e., grayscale or
%                  RGB)
%           faces - The QxK face matrix with Q faces and K sides
%
%
%5/27/03 - Leo Grady

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
% Date - $Id: showmesh.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Create patch
ph = patch('Vertices',points,'Faces',faces);

%Color faces
colormap('gray')    
set(ph, 'FaceVertexCData',vals);
    
%Perform shading
set(ph,'FaceColor', 'interp');

%Remove lines
if (nargin < 4) | ~lines
    set(ph,'LineStyle','None')
end

%Set axis properties
warning off %Matlab throws warnings otherwise
axis equal
axis tight
axis off
warning on
