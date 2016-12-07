function [varargout] = subsasgn(varargin)
%SUBSASGN Subscripted reference for a CLASSREGTREE object.
%   Subscript assignment is not allowed for a CLASSREGTREE object.

%   Copyright 2006-2007 The MathWorks, Inc.
%   $Revision: 1.1.6.4 $  $Date: 2007/08/03 21:43:10 $

error('stats:classregtree:subsasgn:NotAllowed',...
      'Subscripted assignments are not allowed.')