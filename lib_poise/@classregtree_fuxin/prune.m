function Tree=prune(Tree,varargin)
%PRUNE Produce a sequence of subtrees by pruning.
%   T2 = PRUNE(T1,'level',LEVEL) takes a decision tree T1 and a pruning
%   level LEVEL, and returns the decision tree T2 pruned to that level.
%   The value LEVEL=0 means no pruning.  Trees are pruned based on an
%   optimal pruning scheme that first prunes branches giving less
%   improvement in error cost.
%
%   T2 = PRUNE(T1,'nodes',NODES) prunes the nodes listed in the NODES
%   vector from the tree.  Any T1 branch nodes listed in NODES become
%   leaf nodes in T2, unless their parent nodes are also pruned.  The
%   VIEW method can display the node numbers for any node you select.
%
%   T2 = PRUNE(T1,'alpha',ALPHA) prunes up to the specified value of ALPHA.
%   Splits with values of ALPHA less or equal to this one are removed from
%   the tree.
% 
%   Only one of the three arguments, LEVEL, NODES and ALPHA, can be given
%   to the tree at a time.
%
%   T2 = PRUNE(T1,'criterion','error') or 
%   T2 = PRUNE(T1,'criterion','impurity') 
%   prunes nodes using resubstitution error (default) or impurity for the
%   pruning criterion. If 'error' option is chosen, the cost of each node
%   is the classification error (or mean squared error for regression) for
%   this node multiplied by the probability for this node. For
%   classification trees grown using impurity, the resubstitution error
%   above is replaced by impurity if 'impurity' is chosen.
%
%   T2 = PRUNE(T1) returns the decision tree T2 that is the full,
%   unpruned T1, but with optimal pruning information added.  This is
%   useful only if you created T1 by pruning another tree, or by using
%   the CLASSREGTREE function with pruning set 'off'.  If you plan to prune
%   a tree multiple times along the optimal pruning sequence, it is more
%   efficient to create the optimal pruning sequence first.
%
%   Pruning is the process of reducing a tree by turning some branch
%   nodes into leaf nodes, and removing the leaf nodes under the
%   original branch.
%
%   Example:  Display full tree for Fisher's iris data, as well as
%   the next largest tree from the optimal pruning sequence.
%      load fisheriris;
%      varnames = {'SL' 'SW' 'PL' 'PW'};
%      t1 = classregtree(meas,species,'minparent',5,'names',varnames);
%      view(t1);
%      t2 = prune(t1,'level',1);
%      view(t2);
%
%   See also CLASSREGTREE, CLASSREGTREE/TEST, CLASSREGTREE/VIEW.

%   Copyright 2006-2009 The MathWorks, Inc. 
%   $Revision: 1.1.6.5 $  $Date: 2009/05/07 18:32:50 $

% Process inputs
nodes = [];
alpha = [];
crit = 'error';
if nargin==2
    level = varargin{1};
else
    okargs =   {'level' 'nodes' 'alpha' 'criterion'};
    defaults = {[]      []      []      'error'};
    [eid,emsg,level,nodes,alpha,crit] = ...
        getargs(okargs,defaults,varargin{:});
    if ~isempty(emsg)
        error(sprintf('stats:treeprune:%s',eid),emsg);
    end
    pruneflags = ~isempty(level) + ~isempty(nodes) + ~isempty(alpha);
    if pruneflags>1
        error('stats:treeprune:ParameterConflict',...
            'Can use only one of ''level'', ''nodes'' and ''alpha'' parameters.');
    end
end

% Error checking
if ~isempty(level)
   if numel(level)~=1 || ~isnumeric(level) || level<0 || level~=round(level)
      error('stats:treeprune:BadLevel',...
            'Value of ''level'' parameter must be a non-negative integer.')
   end
end
if ~isempty(nodes)
   nodes = nodes(:);
   if ~isnumeric(nodes) || any(nodes<1) || any(nodes~=round(nodes))
      error('stats:treeprune:BadNodes',...
            'Value of ''nodes'' parameter must contain node numbers.');
   end
end
if ~isempty(alpha)
    if ~isnumeric(alpha) || numel(alpha)~=1 || alpha<0 || alpha>=1
      error('stats:treeprune:BadAlpha',...
            'Value of ''alpha'' parameter must be a scalar between 0 and 1.');
    end
end
   
% Now do the pruning
if isempty(strmatch(lower(crit),Tree.prunecriterion))
   Tree = getpruneinfo(Tree,crit);           % may need optimal prune sequence
end
pruned = false;
if ~isempty(alpha)
    level = find(Tree.alpha<=alpha,1,'last') - 1;
end
if ~isempty(level)
   Tree = subtree(Tree,level);               % remove stuff using optimal sequence
   pruned = true;
elseif ~isempty(nodes)
   Tree = prunenodes(Tree,nodes);            % remove children of specified nodes
   pruned = true;
end
if ~isempty(Tree.prunelist) && pruned
   Tree.prunelist = [];
   Tree = getpruneinfo(Tree,crit);           % recompute prune info from scratch
end

% ------------------------------------------------------------------
function Tree=getpruneinfo(Tree,crit)
%GETPRUNEINFO Get optimal pruning information and store into decision tree.

% Start from the smallest tree that minimizes R(alpha,T) for alpha=0
N = length(Tree.node);
parent     = Tree.parent;
children   = Tree.children;

