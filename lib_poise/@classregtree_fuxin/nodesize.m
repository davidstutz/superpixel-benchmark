function n=nodesize(t,j)
%NODESIZE Node size.
%   S=NODESIZE(T) returns an N-element vector S of the sizes of the
%   nodes in the tree T, where N is the number of nodes.  The size of a
%   node is defined as the number of observations from the data used to
%   create the tree that satisfy the conditions for the node.
%
%   S=NODESIZE(T,J) takes an array J of node numbers and returns the 
%   sizes for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES.

%   Copyright 2006 The MathWorks, Inc. 
%   $Revision: 1.1.6.1 $  $Date: 2006/11/11 22:56:20 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:nodesize:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

if nargin<2
    n = t.nodesize;
else
    n = t.nodesize(j,:);
end
