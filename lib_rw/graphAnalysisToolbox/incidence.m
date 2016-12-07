function A=incidence(edges,weights,N)
%Function A=incidence(edges,weights,N) computes the edge-node incidence 
%matrix of the graph specified by edges
%
%Inputs:    edges - A Mx2 list of M edges
%           weights - Optional Mx1 edge weights 
%               (defaults to all ones if not specified)
%           N - Optional number of nodes in the graph.  Used if
%               N > max(edges) (i.e., isolated nodes are present)
%
%Outputs:   A - The edge-node incidence matrix
%
%
%5/21/03 - Leo Grady

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
% Date - $Id: incidence.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
M=size(edges,1);

%Check for weights
if nargin < 2
    weights=ones(M,1);
end

%Check for isolated nodes
if nargin < 3
    N=max(max(edges));
end

%Build sparse incidence matrix
A=sparse([1:M,1:M],[edges(:,1),edges(:,2)],[weights,-weights],M,N);
