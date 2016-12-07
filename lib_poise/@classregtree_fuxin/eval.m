function [idname,nodes,id]=eval(Tree,X,subtrees)
%EVAL Compute fitted value for decision tree applied to data.
%   YFIT = EVAL(T,X) takes a classification or regression tree T and a
%   matrix X of predictor values, and produces a vector YFIT of predicted
%   response values. For a regression tree, YFIT(J) is the fitted response
%   value for a point having the predictor values X(J,:).  For a
%   classification tree, YFIT(J) is the class into which the tree would
%   assign the point with data X(J,:).
%
%   YFIT = EVAL(T,X,SUBTREES) takes an additional vector SUBTREES of
%   pruning levels, with 0 representing the full, unpruned tree.  T must
%   include a pruning sequence as created by the CLASSREGTREE constructor or
%   the PRUNE method. If SUBTREES has K elements and X has N rows, then the
%   output YFIT is an N-by-K matrix, with the Ith column containing the
%   fitted values produced by the SUBTREES(I) subtree.  SUBTREES must be
%   sorted in ascending order. (To compute fitted values for a tree that is
%   not part of the optimal pruning sequence, first use PRUNE to prune the
%   tree.)
%
%   [YFIT,NODE] = EVAL(...) also returns an array NODE of the same size
%   as YFIT containing the node number assigned to each row of X.  The
%   VIEW method can display the node numbers for any node you select.
%
%   [YFIT,NODE,CNUM] = EVAL(...) is valid only for classification trees.
%   It returns a vector CNUM containing the predicted class numbers.
%
%   NaN values in the X matrix are treated as missing.  If the EVAL method
%   encounters a missing value when it attempts to evaluate the split rule
%   at a branch node, it cannot determine whether to proceed to the left or
%   right child node.  Instead, it sets the corresponding fitted value
%   equal to the fitted value assigned to the branch node.
%
%   For a tree T, the syntax [...]=T(X) or [...]=T(X,SUBTREES) also invokes
%   the EVAL method.
%
%   Example: Find predicted classifications for Fisher's iris data.
%      load fisheriris;
%      t = classregtree(meas, species);  % create decision tree
%      sfit = eval(t,meas);              % find assigned class names
%      mean(strcmp(sfit,species))        % proportion correctly classified
%
%   See also CLASSREGTREE, CLASSREGTREE/PRUNE, CLASSREGTREE/VIEW, CLASSREGTREE/TEST.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.5 $  $Date: 2009/11/05 17:04:07 $

if ~isfloat(X)
    error('stats:classregtree:eval:BadData',...
        'X must be a matrix of floating-point numbers.');
end

[nr,nc] = size(X);
if nc~=Tree.npred
   error('stats:treeval:BadInput',...
         'The X matrix must have %d columns.',Tree.npred);
end

if nargin<3
   subtrees = 0;
elseif numel(subtrees)>length(subtrees)
   error('stats:treeval:BadSubtrees','SUBTREES must be a vector.');
elseif any(diff(subtrees)<0)
   error('stats:treeval:BadSubtrees','SUBTREES must be sorted.');
end

if ~isempty(Tree.prunelist)
   prunelist = Tree.prunelist;
elseif ~isequal(subtrees,0)
   Tree = prune(Tree);
else
   prunelist = Inf(size(Tree.node));
end
if isequal(Tree.method,'classification') || ~isempty(Tree.catcols)
    nodes = classregtreeEval(X',Tree.node,Tree.var,Tree.cut,Tree.children,...
        prunelist,subtrees);
    nodes = nodes';
    id = Tree.class(nodes);
    if nr==1
        id = id(:)';
    end

    if isequal(Tree.method,'classification')
       idname = Tree.classname(id);
       if nr==1
           idname = idname(:)';
       end
    else
       idname = id;
    end
else
    [idname, nodes] = regtreeEval(X',Tree.node, Tree.var, Tree.cut, Tree.children,...
        Tree.reg_ws, Tree.reg_pvars);
end