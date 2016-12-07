function [edges,faces]=triangulatepoints(points)
%function [edges, faces]=triangulatepoints(points) inputs a point set
%and outputs a list of edges corresponding to the Delaunay triangulation
%
%Inputs:    points - A NxP list of N vertex locations 
%
%Outputs:   edges - A Mx2 list of M edges indexing into points
%           faces - A Qx(P+1) list of R simplices
%
%
%Note: Function runs faster if MEXed version of triangle.c is used.  Change line 
%commenting as directed if the MEXed version of triangle.c is installed.
%
%
%5/15/03 - Leo Grady

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
% Date - $Id: triangulatepoints.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Find Delaunay simplices
[N P]=size(points);
if P == 2
%Uncomment if the MEX version of triangle.c is installed
%    triStruct.pl=points';
%    outStruct=triangle(triStruct,'Qe');
%    edges=outStruct.el';
%    faces=outStruct.tl';
%    return
    faces=delaunay(points(:,1),points(:,2)); %Comment out if triangle.c installed
elseif P == 3
    faces=delaunay3(points(:,1),points(:,2),points(:,3));
else
    faces=delaunayn(points);
end

%Create edge list
edges=[];
for k=1:P
    for s=(k+1):(P+1)
        edges=[edges;[faces(:,k),faces(:,s)]];
    end
end    

%Remove duplicate edges
W=adjacency(edges);
edges=adjtoedges(W);