isleaf = Tree.var(:)==0;
nleaves = sum(isleaf);
adjfactor = 1 + 100*eps;

% Work up from the bottom of the tree to compute, for each branch node,
% the number of leaves under it and sum of their costs
treatasleaf = isleaf';
nodecost = risk(Tree,1:N,'criterion',crit);
costsum = nodecost;
nodecount = double(isleaf);
while(true)
   % Find ''twigs'' which I define as branches with two leaf children
   branches = find(~treatasleaf);
   twig = branches(sum(treatasleaf(children(branches,:)),2) == 2);
   if isempty(twig), break; end;    % worked our way up to the root node

   % Add the costs and sizes of the two children, give to twig
   kids = children(twig,:);
   costsum(twig)   = sum(costsum(kids'),1)';
   nodecount(twig) = sum(nodecount(kids'),1)';
   treatasleaf(twig) = 1;
end

% Now start pruning to generate a sequence of smaller trees
whenpruned = zeros(N,1);
branches = find(~isleaf);
prunestep = 0;
allalpha = zeros(N,1);
ntermnodes = zeros(N,1);
ntermnodes(1) = nleaves;
while(~isempty(branches))
   prunestep = prunestep + 1;
   
   % Compute complexity parameter -- best tree minimizes cost+alpha*treesize
   alpha = max(0, nodecost(branches) - costsum(branches)) ./ ...
           max(eps,nodecount(branches) - 1);
   bestalpha = min(alpha);
   toprune = branches(alpha <= bestalpha*adjfactor);

   % Mark nodes below here as no longer on tree
   wasleaf = isleaf;
   kids = toprune;
   while ~isempty(kids)
      kids = children(kids,:);
      kids = kids(kids>0);
      kids(isleaf(kids)) = [];
      isleaf(kids) = 1;
   end
   newleaves = toprune(~isleaf(toprune));
   isleaf(toprune) = 1;

   % Remember when branch was pruned, also perhaps leaves under it
   whenpruned(isleaf~=wasleaf & whenpruned==0) = prunestep;
   whenpruned(toprune) = prunestep;   % this branch was pruned

   % Update costs and node counts
   for j=1:length(newleaves)          % loop over branches that are now leaves
      node = newleaves(j);
      diffcost  = nodecost(node) - costsum(node);
      diffcount = nodecount(node) - 1;
      while(node~=0)                  % work from leaf up to root
         nodecount(node) = nodecount(node) - diffcount;
         costsum(node)   = costsum(node) + diffcost;
         node = parent(node);         % move to parent node
      end
   end

   allalpha(prunestep+1) = bestalpha;
   ntermnodes(prunestep+1) = nodecount(1);
   
   % Get list of branches on newly pruned tree
   branches = find(~isleaf);
end

Tree.prunelist  = whenpruned;
Tree.alpha      = allalpha(1:prunestep+1);
Tree.ntermnodes = ntermnodes(1:prunestep+1);
if strmatch(lower(crit),'error')==1
    Tree.prunecriterion = 'error';
elseif strmatch(lower(crit),'impurity')==1
    Tree.prunecriterion = 'impurity';
end

% ------------------------------------------------------------
function Tree = subtree(Tree,p)
%SUBTREE Get subtree from tree indexed by pruning point.

whenpruned = Tree.prunelist;
v = find(whenpruned>0 & whenpruned<=p);
if ~isempty(v)
   Tree = prunenodes(Tree,v);
end

% ------------------------------------------------------------
function Tree = prunenodes(Tree,branches)
%PRUNENODES Prune selected branch nodes from tree.

N = length(Tree.node);

% Find children of these branches and remove them
parents = branches;
tokeep = true(N,1);
kids = [];
while(true)
   newkids = Tree.children(parents,:);
   newkids = newkids(:);
   newkids = newkids(newkids>0 & ~ismember(newkids,kids));
   if isempty(newkids), break; end
   kids = [kids; newkids];
   tokeep(newkids) = 0;
   parents = newkids;
end

% Convert branches to leaves by removing split rule and children
Tree.var(branches) = 0;
Tree.cut(branches) = {0};
Tree.children(branches,:) = 0;

% Get new node numbers from old node numbers
ntokeep = sum(tokeep);
nodenums = zeros(N,1);
nodenums(tokeep) = (1:ntokeep)';

% Reduce tree, update node numbers, update child/parent numbers
Tree.parent    = Tree.parent(tokeep);
Tree.class     = Tree.class(tokeep);
Tree.var       = Tree.var(tokeep);
Tree.cut       = Tree.cut(tokeep);
Tree.children  = Tree.children(tokeep,:);
Tree.nodeprob  = Tree.nodeprob(tokeep);
Tree.nodeerr   = Tree.nodeerr(tokeep);
Tree.nodesize  = Tree.nodesize(tokeep);
Tree.node      = (1:ntokeep)';
Tree.reg_ws    = Tree.reg_ws(tokeep);
Tree.reg_pvars = Tree.reg_pvars(tokeep);
mask = Tree.parent>0;
Tree.parent(mask) = nodenums(Tree.parent(mask));
mask = Tree.children>0;
Tree.children(mask) = nodenums(Tree.children(mask));
if isequal(Tree.method,'classification')
   Tree.classprob = Tree.classprob(tokeep,:);
   Tree.classcount= Tree.classcount(tokeep,:);
   if ~isempty(Tree.impurity)
       Tree.impurity = Tree.impurity(tokeep,:);
   end
end
