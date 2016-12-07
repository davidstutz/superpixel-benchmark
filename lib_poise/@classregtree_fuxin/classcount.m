function p=classcount(t,j)
%CLASSCOUNT Class counts for tree nodes.
%   P=CLASSCOUNT(T) returns an N-by-M array P of class counts for the
%   nodes in the tree classification T, where N is the number of nodes
%   and M is the number of classes.  For any node number K, the class
%   counts P(K,:) are counts of observations (from the data used in
%   fitting the tree) from each class satisfying the conditions for node K.  
%
%   P=CLASSCOUNT(T,J) takes an array J of node numbers and returns the 
%   class counts for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.3 $  $Date: 2007/02/15 21:48:01 $

if isequal(t.method,'regression')
    error('stats:classregtree:classcount:NotClassification',...
          'The CLASSCOUNT method is not available for regression trees.');
end
if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:classcount:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

if nargin<2
    p = t.classcount;
else
    p = t.classcount(j,:);
end
