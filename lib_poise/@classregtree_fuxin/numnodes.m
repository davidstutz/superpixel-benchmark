function n=numnodes(t)
%NUMNODES Number of nodes in tree.
%   N=NUMNODES(T) returns the number of nodes N in the tree T.
%
%   See also CLASSREGTREE.

%   Copyright 2006 The MathWorks, Inc. 
%   $Revision: 1.1.6.1 $  $Date: 2006/11/11 22:56:21 $

n = numel(t.node);
