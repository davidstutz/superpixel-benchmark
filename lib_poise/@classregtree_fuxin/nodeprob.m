function p=nodeprob(t,j)
%NODEPROB Node probability.
%   P=NODEPROB(T) returns an N-element vector P of the probabilities of the
%   nodes in the tree T, where N is the number of nodes.  The probability
%   of a node is computed as the proportion of observations from the
%   original data that satisfy the conditions for the node.  For a
%   classification tree, this proportion is adjusted for any prior
%   probabilities assigned to each class.
%
%   P=NODEPROB(T,J) takes an array J of node numbers and returns the 
%   probabilities for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES, CLASSREGTREE/NODESIZE.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.8.1 $  $Date: 2007/02/02 23:22:01 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:nodeprob:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

if nargin<2
    p = t.nodeprob;
else
    p = t.nodeprob(j,:);
end
