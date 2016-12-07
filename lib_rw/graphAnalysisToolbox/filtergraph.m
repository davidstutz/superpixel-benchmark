function outVals=filtergraph(W,vals,iterations,algorithm,normFlag)
%function outVals=filtergraph(W,vals,iterations,algorithm,normFlag)
%implements a low-pass filter on the coordinates using either a mean filter
%or the filtering algorithm presented in Taubin's paper
%
%Inputs:    W - NxN (weighted) adjacency matrix
%           vals - NxP set of values to filter
%           iterations - Number of iterations to apply
%           algorithm - Optional argument specifying the type of filter
%                   [] - Mean filter (Default)
%                   [kpb lambda] - The kpb and lambda parameters from
%                   Taubin's paper (kpb = .1, lambda = .3 are typical
%                   values)
%           normFlag - Optional flag that specifies normalization of the
%                   output to the range of the input (columnwise)
%                   0 - No normalization (Default)
%                   1 - Normalize
%
%Outputs:   outVals - NxP set of filtered values
%
%
%References:
%@InProceedings{taubin1995:signal,
%  author =	      {Taubin, G.},
%  title =	      {A signal processing approach to fair surface design},
%  booktitle =	  {Computer Graphics Proceedings. SIGGRAPH 95},
%  pages =	      {351--358},
%  year =	      1995,
%  editor =	      {Cook, R.},
%  address =	  {Los Angeles, CA},
%  month =	      {August},
%  organization = {ACM},
%  publisher =	  {ACM} }
%
%
%5/13/03 - Leo Grady

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
% Date - $Id: filtergraph.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Process arguments
if nargin < 4
    algorithm=[];
    normFlag=0;
elseif nargin < 5
    normFlag=0;
end

%Preliminaries
[N P]=size(vals);
minVal=min(vals,[],1);
maxVal=max(vals,[],1);

%Choose filtering method and perform filtering
if isempty(algorithm) %Mean filter
    Dinv=diag(sparse(1./(1+sum(W))));
    outVals=vals;
    avgMat=Dinv*(W+speye(N));
    for k=1:iterations    
        outVals=avgMat*outVals;
    end
elseif length(algorithm) == 2     
    %Build relevant matrices
    Dinv=diag(sparse(1./sum(W)));
    L=speye(N)-Dinv*W;
    L2=L*L;
    
    %Initialize lambda and mu
    kpb=algorithm(1);
    lambda=algorithm(2);
    mu=1/(kpb-1/lambda);
    coeffVector=[lambda,mu];
    
    %Iterate filter to specified order
    outVals=vals;
    for k=1:iterations
        coeff=coeffVector(1+mod(k,2));
        outVals=outVals-coeff*L2*outVals;
    end
end
    
if normFlag
    outVals=normalize(outVals,vals);
end
