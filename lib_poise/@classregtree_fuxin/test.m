function [cost,secost,ntnodes,bestlevel] = test(Tree,TorCorR,X,Y,varargin)
%TEST Compute error rate for tree.
%   COST = TEST(T,'resubstitution') computes the cost of the tree T
%   using a resubstitution method.  T is a decision tree as created by
%   the CLASSREGTREE constructor.  The cost of the tree is the sum over all
%   terminal nodes of the estimated probability of that node times the
%   node's cost.  If T is a classification tree, the cost of a node is
%   the sum of the misclassification costs of the observations in
%   that node.  If T is a regression tree, the cost of a node is the
%   average squared error over the observations in that node.  COST is
%   a vector of cost values for each subtree in the optimal pruning
%   sequence for T.  The resubstitution cost is based on the same
%   sample that was used to create the original tree, so it under-
%   estimates the likely cost of applying the tree to new data.
%
%   COST = TEST(T,'test',X,Y) uses the predictor matrix X and response Y as
%   a test sample, applies the decision tree T to that sample, and returns
%   a vector COST of cost values computed for the test sample.  X and Y
%   should not be the same as the learning sample, which is the sample that
%   was used to fit the tree T. By default, the method applies the prior
%   probabilities and the cost matrix that were used for the learning
%   sample. You can optionally supply new prior probabilities and a new
%   cost matrix. 
%
%   COST = TEST(T,'crossvalidate',X,Y) uses 10-fold cross-validation to
%   compute the cost vector. X and Y should be the learning sample, which
%   is the sample that was used to fit the tree T. If you used weights for
%   fitting the tree, you should supply the same weights here by using
%   'weights' parameter as described below. The function partitions the
%   sample into 10 subsamples, chosen randomly but with roughly equal size.
%   For classification trees the subsamples also have roughly the same
%   class proportions.  For each subsample, TEST fits a tree to the
%   remaining data and uses it to predict the subsample.  It pools the
%   information from all subsamples to compute the cost for the whole
%   sample.
%
%   [COST,SECOST,NTNODES,BESTLEVEL] = TEST(...) also returns the vector
%   SECOST containing the standard error of each COST value, the vector
%   NTNODES containing the number of terminal nodes for each subtree, and
%   the scalar BESTLEVEL containing the estimated best level of pruning.
%   BESTLEVEL=0 means no pruning (i.e. the full unpruned tree).  The best
%   level is the one that produces the smallest tree that is within one
%   standard error of the minimum-cost subtree.
%
%   [...] = TEST(...,'PARAM1',val1,'PARAM2',val2,...) specifies optional
%   parameter name/value pairs chosen from the following (not available for
%   the 'resubstitution' method):
%
%      'weights'    Vector of observation weights. By default the weight
%                   of every observation is set to 1. The length of this
%                   vector must be equal to the number of rows in X.
%      'nsamples'   The number of cross-validation samples (default 10)
%      'treesize'   Either 'se' (the default) to choose the smallest
%                   tree whose cost is within one standard error of the
%                   minimum cost, or 'min' to choose the minimal cost tree
%
%   The following two parameters are valid only for the 'test' method:
%
%      'cost'       Square matrix C, where C(i,j) is the cost of
%                   classifying a point into class j if its true class is i
%                   (default has C(i,j)=1 if i~=j, and C(i,j)=0 if i=j).
%                   Alternatively, this value can be a structure S having
%                   two fields:  S.group containing the group names as a
%                   categorical variable, character array, or cell array
%                   of strings; and S.cost containing the cost matrix C.
%      'priorprob'  Prior probabilities for each class, specified as a
%                   vector (one value for each distinct group name) or as
%                   a structure S with two fields:  S.group containing
%                   the group names as a categorical variable, character
%                   array, or cell array of strings; and S.prob
%                   containing a vector of corresponding probabilities.
%                   If both observation weights and class prior
%                   probabilities are supplied, the weights are
%                   renormalized to add up to the value of the prior
%                   probability in the respective class.
%
%   Example:  Find best tree for Fisher's iris data using cross-validation.
%             The solid line shows the estimated cost for each tree size,
%             the dashed line marks 1 standard error above the minimum,
%             and the square marks the smallest tree under the dashed line.
%      % Start with a large tree
%      load fisheriris;
%      varnames = {'SL' 'SW' 'PL' 'PW'};
%      t = classregtree(meas,species,'minparent',5,'names',varnames);
%
%      % Find the minimum-cost tree
%      [c,s,n,best] = test(t,'cross',meas,species);
%      tmin = prune(t,'level',best);
%
%      % Plot smallest tree within 1 std. error of minimum cost tree
%      [mincost,minloc] = min(c);
%      plot(n,c,'b-o', n(best+1),c(best+1),'bs',...
%           n,(mincost+s(minloc))*ones(size(n)),'k--');
%      xlabel('Tree size (number of terminal nodes)')
%      ylabel('Cost')
%
%   See also CLASSREGTREE, CLASSREGTREE/EVAL, CLASSREGTREE/VIEW, CLASSREGTREE/PRUNE.

