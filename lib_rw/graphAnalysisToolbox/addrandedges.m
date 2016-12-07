function newEdges=addrandedges(numbAdd,edges,N)
%Function newEdges=addrandedges(numbAdd,edges,N) adds numbAdd random 
%edges to the current edge set.  Generally used to "small-worldify" a 
%graph by adding some long range connections (i.e., drastically decrease 
%the graph diameter)
%
%Inputs:    numbAdd - Number of additional edges to add
%           edges - An Mx2 vector indicating the edges of the graph
%           N - Optional number of nodes in the graph, if different than
%               N = max(max(edges)) (i.e., there are isolated nodes)
%
%Outputs:   newEdges - An (M + numbAdd)x2 vector of edges for the new 
%               graph
%
%
%Note: If duplicate edges are generated randomly, the duplicates are
%   ignored - Therefore the actual number of edges added may be less than
%   numbAdd (e.g., if the input graph were fully connected)
%
%
%5/22/03 - Leo Grady

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
% Date - $Id: addrandedges.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Determine number of points
if nargin < 3
    N=max(max(edges));
end

%Randomly generate new edges
tmpEdges=round((N-1)*rand(numbAdd,2)+1);

%Remove duplicates
W=adjacency([edges;tmpEdges],ones(length(edges)+numbAdd,1),N);
newEdges=adjtoedges(W);
