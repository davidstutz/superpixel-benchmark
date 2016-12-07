function [bndry_pairs, edgelet_ids, edgelet_sp] = ...
        generate_sp_neighborhood(sp_seg)
% finds all pairs of superpixels which share a boundary.
%
% @authors:     Ahmad Humayun
% @contact:     ahumayun@cc.gatech.edu
% @affiliation: Georgia Institute of Technology
% @date:        Fall 2013 - Summer 2014

    % create offset images to find superpixel boundary pixels
    south_seg = [sp_seg(2:end,:); sp_seg(end,:)];
    east_seg = [sp_seg(:,2:end), sp_seg(:,end)];

    % find the actual boundary pixels
    south_bndry = south_seg ~= sp_seg;
    east_bndry = east_seg ~= sp_seg;

    % find the src and the dst superpixel for each boundary pixel
    south_sp_src = sp_seg(south_bndry);
    south_sp_dst = south_seg(south_bndry);
    east_sp_src = sp_seg(east_bndry);
    east_sp_dst = east_seg(east_bndry);

    south_bndry_pairs = find(south_bndry);
    east_bndry_pairs = find(east_bndry);
    bndry_pairs = [south_bndry_pairs, south_bndry_pairs+1;
                   east_bndry_pairs, ...
                   east_bndry_pairs+size(sp_seg,1)];

    src_dst_pairs = [south_sp_src south_sp_dst; ...
                     east_sp_src east_sp_dst];

    [src_dst_pairs, sorted_ind] = sort(src_dst_pairs, 2);

    sorted_ind = sub2ind(size(src_dst_pairs), ...
        repmat((1:size(sorted_ind,1))', 1, 2), sorted_ind);

    bndry_pairs = bndry_pairs(sorted_ind);

    [edgelet_sp, ~, edgelet_ids] = unique(src_dst_pairs, 'rows');
end