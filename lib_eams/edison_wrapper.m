function [varargout] = edison_wrapper(rgbim, featurefun, varargin)
%
% Performing mean_shift operation on image
%
% Usage:
%   [fimage labels modes regSize grad conf] = edison_wrapper(rgbim, featurefunc, ...)
% 
% Inputs:
%   rgbim - original image in RGB space
%   featurefunc - converting RGB to some feature space in which to perform
%                 the segmentation, like @RGB2Lab etc.
%
% Allowed parameters:
%   'steps'       - What steps the algorithm should perform:
%                   1 - only mean shift filtering
%                   2 - filtering and region fusion [default]
%   'synergistic' - perform synergistic segmentation [true]|false
%   'SpatialBandWidth' - segmentation spatial radius (integer) [7]
%   'RangeBandWidth'   - segmentation feature space radius (float) [6.5]
%   'MinimumRegionArea'- minimum segment area (integer) [20]
%   'SpeedUp'          - algorithm speed up {1,2,3} [2]
%   'GradientWindowRadius' - synergistic parameters (integer) [2]
%   'MixtureParameter' - synergistic parameter (float 0,1) [.3]
%   'EdgeStrengthThreshold'- synergistic parameter (float 0,1) [.3]
%
% Outputs:
%   fimage  - the result in feature space
%   labels  - labels of regions [if steps==2]
%   modes   - list of all modes [if steps==2]
%   regSize - size, in pixels, of each region [if steps==2]
%   grad    - gradient map      [if steps==2 and synergistic]
%   conf    - confidence map    [if steps==2 and synergistic]
%


% rgbim must be of type uint8
if ~isa(rgbim,'uint8'),
    if max(rgbim(:)) <= 1,
        rgbim = im2uint8(rgbim);
    else
        rgbim = uint8(rgbim);
    end
end
imsiz = size(rgbim);

fim = im2single(rgbim);
fim = single(featurefun(fim));

rgbim = permute(rgbim,[3 2 1]);
fim = permute(fim, [3 2 1]);

p = parse_inputs(varargin);

labels = [];
modes =[];
regSize = [];
grad = [];
conf = [];

if p.steps == 1
    [fimage] = edison_wrapper_mex(fim, rgbim, p);
else
    if p.synergistic
        [fimage labels modes regSize grad conf]  = edison_wrapper_mex(fim, rgbim, p);
    else
        [fimage labels modes regSize]  = edison_wrapper_mex(fim, rgbim, p);
    end
    grad = reshape(grad,imsiz([2 1]))';
    conf = reshape(conf',imsiz([2 1]))';
end
fimage = permute(fimage, [3 2 1]);

if nargout >= 1, varargout{1} = fimage; end;
if nargout >= 2, varargout{2} = labels'; end;
if nargout >= 3, varargout{3} = modes; end;
if nargout >= 4, varargout{4} = regSize; end;
if nargout >= 5, varargout{5} = grad; end;
if nargout >= 6, varargout{6} = conf; end;


%--------------------------------------------------------%
function [p] = parse_inputs(args)
% Allowed parameters
%   'steps'       - What steps the algorithm should perform:
%                   1 - only mean shift filtering
%                   2 - filtering and region fusion [defualt]
%   'synergistic' - perform synergistic segmentation [true]|false
%   'SpatialBandWidth' - segmentation spatial radius (integer) [7]
%   'RangeBandWidth'   - segmentation feature space radius (float) [6.5]
%   'MinimumRegionArea'- minimum segment area (integer) [20]
%   'SpeedUp'          - algorithm speed up {0,1,2} [1]
%   'GradientWindowRadius' - synergistic parameters (integer) [2]
%   'MixtureParameter' - synergistic parameter (float 0,1) [.3]
%   'EdgeStrengthThreshold'- synergistic parameter (float 0,1) [.3]


% convert ars to parameters - then init all the rest according to defualts
try 
    p = struct(args{:});
catch
    error('edison_wrapper:parse_inputs','Cannot parse arguments');
end

% % modes of operation
% -. edge detection -- currently unsupported
% 1. Filtering
% 2. Fusing regions
% 3. Segmentation
if ~isfield(p,'steps')
    p.steps = 2;
end
if p.steps ~= 1 && p.steps ~=2 
    error('edison_wrapper:parse_inputs','steps must be either 1 or 2');
end

% % parameters
% Flags
% 1. synergistic
if ~isfield(p,'synergistic')
    p.synergistic = true;
end
p.synergistic = logical(p.synergistic);

% Mean Shift Segmentation parameters
% SpatialBandWidth [integer]
if ~isfield(p,'SpatialBandWidth')
    p.SpatialBandWidth = 7;
end
if p.SpatialBandWidth < 0 || p.SpatialBandWidth ~= round(p.SpatialBandWidth)
    error('edison_wrapper:parse_inputs','SpatialBandWidth must be a positive integer');
end
% RangeBandWidth [float]
if ~isfield(p,'RangeBandWidth')
    p.RangeBandWidth = 6.5;
end
if p.RangeBandWidth < 0
    error('edison_wrapper:parse_inputs','RangeBandWidth must be positive');
end
% MinimumRegionArea [integer]
if ~isfield(p,'MinimumRegionArea')
    p.MinimumRegionArea = 20;
end
if p.MinimumRegionArea < 0 || p.MinimumRegionArea ~= round(p.MinimumRegionArea)
    error('edison_wrapper:parse_inputs','MinimumRegionArea must be a positive integer');
end
% SpeedUp
if ~isfield(p,'SpeedUp')
    p.SpeedUp = 2;
end
if p.SpeedUp ~=1 && p.SpeedUp ~= 2 && p.SpeedUp ~= 3
    error('edison_wrapper:parse_inputs','SpeedUp must be either 1, 2 or 3');
end
% Synergistic Segmentation parameters
% GradientWindowRadius [integer]
if ~isfield(p,'GradientWindowRadius')
    p.GradientWindowRadius = 2;
end
if p.GradientWindowRadius < 0 || p.GradientWindowRadius ~= round(p.GradientWindowRadius)
    error('edison_wrapper:parse_inputs','GradientWindowRadius must be a positive integer');
end
% MixtureParameter [float (0,1)]
if ~isfield(p,'MixtureParameter')
    p.MixtureParameter = .3;
end
if p.MixtureParameter < 0 || p.MixtureParameter > 1
    error('edison_wrapper:parse_inputs','MixtureParameter must be between zero and one');
end
% EdgeStrengthThreshold [float (0,1)]
if ~isfield(p,'EdgeStrengthThreshold')
    p.EdgeStrengthThreshold = .3;
end
if p.EdgeStrengthThreshold < 0 || p.EdgeStrengthThreshold > 1
    error('edison_wrapper:parse_inputs','MixtureParameter must be between zero and one');
end

% % Currently unsupported
% Edge Detection Parameters
% GradientWindowRadius [integer]
% MinimumLength [integer]
% NmxRank [float (0,1)]
% NmxConf [float (0,1)]
% NmxType
% HysterisisHighRank [float (0,1)]
% HysterisisHighConf [float (0,1)]
% HysterisisHighType 
% HysterisisLowRank [float (0,1)]
% HysterisisLowConf [float (0,1)]
% HysterisisLowType 

% % 