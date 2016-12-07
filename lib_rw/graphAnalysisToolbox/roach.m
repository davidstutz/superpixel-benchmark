function [W,points]=roach(totalLength)
%Function [W,points]=roach(totalLength) computes the "roach" graph 
%
%Inputs:    totalLength - Length of the "body" plus "antennae"
%
%Outputs:   W - Adjacency matrix
%           points - A Nx2 point list
%
%
%5/24/03 - Leo Grady

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
% Date - $Id: roach.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
bodyLength=floor(totalLength/2);
antLength=ceil(totalLength/2);
head=bodyLength-1;

%Generate roach "body"
[points edges]=lattice(2,bodyLength);

%Add antennae
points=[points;[(head+1):(head+antLength);ones(1,antLength)]'];
points=[points;[(head+1):(head+antLength);zeros(1,antLength)]'];

%Add antennae edges
edges=[edges;[(bodyLength*2+2):(bodyLength*2+antLength);
        (bodyLength*2+1):(bodyLength*2-1+antLength)]'];
edges=[edges;[(bodyLength*2+antLength+1):(bodyLength*2-1+2*antLength);
        (bodyLength*2+antLength+2):(bodyLength*2+antLength*2)]'];

%Connect antennae to body
edges=[edges;[2*bodyLength,2*bodyLength+1; ...
            2*bodyLength+1+antLength,2*bodyLength-1]];

%Compute laplacian
W=adjacency(edges);
