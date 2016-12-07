function [varargout] = subsref(t,s)
%SUBSREF Subscripted reference for a classregtree object.
%   B = SUBSREF(T,S) is called for the syntax T(X) when T is a
%   classregtree object.  S is a structure array with the fields:
%       type -- string containing '()', '{}', or '.' specifying the
%               subscript type.
%       subs -- Cell array or string containing the actual subscripts.
%
%   [...]=T(...) invokes the EVAL method for the tree T.
%
%   See also CLASSREGTREE, CLASSREGTREE/EVAL.

%   Copyright 2006-2008 The MathWorks, Inc.
%   $Revision: 1.1.6.6 $  $Date: 2008/10/31 07:40:45 $

switch s(1).type
    case '()'
        % No cascaded subscripts are allowed to follow parenthesis indexing.
        if ~isscalar(s)
            error('stats:classregtree:subsref:InvalidSubscriptExpr', ...
                'Subscript expression is invalid.  Use () indexing to evaluate the tree.');

        elseif numel(s(1).subs) ~= 1
            error('stats:classregtree:subsref:BadSubscript', ...
                '() indexing requires a single array of predictor values.');
        end

        % '()' is a reference to the eval method
        [varargout{1:nargout}] = t.eval(s(1).subs{1});


    case '{}'
        error('stats:classregtree:subsref:CellSubscript', ...
            'The CLASSREGTREE class does not support cell array indexing.');

    case '.'
        methodnames = methods(t);
        if ismember(s(1).subs,methodnames)
            % Support dot subscripting to call methods
            if isscalar(s)
                args = {};
            elseif numel(s)>2 || ~isequal(s(2).type,'()')
                error('stats:classregtree:subsref:BadSubscript', ...
                    'Invalid subscripting for CLASSREGTREE object.');
            elseif numel(s)==2
                args = s(2).subs;
            end
            if isequal(s(1).subs,'view') %% nargout==0
                % special case to suppress handle display from view
                view(t,args{:});
            elseif isequal(s(1).subs,'display') %% nargout==0
                % special case to supply variable name to display method
                display(t,inputname(1));
            else
                [varargout{1:nargout}] = feval(s(1).subs,t,args{:});
            end
        else
            % A reference to a variable or a property.
            b = t.(s(1).subs);

            if isscalar(s)
                % If there's no cascaded subscripting, only ever assign the var itself.
                varargout{1} = b;
            else
                % Now let the variable's subsref handle the remaining subscripts in
                % things like a.name(...) or  a.name{...} or a.name.attribute. This
                % may return a comma-separated list, so ask for and assign to as many
                % outputs as we're given.  If there's no LHS to the original expression,
                % then we're given nargout==0, and this only assigns one output and
                % drops everything else in the CSL.
                try
                    [varargout{1:nargout}] = subsref(b,s(2:end));
                catch ME
                    rethrow(ME);
                end
            end
        end
end