%   Copyright 2006-2009 The MathWorks, Inc. 
%   $Revision: 1.1.6.9 $  $Date: 2009/05/07 18:32:52 $

if nargin<2, error('stats:treetest:TooFewInputs','Not enough arguments.'); end

if ~ischar(TorCorR) || ~(TorCorR(1)=='t' || TorCorR(1)=='c' || TorCorR(1)=='r')
   error('stats:treetest:InvalidOption',...
         'Second argument must be ''test'', ''crossvalidate'', or ''resubstitution''.');
end
if TorCorR(1)=='t' && nargin<4
   error('stats:treetest:TooFewInputs',...
         'Not enough arguments.  Need X and Y for the test sample.');
elseif TorCorR(1)=='c' && nargin<4
   error('stats:treetest:TooFewInputs',...
         'Not enough arguments.  Need X and Y from the learning sample.');
elseif TorCorR(1)=='r' && nargin>2
    % No param name/value args allowed.  Okay to give X,Y if we recognize
    % X as not a param name
    if nargin>4 || ischar(X)
         error('stats:treetest:TooManyInputs',...
           'Parameter name/value pairs not allowed with the ''resubstitution'' method.');
    end
end
if TorCorR(1)~='r'
   if ~ischar(Y) && numel(Y)~=length(Y)
      error('stats:treetest:BadData','Y must be a vector.');
   else
      if iscell(Y) || isnumeric(Y) || islogical(Y)
         n = length(Y);
      else
         n = size(Y,1);
      end
      if size(X,1)~=n
         error('stats:treetest:InputSizeMismatch',...
               'There must be one Y value for each row of X.');
      end
   end
end

okargs =   {'weights' 'nsamples' 'treesize' 'cost' 'priorprob'};
defaults = {       []         10       'se'     []          []};
[eid,emsg,W,ncv,treesize,cost,prior] = ...
    internal.stats.getargs(okargs,defaults,varargin{:});
if ~isempty(emsg)
    error(sprintf('stats:treetest:%s',eid),emsg);
end

if ~isnumeric(ncv) || numel(ncv)~=1 || ncv<2 || ncv~=round(ncv)
   error('stats:treetest:BadNSamples',...
         'Value of ''nsamples'' argument must be an integer 2 or larger.');
end
if ~ischar(treesize) || ~(treesize(1)=='s' || treesize(1)=='m')
   error('stats:treetest:BadTreeSize',...
         'Value of ''treesize'' argument must be ''se'' or ''min''.');
end
if TorCorR(1)~='t' && (~isempty(cost) || ~isempty(prior))
    error('stats:treetest:InvalidInput',...
         'You cannot supply cost and prior unless you use ''test'' mode.');
end

% Get complexity parameters for all pruned subtrees
if isempty(Tree.alpha)
   Tree = treeprune(Tree);
end

% Prepare data and weights
if nargin>=4
    n = size(X,1);
    if isempty(W)
        W = ones(n,1);
    end
    [X,Y,W,classname] = ...
        classregtree.preparedata(X,Y,W,isequal(Tree.method,'classification'));
    [tf,loc] = ismember(classname,Tree.classname);
    if any(~tf)        
        error('stats:treetest:BadYValue',...
            'At least one Y value (''%s'') is incompatible with the tree.',...
            classname{find(~tf,1)});
    end
    for c=1:length(classname)
        Y(Y==c) = loc(c);
    end
end

% Do proper type of testing (error estimation)
switch(TorCorR(1))
 case 't', [cost,secost] = testtree(Tree,X,Y,W,cost,prior);
 case 'c', [cost,secost] = cvtree(Tree,X,Y,W,ncv);
 case 'r', [cost,secost] = resubinfo(Tree); treesize = 'm';
end

cost = cost(:);
secost = secost(:);
if nargout>=3
   ntnodes = Tree.ntermnodes(:);
end
if nargout>=4
   bestlevel = selecttree(cost,secost,treesize(1)) - 1;
end

% ---------------------------------------------------------
function [resuberr,seresub] = resubinfo(Tree)
%RESUBINFO Compute error rates for tree using resubstitution error.

