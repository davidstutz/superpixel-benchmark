function e=nodeerr(t,j)
%NODEERR Node error.
%   E=NODEERR(T) returns an N-element vector E of the errors of the
%   nodes in the tree T, where N is the number of nodes.  For a
%   regression tree, the error E(J) for node J is the variance of the
%   observations assigned to node J.  For a classification tree, E(J)
%   is the misclassification probability for node J.
%
%   E=NODEERR(T,J) takes an array J of node numbers and returns the 
%   errors for the specified nodes.
%
%   The error E is the so-called resubstitution error computed by applying
%   the tree to the same data used to create the tree.  This error is
%   likely to under-estimate the error you would find if you applied the
%   tree to new data.  The TEST method provides options to compute the
%   error (or cost) using cross-validation or a test sample.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES, CLASSREGTREE/TEST.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.8.1 $  $Date: 2007/02/02 23:22:00 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:nodeerr:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

if nargin<2
    e = t.nodeerr;
else
    e = t.nodeerr(j,:);
end
