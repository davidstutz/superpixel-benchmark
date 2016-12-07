function result=binarysearch(vec,searchVal)
%function result=binarysearch(vec,searchVal) performs a binary search on a
%sorted vector and returns the interval that searchVal falls into in vec.
%i.e., If vec is Nx1, than result is a number in the interval [1,(N-1)]
%
%Inputs:    vec - A sorted vector of intervals
%           searchVal - A number in the interval [vec(1),vec(N)]
%
%Outputs:   result - An integer indexing the appropriate interval
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
% Date - $Id: binarysearch.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Error checking
N=length(vec);
if (searchVal < vec(1)) | (searchVal > vec(N))
    disp('Value outside range.  Returning....')
    result=-1;
    return
end

%Initialization
endFlag=1;
oldMin=1;
oldMax=N;
currVal=floor(N/2);

%Main search loop
count=0;
while endFlag
    count=count+1;
    if count==500
        disp('No convergence...')
        endFlag=0;
        result=0;
    end
    if searchVal > vec(currVal)
        %Greater
        if oldMin == currVal
            result=currVal;
            endFlag=0;
        else
            oldMin=currVal;
            currVal=currVal+floor((oldMax-currVal)/2);
        end
    else
        %Less
        if oldMax == currVal
            result=currVal;
            endFlag=0;
        else      
            oldMax=currVal;
            currVal=oldMin+floor((currVal-oldMin)/2);
        end
    end
end
