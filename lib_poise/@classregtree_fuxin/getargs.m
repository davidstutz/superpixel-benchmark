function [eid,emsg,varargout]= getargs(pnames,dflts,varargin)
%
% getargs() processes parameter name/value pairs, allowing a function
% with a large number of optional input parameters to be called with values
% being provided only for those parameters whose names are passed (all
% other parameters receive default values).  The syntax is as follows:
%
% [eid,emsg,a,b,...]= getargs(pnames,dflts,'name1',val1,'name2',val2,...)
%
% This function, which is based on statgetargs(), accepts a cell array
% pnames of valid parameter names, a cell array dflts of default values
% for the parameters named in pnames, and additional parameter name/value
% pairs.  Unlike statgetargs(), parameter getargs() does case sensitive
% parameter name comparisons.

% getargs() returns parameter values A,B,... in the same order as the
% names in pnames.  Outputs corresponding to entries in pnames that are
% not specified in the name/value pairs are set to the corresponding value
% from DFLTS.  If nargout is equal to
% length(pnames)+1, then unrecognized name/value pairs are an error.  If
% nargout is equal to length(pnames)+2, then all unrecognized name/value
% pairs are returned in a single cell array following any other outputs.
%
% EID and EMSG are empty if the arguments are valid.  If an error occurs,
% EMSG is the text of an error message and EID is the final component
% of an error message id.  getargs() does not actually throw any errors,
% but rather returns EID and EMSG so that the caller may throw the error.
% Outputs will be partially processed after an error occurs.
%
% This utility is used by some Statistics Toolbox functions to process
% name/value pair arguments.
%
% Example:
%     pnames = {'color' 'linestyle', 'linewidth'}
%     dflts  = {    'r'         '_'          '1'}
%     varargin = {{'linew' 2 'nonesuch' [1 2 3] 'linestyle' ':'}
%     [eid,emsg,c,ls,lw] = getargs(pnames,dflts,varargin{:})    % error
%     [eid,emsg,c,ls,lw,ur] = getargs(pnames,dflts,varargin{:}) % ok
%
% We always create (nparams+2) outputs:
%    one each for emsg and eid
%    nparams varargs for values corresponding to names in pnames
%
% If the calling program asks for one more (nargout == nparams+3), it's for
% unrecognized names/values

% Initialize some variables
emsg = '';
eid = '';
nparams = length(pnames);
varargout = dflts;
unrecog = {};
nargs = length(varargin);

% Must have name/value pairs
if mod(nargs,2)~=0
    eid = 'WrongNumberArgs';
    emsg = 'Wrong number of arguments.';
else
    % Process name/value pairs
    for j= 1:2:nargs
        pname = varargin{j};
        if ~ischar(pname)
            eid = 'BadParamName';
            emsg = 'Parameter name must be text.';
            break;
        end
        i= strmatch(pname,pnames);
        if isempty(i)
            % if they've asked to get back unrecognized names/values, add this
            % one to the list
            if nargout > nparams+2
                unrecog((end+1):(end+2))= {varargin{j} varargin{j+1}};

                % otherwise, it's an error
            else
                eid = 'BadParamName';
                emsg = sprintf('Invalid parameter name:  %s.',pname);
                break;
            end
        elseif length(i)>1
            eid = 'BadParamName';
            emsg = sprintf('Ambiguous parameter name:  %s.',pname);
            break;
        else
            varargout{i} = varargin{j+1};
        end
    end
end

varargout{nparams+1} = unrecog;
