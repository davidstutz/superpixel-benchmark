function [sp_data, val, bndry] = ...
            prepare_pairwise_data_feature(sp_data, bndry, edge_diff_type)
%PREPARE_PAIRWISE_DATA_FEATURE takes superpixels and boundaries (thin
% boundaries) and computes superpixel pairs and its edgelet information -
% and aligns the given boundaries to the superpixel boundaries. It also
% computes the bounary difference values between superpixel boundary pixel
% pairs.
%
% @authors:     Ahmad Humayun
% @contact:     ahumayun@cc.gatech.edu
% @affiliation: Georgia Institute of Technology
% @date:        Fall 2013 - Summer 2014

    % get the indexes for the pairs of pixels to compute pairwise 
    %  potentials over
    [bndry_pairs, edgelet_ids, edgelet_sp] = ...
        generate_sp_neighborhood(sp_data.sp_seg);
    
    % store in the class
    sp_data.bndry_pairs = bndry_pairs;
    sp_data.edgelet_ids = edgelet_ids;
    sp_data.edgelet_len = accumarray(edgelet_ids, 1);
    sp_data.edgelet_sp = edgelet_sp;
 
%     if do_sp_pairwise_scaling
%         % compute the degree of each superpixel in the graph (number of
%         % neighboring superpixels)
%         deg_verts = accumarray(edgelet_sp(:), ...
%                                ones(numel(edgelet_sp),1));
% 
%         % adjust the contrast sensitive weight according to the average
%         % degree of the two superpixels sharing the edgelet. If the
%         % average degree is more than 4, then the contrast sensitive
%         % weight should be reduced, and vice verse. The number 4 comes
%         % from the average degree for a pixel level graph
%         deg_edgelet_sps = deg_verts(edgelet_sp);
%         avg_edgelet_deg = mean(deg_edgelet_sps,2);
%         edgelet_scaling = 4 ./ avg_edgelet_deg;
%         CONTRAST_SENSITIVE_WEIGHT = CONTRAST_SENSITIVE_WEIGHT .* ...
%                                     edgelet_scaling(edgelet_ids);
%     end

    % make all values on the boundary positive
    if any(bndry(:) < 0)
        bndry = bndry + abs(min(bndry(:)));
    end

    % further adjust boundaries by looking at superpixels
    bndry = adjust_boundaries_sp(bndry, sp_data.sp_seg);

    % val can range [0, 255]
    val = intens_pixel_diff_mex(double(bndry), ...
            uint32(bndry_pairs(:,1)), uint32(bndry_pairs(:,2)), ...
            edge_diff_type);
    
    % intens_pixel_diff_mex is equivalent to:
    % bndry_dbl = double(bndry);
    % val = abs(bndry_dbl(ids(:,1)) - bndry_dbl(ids(:,2)));
end
