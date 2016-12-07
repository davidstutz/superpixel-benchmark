function p=parent(t,j)
%PARENT Parent node.
%   P=PARENT(T) returns an N-element vector P containing the number of the
%   parent node for each node in the tree T, where N is the number of nodes
%   in the tree.  The parent of the top node is 0.
%
%   P=PARENT(T,J) takes an array J of node numbers and returns the parent
%   node numbers for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES, CLASSREGTREE/CHILDREN.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.2 $  $Date: 2007/02/15 21:48:10 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:parent:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

if nargin<2
    p = t.parent;
else
    p = t.parent(j,:);
end
