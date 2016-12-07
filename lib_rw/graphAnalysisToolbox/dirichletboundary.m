function newVals=dirichletboundary(L,index,vals)
%function newVals=dirichletboundary(L,index,vals) finds the values on
%non-boundary points for a graph represented by L with "boundary" given by
%vals at nodes indicated by index by solving the dirichlet problem (i.e.,
%newVals represents the harmonic function on the graph with the given
%boundary conditions)
%
%Inputs:    L - NxN Laplacian matrix
%           index - Px1 Vector specifying the "boundary" nodes
%           vals - KxP Matrix of K, P-dimensional values on the 
%                  "boundary" nodes
%
%Outputs:   newVals - Nx1 vector specifying the values on all nodes
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
% Date - $Id: dirichletboundary.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
N=length(L);
antiIndex=1:N;
antiIndex(index)=[];

%Find RHS
b=-L(antiIndex,index)*(vals);

%Solve system
x=L(antiIndex,antiIndex)\b;

%Build output
newVals=zeros(size(vals));
newVals(index,:)=vals;
newVals(antiIndex,:)=x;
