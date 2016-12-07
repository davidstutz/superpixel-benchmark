function [points,edges]=pdf2graph(pdfImg,N,K)
%function [points,edges]=pdf2graph(pdfImg,N,K) converts a probability 
%density function (pdf) image into a graph of nodes and edges with N nodes
%
%Inputs:    pdfImg - An image representing a probability at each pixel
%                    (i.e. sum(sum(pdfImg)) = 1)
%           N - Number of samples (nodes) to generate from the pdf
%           K - Optional argument.  If supplied, produces edge list
%              of each node connected to its K nearest neighbors.
%              Otherwise edge produced with Delaunay
%
%Outputs:   points - An Nx2 set of coordinates for the nodes
%           edges - An Mx2 set of edges connecting the points
%
%Note1: Points are dithered to avoid havingt two samples at the same point
%Note2: Edges are generated using the Delanauy triangulation produced by
%       triangle.c
%Note3: Origin is taken as the point of maximum density
%
%
%5/19/03 - Leo Grady

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
% Date - $Id: pdf2graph.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization 
[X Y]=size(pdfImg);

%Find origin
[dummy,index]=max(pdfImg(:));
[origin(2),origin(1)]=ind2sub([X Y],index);

%Generate pdfVector
pdfVector=cumsum([0;pdfImg(:)]);

%Generate samples
for s=1:N
    samples(s)=binarysearch(pdfVector,rand(1,1));
end

%Convert sample indices to points
[pointsY,pointsX] = ind2sub([X Y],samples);
pointsY=max(pointsY)-pointsY+1; %Invert due to Matlab image inversion
points=[pointsX',pointsY'];
points=points+rand(N,2)-.5; %Dither points out of pixellated cells

%Center points at origin
points(:,1)=points(:,1)-origin(1); 
points(:,2)=points(:,2)-origin(2);

%Generate edges
if nargin < 3
    %Triangulate
    [edges, faces]=triangulatepoints(points);
else
    %K nearest neighbors
    edges=knnedges(points,K);
end
