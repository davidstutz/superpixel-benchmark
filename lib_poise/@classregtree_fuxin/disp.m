function disp(t)
%DISP Display a CLASSREGTREE object.
%   DISP(T) prints the CLASSREGTREE object T.
%
%   See also CLASSREGTREE, CLASSREGTREE/VIEW.

%   Copyright 2006 The MathWorks, Inc. 
%   $Revision: 1.1.6.3.8.1 $  $Date: 2009/12/11 15:44:55 $

isLoose = strcmp(get(0,'FormatSpacing'),'loose');
maxWidth = get(0,'CommandWindowSize'); maxWidth = maxWidth(1);

% disp(struct(t));

if (isLoose), fprintf('\n'); end

% Get some information about the whole tree
maxnode = numel(t.node);
nd = 1 + floor(log10(maxnode)); % number of digits for node number
names = t.names;
if isempty(names)
    names = strcat('x',strread(sprintf('%d\n',1:t.npred),'%s\n'));
end
isregression = isequal(t.method,'regression');
fprintf('Decision tree for %s\n',t.method);

% Display information about each node
for j=1:maxnode
    if any(t.children(j,:))
        % branch node
        vnum = t.var(j);
        vname = names{abs(vnum)};
        cut = t.cut{j};
        kids = t.children(j,:);
        if     strcmp(type(t),'regression')
            Yfit = t.class(j);
        elseif strcmp(type(t),'classification')
            Yfit = t.classname{t.class(j)};
        end
        Yfit = num2str(Yfit,'%g');
        if vnum>0        % continuous predictor "<" condition
            condleft = sprintf('%s<%g',vname,cut);
            condright = sprintf('%s>=%g',vname,cut);
            fprintf('%*d  if %s then node %d elseif %s then node %d else %s\n',...
                nd,j,condleft,kids(1),condright,kids(2),Yfit);
        else             % categorical predictor, membership condition
            cats = cut{1};
            if isscalar(cats)
                condleft = sprintf('%s=%g',vname,cats);
            else
                set = deblank(num2str(cats,'%g '));
                condleft = sprintf('%s in {%s}',vname,set);
            end
            cats = cut{2};
            if isscalar(cats)
                condright = sprintf('%s=%g',vname,cats);
            else
                set = deblank(num2str(cats,'%g '));
                condright = sprintf('%s in {%s}',vname,set);
            end
            fprintf('%*d  if %s then node %d elseif %s then node %d else %s\n',...
                nd,j,condleft,kids(1),condright,kids(2),Yfit);
        end
    else
        % terminal node, display fit (regression) or class assignment
        if isregression
            fprintf('%*d  fit = %g\n',nd,j,t.class(j));
        else
            fprintf('%*d  class = %s\n',nd,j,t.classname{t.class(j)});
        end
    end
end
if (isLoose), fprintf('\n'); end