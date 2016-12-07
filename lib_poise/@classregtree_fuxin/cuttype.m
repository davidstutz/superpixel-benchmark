function c=cuttype(t,j)
%CUTTYPE Type of cut used at branches in decision tree.
%   C=CUTTYPE(T) returns an N-element cell array C indicating the type of
%   cut at each node in the tree T, where N is the number of nodes in the
%   tree.  For each node K, C{K} is 'continuous' if the cut is defined in
%   the form Z<V for a variable Z and cutpoint V, 'categorical' if the cut
%   is defined by whether Z takes a value in a set of categories, or '' if
%   K is not a branch node.  The CUTVAR method returns the cutpoints for
%   'continuous' cuts, and the CUTCATEGORIES method returns the set of
%   categories for 'categorical' cuts.
%
%   V=CUTTYPE(T,J) takes an array J of node numbers and returns the cut
%   types for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES, CLASSREGTREE/CUTVAR.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.2 $  $Date: 2007/02/15 21:48:06 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:parent:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

% Get variable defining each cut
if nargin<2
    v = t.var;
else
    v = t.var(j,:);
end
v = v(:);

% Create array of cut types
c = repmat({''},numel(v),1);
c(v>0) = {'continuous'};
c(v<0) = {'categorical'};