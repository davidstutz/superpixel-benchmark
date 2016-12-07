function v = catsplit(t,j)
%CATSPLIT Categorical splits used for branches in a decision tree.
%   V=CATSPLIT(T) returns an Nx2 cell array V. Each row in V gives
%   left and right values for a categorical split. For each branch 
%   node J based on a categorical predictor variable Z, the left child 
%   is chosen if Z is in V(J,1) and the right child is chosen if Z is 
%   in V(J,2). The splits are in the same order as nodes of the tree.
%   Nodes for these splits can be found by running CUTTYPE and selecting
%   'categorical' cuts from top to bottom.
%
%   V=CATSPLIT(T,J) takes an array J of rows and returns the
%   splits for the specified rows.

%   Copyright 2008 The MathWorks, Inc. 
%   $Revision: 1.1.6.3 $  $Date: 2009/10/10 20:11:27 $

% If this is an old tree produced before 9a, it must have a separate array
% of categorical splits.
if ~iscell(t.cut)
    v = t.catsplit(j);
    return;
end

% Form a list of categorical cuts
ncut = length(t.cut);
v = cell(ncut,2);
nexttolast = 1;
for i=1:ncut
    if ~isnumeric(t.cut{i})
        v(nexttolast,1) = t.cut{i}(1);
        v(nexttolast,2) = t.cut{i}(2);
        nexttolast = nexttolast + 1;
    end
end
v(nexttolast:end,:) = [];

% Apply indices
if nargin<2
    j = (1:size(v,1))';
end
v = v(j,:);

end