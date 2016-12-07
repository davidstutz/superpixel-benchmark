function x=isosolve(L,f,ground,methodFlag,normFlag)
%Function x=isosolve(L,f,ground,methodFlag,normFlag) 
%finds the solution for x, based on minimizing the Isoperimetric 
%constraints
%
%Inputs:    L - NxN (weighted) Laplacian matrix
%           f - The Nx1 vector corresponding to the notion of volume
%              defined on the graph (e.g., f=diag(L) or f=ones(N,1))
%           ground - Index to the ground point (index to a vertex)
%           methodFlag - Optional flag determining direct or iterative 
%               solution
%              '0' - Direct solution (Default)
%              '1' - Iterative with conjugate gradients
%           normFlag - Optional flag specifying normalization of the output
%              (Default 0).
%
%Outputs:   x - The Nx1 potential function on the nodes
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
% Date - $Id: isosolve.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
N=length(L);

%Read inputs
if nargin < 4
    methodFlag=0;
end

%Remove ground points
warning off %Suppress warning if near singular
index=[1:(ground-1),(ground+1):N];
f2=f(index);
L2=L(index,index);

%Solve system
if ~methodFlag
    x=L2\f2;
else
    x=pcg(L2,f2,[],N,diag(f2));
end

%Error catch if numerical instability occurs (due to ill-conditioned or 
%singular matrix)
minVal=min(min(x)); 
if minVal < 0
    x(find(x<0))=max(max(x))+1;
end

%Normalize, if desired
if (nargin > 4) & (normFlag)
    x=normalize(x);
end

%Re-insert ground point
x=[x(1:(ground-1));0;x((ground):(N-1))];

%Remove sparseness of output
x=full(x);
