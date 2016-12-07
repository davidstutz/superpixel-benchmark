function imp = varimportance(Tree)
%VARIMPORTANCE Compute embedded estimates of input feature importance.
%
% IMP = VARIMPORTANCE(T) computes estimates of input feature importance for
% tree T by summing changes in the risk due to splits on every feature. The
% returned vector IMP has one element for each input variable in the data
% used to train this tree. At each node, the risk is estimated as node
% impurity if impurity was used to split nodes and node error otherwise.
% This risk is weighted by the node probability. Variable importance
% associated with this split is computed as the difference between the risk
% for the parent node and the total risk for the two children.
%
% See also CLASSREGTREE/RISK.

%   Copyright 2008 The MathWorks, Inc. 
%   $Revision: 1.1.6.2 $

% Init
imp = zeros(1,Tree.npred);

% If impurity has been computed, use it to estimate variable
% importance. Otherwise use classification error.
if ~isempty(Tree.impurity)
    risk = Tree.nodeprob .* Tree.impurity;
else
    risk = Tree.nodeprob .* Tree.nodeerr;
end

% Sum risk changes over allsplits
for parent=1:length(Tree.node)
    kids = Tree.children(parent,:);
    if length(kids)==2 && kids(1)>0 && kids(2)>0
        [tf,ivar] = ismember(Tree.cutvar(parent),Tree.names);
        imp(ivar) = imp(ivar) + risk(parent) ...
            - risk(kids(1)) - risk(kids(2));
    end
end
imp = imp/length(Tree.node);
end