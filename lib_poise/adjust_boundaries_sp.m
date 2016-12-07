% @authors:     Ahmad Humayun
% @contact:     ahumayun@cc.gatech.edu
% @affiliation: Georgia Institute of Technology
% @date:        Fall 2013 - Summer 2014

function [bndry_thin] = adjust_boundaries_sp(bndry_thin, sp_seg)
% This function moves boundaries so that they are better aligned with the 
% superpixel boundaries. It just looks at the neighborhood of each
% superpixel boundary and finds if any bndry_thin is in the region - and if
% so it just moves values to the superpixel boundary pixels.

    nrows = size(sp_seg, 1);
    ncols = size(sp_seg, 2);

    % find suerpixel boundary pixels
    sp_bndr = false(size(sp_seg));
    % find boundary pixels in the south
    sp_bndr(sp_seg ~= sp_seg([2:end end],:)) = 1;
    % find boundary pixels in the north
    sp_bndr(sp_seg ~= sp_seg([1 1:end-1],:)) = 1;
    % find boundary pixels in the east
    sp_bndr(sp_seg ~= sp_seg(:,[2:end end])) = 1;
    % find boundary pixels in the west
    sp_bndr(sp_seg ~= sp_seg(:,[1 1:end-1])) = 1;

    bndry_thin_out_sp_bndry = bndry_thin;
    bndry_thin_out_sp_bndry(sp_bndr) = 0;
    out_sp_locs = find(bndry_thin_out_sp_bndry);
    [out_sp_locs_r, out_sp_locs_c] = ...
        ind2sub([nrows, ncols], out_sp_locs);

    nghbrs = [-nrows -1 1 nrows];
    nghbrs = bsxfun(@plus, out_sp_locs, nghbrs);

    % throw neighbors which are beyond frame boundaries
    [r, c] = find(nghbrs < 1);
    nghbrs(nghbrs < 1) = out_sp_locs(r);
    [r, c] = find(nghbrs > nrows*ncols);
    nghbrs(nghbrs > nrows*ncols) = out_sp_locs(r);
    nghbrs(out_sp_locs_r == 1, 2) = ...
        out_sp_locs(out_sp_locs_r == 1);
    nghbrs(out_sp_locs_r == nrows, 3) = ...
        out_sp_locs(out_sp_locs_r == nrows);
    nghbrs(out_sp_locs_c == 1, 1) = ...
        out_sp_locs(out_sp_locs_c == 1);
    nghbrs(out_sp_locs_c == ncols, 4) = ...
        out_sp_locs(out_sp_locs_c == ncols);

    % find the values of gPb in each selected pixel's neighborhood
    bndry_thin_nghbrs = double(bndry_thin(nghbrs));
    % find if these selected neighbors fall on the SP boundaries
    bndr_nghbrs_sp = sp_bndr(nghbrs);
    % find the location on the SP boundary with the minimum gPb 
    % value for each pixel's neighborhood
    bndry_thin_nghbrs(~bndr_nghbrs_sp) = inf;
    [min_gpb, min_idx] = min(bndry_thin_nghbrs, [], 2);
    % if infinity was returned for a particular neighborhood it
    % means no superpixel boundary exists in that neighborhood
    valid_repl = ~isinf(min_gpb);
    min_idx = min_idx(valid_repl);
    out_sp_locs = out_sp_locs(valid_repl);

    % find the pixel indices which should be replaced by new values
    repl_idx_r = [1:size(nghbrs,1)]';
    repl_idx_r = repl_idx_r(valid_repl);
    rep_sp_locs = nghbrs(sub2ind(size(nghbrs), repl_idx_r, min_idx));

    % add source plus destination gPb values
    % This operation synthetically moves the gPb boundary closer to
    % the superpixel boundary
    bndry_thin(rep_sp_locs) = bndry_thin(rep_sp_locs) + ...
                              bndry_thin(out_sp_locs);

    % it doesn't matter if you set the source gPb values to 0,
    % because all edge potentials are made from the SP boundaries
    bndry_thin(out_sp_locs) = 0;
end