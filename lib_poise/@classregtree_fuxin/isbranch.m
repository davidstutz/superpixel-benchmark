function tf=isbranch(t,j)
%ISBRANCH Is node a branch node.
%   TF=ISBRANCH(T) returns an N-element logical vector TF that is true (1) for
%   each branch node and false (0) for each leaf node.
%
%   TF=ISBRANCH(T,J) takes an array J of node numbers and returns an array of
%   logical values for the specified nodes.
%
%   See also CLASSREGTREE, CLASSREGTREE/NUMNODES, CLASSREGTREE/CUTVAR.

%   Copyright 2006-2007 The MathWorks, Inc. 
%   $Revision: 1.1.6.3 $  $Date: 2007/02/15 21:48:09 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:children:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

if nargin<2
    tf = any(t.children~=0,2);
else
    tf = any(t.children(j,:)~=0,2);
end

