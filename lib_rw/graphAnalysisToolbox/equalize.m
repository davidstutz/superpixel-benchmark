function out=equalize(in,levels)
%Function out=equalize(in,levels) performs histogram equalization on 
%the vector passed by 'in' 
%
%Inputs:    in - An Nx1 vector of N values
%           levels - The granularity of the historgram.  
%               If not defined, defaults to 256
%
%Outputs:   out - A vector of N, equalized, values with the same range
%               as the input values
%
%Note: Histogram is equalized across each dimension independently
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
% Date - $Id: equalize.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Read inputs
if nargin < 2
    levels=256;
end

%Parameters
[N P]=size(in);

%Error check for single input
if N == 1
    out=in;
    return
end

%Normalize inputs to the interval [0,1]
minOrig=min(in);
maxOrig=max(in);
normVals=normalize(in);

%Find normalized histogram
inHist=hist(in,levels)./N;

%Cumulative probability distribution
inCDF=cumsum(inHist)';

%Transform input to equalized input
tmp=1+round(normVals.*(levels-1));
out=inCDF(tmp);

%Normalize equalized output
out=normalize(out,[minOrig;maxOrig]);
