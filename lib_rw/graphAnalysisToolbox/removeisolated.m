function newPoints=removeisolated(points,edges)
%Function accepts a node and edge set and returns the node set with
%isolated nodes removed.
%
%Inputs:    points - An NxP node list
%           edges - An Mx2 edge list
%
%Outputs:   newPoints - A KxP node list where K <= P
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
% Date - $Id: removeisolated.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
N=size(points,1);
M=size(edges,1);

%Build adjacency matrix
W=adjacency(edges,ones(M,1),N);

%Find isolated nodes
connectedIndex=find(sum(W));

%Return updated node list
newPoints=points(connectedIndex,:);
