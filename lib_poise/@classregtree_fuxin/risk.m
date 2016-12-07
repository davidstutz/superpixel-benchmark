function r=risk(t,j,varargin)
%RISK Node risk.
%   R=RISK(T) returns an N-element vector R of the risk of the nodes in the
%   tree T, where N is the number of nodes.  
%
%   R=RISK(T,J) takes an array J of node numbers and returns the risk
%   values for the specified nodes.
%
%   R=RISK(T,J,'criterion','error') returns risk vector R, where R(J) for
%   node J is the node error E(J) (classification error or mean squared
%   error for regression) weighted by the node probability P(J). 
%
%   R=RISK(T,J,'criterion','impurity') computes risk by using the node
%   impurity measure for each node instead of the node error. This option
%   is only valid for classification trees grown using impurity measures
%   such as Gini index or deviance. 
%
%   See also CLASSREGTREE, CLASSREGTREE/NODEERR, CLASSREGTREE/NODEPROB.

%   Copyright 2006-20097 The MathWorks, Inc. 
%   $Revision: 1.1.8.4 $  $Date: 2009/05/07 18:32:51 $

if nargin>=2 && ~validatenodes(t,j)
    error('stats:classregtree:risk:InvalidNode',...
          'J must be an array of node numbers or a logical array of the proper size.');
end

args = {'criterion'};
defs = {'error'};
[~,emsg,crit] = getargs(args,defs,varargin{:});
if ~isempty(emsg)
    error('stats:classregtree:risk:InvalidInput','Invalid input: %s',emsg);
end

if isempty(strmatch(lower(crit),'error')) && isempty(strmatch(lower(crit),'impurity'))
    error('stats:classregtree:risk:InvalidInput',...
        '''crit'' argument must be either ''error'' or ''impurity''.');
end

if strcmpi(crit,'error')
    r = t.nodeprob .* t.nodeerr;
else
    if isempty(t.impurity)
        error('stats:classregtree:risk:InvalidInput',...
        'Node risk cannot be computed using impurity. This is either a regression tree or splits were not found using impurity.');
    end
    r = t.nodeprob .* t.impurity;
end

if nargin>=2
    r = r(j,:);
end
end
