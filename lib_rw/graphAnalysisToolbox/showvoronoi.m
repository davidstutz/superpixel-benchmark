function showvoronoi(vals, voronoiStruct)
%Function showvoronoi(vals, voronoiStruct)
%displays a graph by the voronoi cells
%
%Inputs:    vals - An Nx1 (grayscale) or Nx3 (RGB) matrix indicating the 
%               value of each node
%           voronoiStruct - Outputs from the function voronoicells()
%
%Outputs:   Function outputs visualization to the current figure
%
%
%5/20/03 - Leo Grady

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
% Date - $Id: showvoronoi.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Create patch
ph = patch('Vertices', voronoiStruct.pts, 'Faces', voronoiStruct.faces);

%Color faces
set(ph, 'FaceVertexCData', vals(voronoiStruct.index,:));
set(ph,'FaceColor', 'flat');

%Fill in lines, if display mesh
set(ph,'LineStyle','None')

%Set axis properties
warning off %Avoids persistant warning 
%   "RGB not yet supported in Painter's Mode"
axis equal
axis tight
axis off
colormap(gray)
warning on
