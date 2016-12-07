function ok=validatenodes(t,j)
%VALIDATENODES Validate array of node numbers
%   OK=VALIDATENODES(T,J) returns true if J is a valid array of node
%   numbers or logical array for indexing into an object having a leading
%   dimension with one element per node in the array T.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.2 $  $Date: 2007/02/15 21:48:15 $

numnodes = numel(t.node);
if islogical(j)
    ok = (numel(j) <= numnodes);
else
    ok = ismember(j,1:numnodes);
    ok = all(ok(:));
end
