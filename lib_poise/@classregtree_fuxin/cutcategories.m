function C=cutcategories(t,j)
%CUTCATEGORIES Categories used for branches in decision tree.
%   C=CUTCATEGORIES(T) returns an N-by-2 cell array C of the categories
%   used at branches in the decision tree T, where N is the number of
%   nodes in the tree.  For each branch node K based on a categorical
%   predictor variable Z, the left child is chosen if Z is among the
%   categories listed in C{K,1}, and the right child is chosen if Z is
%   among those listed in C{K,2}.  Both columns of C are empty for branch
%   nodes based on continuous predictors and for non-branch (leaf) nodes.
%
%   C=CUTCATEGORIES(T,J) takes an array J of node numbers and returns the
%   categories for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/CUTVAR, CLASSREGTREE/CUTPOINT, CLASSREGTREE/CUTTYPE.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.6 $  $Date: 2009/01/08 18:59:32 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:cutcategories:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

% Get variable numbers
if nargin<2
    n = t.var;
    cut = t.cut;
else
    n = t.var(j,:);
    cut = t.cut(j);
end

% Set up array for output
C = repmat({[] []},numel(n),1);

% Fetch category list pairs for categorical branches
catvar = find(n<0);
if ~isempty(catvar)
    for i=1:length(catvar)
        C(catvar(i),:) = cut{catvar(i)};
    end
end