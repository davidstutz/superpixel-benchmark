% Code originally provided by Ren and Bo:
% X. Ren, L. Bo.
% Discriminatively trained sparse code gradients for contour detection.
% In Advances in Neural Information Processing Systems, volume 25, pages 584–592. Curran Associates, 2012.

function [bdry]  = nyuv2_seg2bdry(seg, fmt)

    if nargin < 2
        fmt = 'doubleSize';
    end;

    if ~strcmp(fmt,'imageSize') && ~strcmp(fmt,'doubleSize')
        error('possible values for fmt are: imageSize and doubleSize');
    end;

    [tx, ty, nch] = size(seg);

    if nch ~= 1
        error('seg must be a scalar image');
    end;

    bdry = zeros(2*tx+1, 2*ty+1);

    edgels_v = ( seg(1:end-1, :) ~= seg(2:end, :) );
    edgels_v(end+1, :) = 0;
    edgels_h = ( seg(:, 1:end-1) ~= seg(:, 2:end) );
    edgels_h(:, end+1) = 0;

    bdry(3:2:end, 2:2:end) = edgels_v;
    bdry(2:2:end, 3:2:end) = edgels_h;
    bdry(3:2:end-1, 3:2:end-1)= max ( max(edgels_h(1:end-1, 1:end-1), edgels_h(2:end, 1:end-1)), max(edgels_v(1:end-1,1:end-1), edgels_v(1:end-1,2:end)) );

    bdry(1, :) = bdry(2, :);
    bdry(:, 1) = bdry(:, 2);
    bdry(end, :) = bdry(end-1, :);
    bdry(:, end) = bdry(:, end-1);

    if strcmp(fmt,'imageSize')
        bdry = bdry(3:2:end, 3:2:end);
    end;
end