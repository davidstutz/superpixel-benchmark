function v=cutpoint(t,j)
%CUTPOINT Cutpoints used for branches in decision tree.
%   V=CUTPOINT(T) returns an N-element vector V of the values used as
%   cutpoints in the decision tree T.  For each branch node J based on a
%   continuous predictor variable Z, the left child is chosen if Z<V(J) and
%   the right child is chosen if Z>=V(J).  V is NaN for branch nodes based
%   on categorical predictors and for non-branch (leaf) nodes.
%
%   V=CUTPOINT(T,J) takes an array J of node numbers and returns the
%   cutpoints for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/CUTVAR, CLASSREGTREE/CUTCATEGORIES, CLASSREGTREE/CUTTYPE.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.4 $  $Date: 2008/10/31 07:40:39 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:parent:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

% Get variable numbers and cut points
if nargin<2
    j = (1:length(t.var))';
end
n = t.var(j,:);
v = zeros(length(n),1);
for i=1:length(n)
    if isnumeric(t.cut{j(i)})
        v(i) = t.cut{j(i)};
    end
end

% Remove invalid values
v(n<=0) = NaN;