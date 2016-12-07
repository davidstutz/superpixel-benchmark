function normVals=normalize(newVals,oldVals)
%function normVals=normalize(newVals,oldVals) normalizes the range of
%newVals to the range of oldVals such that every column is normalized
%independantly
%
%Inputs:    newVals - NxP matrix of new values to be normalized to the
%               range of oldVals
%           oldVals - Optional NxP matrix of original values (for 
%               arbitrary K).  Defaults to normalizing range to [0,1]
%
%Outputs:   normVals - NxP matrix of newVals normalized (columnwise) to 
%               the range of oldVals
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
% Date - $Id: normalize.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialize
[N P]=size(newVals);

%Supply optional argument, if required
if nargin == 1
    oldVals=[zeros(1,P);ones(1,P)];
end

%Find original minima/maxima
minVal=min(oldVals,[],1);
maxVal=max(oldVals,[],1);

%Find current minima/maxima
minNewVal=min(newVals,[],1);
maxNewVal=max(newVals,[],1);

%Perform normalization
warning off MATLAB:divideByZero %Error for divide by zero handled below
normVals=newVals-ones(N,1)*minNewVal;
normVals=normVals.*(ones(N,1)*(maxVal-minVal))./ ...
    (ones(N,1)*max(normVals,[],1));
normVals=normVals+ones(N,1)*minVal;
warning on MATLAB:divideByZero

%Error check for completely uniform inputs
uniformIndex=find(minNewVal==maxNewVal);
normVals(:,uniformIndex)=ones(N,1)*minNewVal(:,uniformIndex);
