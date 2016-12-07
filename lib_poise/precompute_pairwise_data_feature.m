% @authors:     Ahmad Humayun,  Fuxin Li
% @contact:     ahumayun@cc.gatech.edu
% @affiliation: Georgia Institute of Technology
% @date:        Fall 2013 - Spring 2015

function [bndry_thin, sp_data] = precompute_pairwise_data_feature(sp_seg, bndry_thin)
    t_pair_precomp = tic;

    % Compute superpixel boundary edgelet information - and computes 
    % bounary difference values on superpixel boundary pixel pairs, after
    % boundary alignment
	sp_data.sp_seg = sp_seg;
    [sp_data, val, bndry_thin] = ...
        prepare_pairwise_data_feature(sp_data, ...
                                      bndry_thin, 0);

    % setting median value for each edglet
    [feat, the_rest, ~, sel] = gen_sp_feats(sp_data, val);
    
    load('pair_trees_very_high.mat');
    accum_edge(sel) = eval_tree_lad(feat', trees, ...
                                    scaling_type, scaling, ...
                                    f0, rho);
    accum_edge(accum_edge < 0) = 0;
    % Instead of median use eval_tree_lad
    accum_edge(~sel) = the_rest(~sel);
    % range [0,1] unlike RIGOR/POISE
    sp_data.edge_vals = accum_edge;
end
