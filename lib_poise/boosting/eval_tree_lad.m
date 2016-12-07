function val = eval_tree_lad(features, trees, scaling_type, scaling, f0, rho)
    if ~isempty(scaling_type)
        new_feats = scale_data(features', scaling_type, scaling)';
    else
        new_feats = features;
    end
    val = ones(size(features,1),1) * f0;
    for i=1:length(trees)
        val = val + rho(i) * trees{i}.eval(double(new_feats));
    end
end
