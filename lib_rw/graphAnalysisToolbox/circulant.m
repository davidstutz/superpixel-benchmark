function W=circulant(vec)
%Function W=circulant(vec) inputs a vector and generates a circulant 
%matrix where each row is the nonzero pattern of vec shifted by one 
%position
%
%Inputs:    vec - A 1xN vector to be shifted
%
%Outputs:   W - An NxN circulant matrix consisted on vec shifted
%               progressively one place on each row
%
%
%Note:  Function accomplishes something similar to the function Toeplitz,
%       but that function is not intended to deal with large, sparse input 
%       vectors
%Note2: Primarily used to generate adjacency matrices for sparse,
%       shift-invariant graphs
%
%
%5/23/03 - Leo Grady

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
% Date - $Id: circulant.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Determine legnth of vec
N=length(vec);

%Build index
index=find(vec);
diagFact=N+1;
tmp=(diagFact).*(0:(N-1));
masterIndex=index'*ones(1,N) + ones(length(index),1)*tmp;
masterIndex(find(masterIndex>(N^2)))=masterIndex(1);

%Build A
[i,j] = ind2sub([N N],masterIndex(:));
lowerIndex=find(j<=i);
W=sparse(i(lowerIndex),j(lowerIndex),1,N,N);
W=spones(W+W');

%If vec is not sparse, make full again
if ~issparse(vec)
   W=full(W);
end
