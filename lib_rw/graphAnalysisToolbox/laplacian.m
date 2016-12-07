function L=laplacian(edges,weights,N)
%Function L=laplacian(edges,weights,N) computes the weighted 
%   Laplacian matrix of a point and edge set.  For an unweighted 
%   matrix, set weights equal to ones(1,M) where M is the number 
%   of edges in the graph.  
%
%Inputs:    edges - A Mx2 list of M edges indexing into points
%           weights - The weights used to determine matrix values.  
%               If not specified, uses vector of all ones
%           N - Optional number of nodes in the graph.  Used if
%               N > max(edges) (i.e., isolated nodes are present)
%
%Outputs:   L - Laplacian matrix
%
%
%6/5/03 - Leo Grady

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
% Date - $Id: laplacian.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%If weights are not specified, use unity weighting
if nargin == 1
    weights=ones(size(edges,1),1);
end

%If N is not specified, use maximum values of edges
if nargin < 3
    N=max(max(edges));
end

%Build sparse Laplacian matrix
W=adjacency(edges,weights,N);
L=diag(sum(W))-W;
