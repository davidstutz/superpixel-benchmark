function sp_seg = merge_superpix_mex(I, bndrs, desire_sups, sigma, edge_modifier, color_modifier,thres, diagnostics)
% Usual parameters:
%   merge_superpix_mex(I, model, desired_sups);
%   I: Image
%   model: input boundary model for StructEdges
%   desired_sups: Number of desired superpixels, the algorithm won't give you exactly that number (usually a bit too many when you specify a small number), but you can still control the number of superpixels via this parameter
%
% Parameters that usually don't need changing:
%
%   sigma: controls the size of each superpixel, the larger sigma is, the smaller each superpixel would be
%   edge_modifier: controls the step size on edge weights. The larger the step size is, the faster the algorithm, but performance might be lowered slightly.
%   color_modifier: controls the step size on color differences. Similar to edge_modifier.
%   starting_threshold: Superpixels with color differences and edge differences below starting_threshold will be automatically merged, speeding up the algorithm

% Fuxin Li, Georgia Institute of Technology/Oregon State University, 2015

     if ~exist('thres','var') || isempty(thres)
         thres = 0.03;
     end
    if ~exist('sigma','var') || isempty(sigma)
        sigma = 1.5;
    end
    if ~exist('edge_modifier','var') || isempty(edge_modifier)
        edge_modifier = 0.8;
    end
    if ~exist('color_modifier','var') || isempty(color_modifier)
        color_modifier = 0.6;
    end
    if ~exist('desire_sups','var') || isempty(desire_sups)
        desire_sups = 64;
    end
    if ~exist('diagnostics','var') || isempty(diagnostics)
        diagnostics = false;
    end
    
    %model.opts.nms = 1;
    %model.opts.multiscale = 1;
    %model.opts.sharpen = 2;
    %model.opts.separatenms = 1;
    %bndrs = edgesDetect(I, model);
    
    sp_seg = watershed(bndrs);
    sp_seg = fill_zeros(I, sp_seg);
    t=tic();
	[~, sp_data] = precompute_pairwise_data_feature(sp_seg, uint8(bndrs * 255));
    I3 = permute(I,[3 1 2]);
    sp_seg = merge_superpixels2(I3, uint32(sp_seg), uint32(sp_data.edgelet_sp'), ...
                                single(sp_data.edge_vals), ...
                                size(sp_data.edgelet_sp,1), desire_sups, sigma, edge_modifier, color_modifier,thres);
%    sp_seg = sp_data.sp_seg;
     tt=toc(t);
     [~,~,sp_seg] = unique(sp_seg);
     sp_seg = reshape(sp_seg, size(I,1),size(I,2));  
     if diagnostics
        clear cmap
        num_each = histc(sp_seg(:),1:max(sp_seg(:)));
        I2 = reshape(I, size(I,1) * size(I,2), size(I,3));
            cmap(:,1) = accumarray(sp_seg(:), I2(:,1));
            cmap(:,2) = accumarray(sp_seg(:), I2(:,2));
            cmap(:,3) = accumarray(sp_seg(:), I2(:,3));
            cmap = bsxfun(@rdivide, cmap, num_each) / 255;
        cmap2 = SvmSegm_labelcolormap(size(cmap,1));
        cmap2 = cmap2(randperm(size(cmap,1)),:);
       figure,imshow(sp_seg, (cmap + 0.1 * cmap2) ./ 1.1);
     end
end


function [sp_seg] = fill_zeros(orig_I, init_seg)
    % fill in the boundary pixels with the neighboring segment with
    % the closest RGB color
    sp_seg = fill_in_segmentation(orig_I, init_seg, 0, 4);
    
    % remove zeros (pixels which are still marked as boundaries)
    nghbrs = [-1 0 1];
    nghbrs = [nghbrs-size(sp_seg,1), nghbrs, nghbrs+size(sp_seg,1)];
    nghbrs(5) = [];
    zero_locs = find(sp_seg == 0);
    nghbrs = bsxfun(@plus, zero_locs, nghbrs);
    nghbrs = sp_seg(nghbrs);
    % setting it to mode, which is the most frequently occuring
    % superpixel in the neighborhood
    sp_seg(zero_locs) = uint16(mode(single(nghbrs), 2));
end