% Get complexity parameters for all pruned subtrees
nsub = 1+max(Tree.prunelist);

% Get error rate for each subtree in this sequence
resuberr = zeros(nsub,1);
for j=1:nsub;
   Tj = treeprune(Tree,'level',j-1);
   leaves = Tj.node(Tj.var==0);
   resuberr(j) = sum(Tj.risk(leaves));
end
seresub = zeros(size(resuberr));

% ---------------------------------------------------------------
function [testerr,seerr] = testtree(Tree,X,id,W,Cost,Prior)
%TESTTREE Compute error rates for tree using test sample.
%   The id variable is the class id for classification, or the y variable
%   for regression.

% Get pruning sequence and compute fitted values for the whole sequence
nsub = 1 + max(Tree.prunelist);
yfit = treeval(Tree,X,(0:nsub-1));

% Classification or regression?
doclass = isequal(Tree.method,'classification');

% Compute error statistics
N = size(X,1);
if doclass
   % Distribute weights across classes
   cnames = Tree.classname;
   nclasses = max(id);
   C = false(N,nclasses);
   C(sub2ind([N nclasses],(1:N)',id)) = 1;   
   WC = bsxfun(@times,C,W);
   Wj = sum(WC,1);
   
   % Compute SE for classification error using all observations at once
   % (sePerClass=false) or compute SE errors within each class and then add
   % these together (sePerClass=true)?
   if isempty(Prior)
       sePerClass = false;
   else
       sePerClass = true;
   end
   
   % Get prior and cost and renormalize weights
   [Prior,Cost,removerows] = classregtree.priorandcost(Prior,Cost,cnames,Wj,id);
   if any(removerows)
       yfit(removerows,:) = [];
       WC(removerows,:) = [];
       Wj = sum(WC,1);
   end
   idx = Wj>0;
   WC(:,idx) = bsxfun(@times,WC(:,idx),Prior(idx)./Wj(idx));

   % Get errors
   [testerr,seerr] = classerr(id,yfit,WC,Prior,Cost,nsub:-1:1,sePerClass);
else
   [testerr,seerr] = regerr(id,yfit,W);
end

% ---------------------------------------------------------------
function [cverr,secverr] = cvtree(Tree,X,id,W,ncv)
%CVTREE Compute error rates for tree using cross-validation.
%   [CVERR,SECVERR] = CVTREE(TREE,X,ID,NCV)

% Get geometric means of the alpha boundary points
alpha = Tree.alpha;
avgalpha = [sqrt(alpha(1:end-1) .* alpha(2:end)); Inf];

% Put all input parameters into one list
treeArgs = {};
if ~isempty(Tree.catcols)
    treeArgs = [treeArgs {'categorical' Tree.catcols}];
end
if ~isempty(Tree.method)
    treeArgs = [treeArgs {'method' Tree.method}];
end
if ~isempty(Tree.names)
    treeArgs = [treeArgs {'names' Tree.names}];
end
if isempty(Tree.prunelist)
    treeArgs = [treeArgs {'prune' 'off'}];
else
    treeArgs = [treeArgs {'prune' 'on'}];
end
if ~isempty(Tree.minparent)
    treeArgs = [treeArgs {'minparent' Tree.minparent}];
end
if ~isempty(Tree.minleaf)
    treeArgs = [treeArgs {'minleaf' Tree.minleaf}];
end
if ~isempty(Tree.nvartosample)
    treeArgs = [treeArgs {'nvartosample' Tree.nvartosample}];
end
if ~isempty(Tree.mergeleaves)
    treeArgs = [treeArgs {'mergeleaves' Tree.mergeleaves}];
end
if ~isempty(Tree.qetoler)
    treeArgs = [treeArgs {'qetoler' Tree.qetoler}];
end
if ~isempty(Tree.splitcriterion)
    treeArgs = [treeArgs {'splitcriterion' Tree.splitcriterion}];
end

% Loop over cross-validation samples
N = size(X,1);
ntrees = length(avgalpha);
cvid = 1 + mod((1:N),ncv);

doclass = isequal(Tree.method,'classification');
if doclass
   % Distribute weights across classes
   nclasses = max(id);
   C = false(N,nclasses);
   C(sub2ind([N nclasses],(1:N)',id)) = 1;   
   WC = bsxfun(@times,C,W);
   Wj = sum(WC,1);
   
   % Renormalize weights
   idx = Wj>0;
   WC(:,idx) = bsxfun(@times,WC(:,idx),Tree.prior(idx)./Wj(idx));
    
   % Use a random permutation with fixed category proportions
   idrand = id + rand(size(id));
   [~,idx] = sort(idrand);
   cvid = cvid(idx);
else   
   % Use a random permutation with fixed numbers per cross-validation sample
   cvid = cvid(randperm(N));
end

% Get predicted values using cross-validation samples
cvclass = zeros(N,ntrees);
for j=1:ncv
   % Use jth group as a test, train on the others
   testrows = (cvid == j);
   trainrows = ~testrows;

   % Keep prior and cost only for classes present if this fold
   extraArgs = {};
   if doclass
       idfold = unique(id(trainrows));
       prior = Tree.prior(idfold);
       cost = Tree.cost(idfold,idfold);
       extraArgs = {'priorprob' prior 'cost' cost};
   end
   
   % Get a sequence of pruned trees for the training set
   Tj = treefit(X(trainrows,:),id(trainrows),'weights',W(trainrows),...
                treeArgs{:},extraArgs{:});

   % Get classifications based on each subtree that we require
   treesneeded = findsubtree(Tj,avgalpha);
   cvclass(testrows,:) = treeval(Tj,X(testrows,:),treesneeded-1);
   if doclass
       % Assign correct class numbers if some classes are missing
       cvclass(testrows,:) = idfold(cvclass(testrows,:));
   end
end

% Compute classification error and its standard error.
if doclass
    [cverr,secverr] = classerr(id,cvclass,WC,Tree.prior,Tree.cost,1:ntrees,true);
else
    [cverr,secverr] = regerr(id,cvclass,W);
end

% ----------------------------
function k = findsubtree(Tree,alpha0)
%FINDSUBTREE Find subtree corresponding to specified complexity parameters.

adjfactor = 1 + 100*eps;
alpha = Tree.alpha;
k = zeros(size(alpha0));
for j=1:length(alpha0);
   k(j) = sum(alpha <= alpha0(j)*adjfactor);
end

% -----------------------------
function bestj = selecttree(allalpha,sealpha,treesize)
%SELECTTREE Select the best tree from error rates using some criterion.

% Find the smallest tree that gives roughly the minimum error
[minerr,minloc] = min(allalpha);
if isequal(treesize(1),'m')
   cutoff = minerr * (1 + 100*eps);
else
   cutoff = minerr + sealpha(minloc);
end
j = find(allalpha <= cutoff);
bestj = j(end);

% -------------------------------
%
% Compute classification error and its standard error.
%
% If sePerClass is false, it is assumed that test data are sampled with
% unknown class proportions. Variance of the classification error
% estimate is obtained essentially as e*(1-e)/N in the non-weighted case
% and as e*(1-e)*sum(w.^2) in the weighted case (with weights normalized
% to unit sum).
%
% If sePerClass is true, it is assumed that the number of observations in
% each class is fixed by the user. Variance of the classification error
% estimate in this case is estimated as sum(prior.^2*se.^2), where se.^2 is
% the variance per class given by e*(1-e)*sum(w.^2) over observations
% within this class.
%
function [err,seerr] = classerr(id,yfit,WC,prior,cost,itrees,sePerClass)
% Init
nclasses = max(id);
ntrees = length(itrees);
err = zeros(ntrees,1);
seerr = zeros(ntrees,1);

% Squared weights per class
if sePerClass
    Wsq = sum(WC.^2,1);
else
    Wsq = sum(sum(WC.^2));
end

% Loop over trees
for k=itrees
    loss = zeros(1,nclasses); % loss per true class
    sqloss = zeros(1,nclasses); % squared loss per true class
    for c=1:nclasses
        ic = yfit(:,k)==c; % observations classified into this class
        ccol = cost(:,c)'; % column of the cost matrix for classifying into this class
        loss = loss + sum(bsxfun(@times,WC(ic,:),ccol),1);
        sqloss = sqloss + sum(bsxfun(@times,WC(ic,:),ccol.^2),1);
    end
    err(k) = sum(loss);
    if sePerClass
        idx = prior>0;
        loss(idx) = loss(idx)./prior(idx);
        sqloss(idx) = sqloss(idx)./prior(idx);
        seerr(k) = sqrt(sum(Wsq.*(sqloss-loss.^2)));
    else
        seerr(k) = sqrt(Wsq*(sum(sqloss) - err(k)^2));
    end
end

% -------------------------------
% Compute regression error and its standard error.
function [err,seerr] = regerr(id,yfit,W)
N = length(W);
Wtot = sum(W);
E = (yfit - repmat(id,1,size(yfit,2))).^2;
err = sum(bsxfun(@times,E,W),1) / Wtot;
s2 = sum(bsxfun(@times,(E - repmat(err,size(E,1),1)).^2,W),1) / Wtot;
seerr = sqrt(s2/N);
