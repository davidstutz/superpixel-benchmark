function [v,n]=cutvar(t,j)
%CUTVAR Variables used for branches in decision tree.
%   V=CUTVAR(T) returns an N-element cell array V of the names of the
%   variables used for branching in each node of the tree T, where N
%   is the number of nodes in the tree.  These variables are sometimes
%   known as cut variables.  For non-branch (leaf) nodes, V contains
%   an empty string.
%
%   V=CUTVAR(T,J) takes an array J of node numbers and returns the cut
%   variables for the specified nodes.
%
%   [V,NUM]=CUTVAR(...) also returns the number NUM of each variable.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES, CLASSREGTREE/CHILDREN.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.2 $  $Date: 2007/02/15 21:48:07 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:parent:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

% Get variable number, taking care that negative values may appear to
% indicate categorical cuts
if nargin<2
    n = abs(t.var);
else
    n = abs(t.var(j,:));
end

% Get variable name
v = repmat({''},numel(n),1);
mask = n>0;
v(mask) = t.names(n(mask));