function [neighGroup,edges]=knn(points,K)
%function [neighGroup,edges]=knn(points,K) finds the k nearest neighbors 
%of a point set
%
%Inputs:    points - An NxP point set of N points with dimension P to find
%               the nearest neighbors for
%           K - The number of nearest neighbors to return for each point
%
%Outputs:   neighGroup - An NxK set listing indices into points of the k nearest 
%               neighbors
%           edges - Optional output.  Builds an Mx2 list of edges from the
%               neighbors
%
%
%5/19/03 - Leo Grady
%Note: Very non-optimal.  Uses Euclidean distance.

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
% Date - $Id: knn.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
[N P]=size(points);

%Main loop
for s=1:N
    currIndex=1:N;
    currIndex(s)=[];
    distances=0;
    distances=sum((ones(N-1,1)*points(s,:)-points(currIndex,:)).^2,2);
    distances=[distances(1:(s-1));Inf;distances(s:(N-1))];
    distances(:,2)=[1:N]';
    order=sortrows(distances,1);
    neighGroup(s,:)=order(1:K,2)';
end

%Build edge list, if requested
if nargout > 1
    %Convert output of knn to an edge set
    edges=[];
    for s=1:K
        edges=[edges;[[1:N]',neighGroup(:,s)]];
    end

    %Remove duplicate edges
    W=adjacency(edges);
    edges=adjtoedges(W);
end
