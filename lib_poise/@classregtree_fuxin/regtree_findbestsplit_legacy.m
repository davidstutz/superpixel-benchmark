function [critval, cutval] = regtree_findbestsplit(Tree,x, c, w, Xnode_pvars, ...
             bestcrit, minleaf)
% x: sorted input for the split dimension
% c: output
% w: weight vector
% ignore w for now.
% Xnode_pvars: the columns we already know at this point
% bestcrit: current best, if we can't better that then we should stop
% minleaf: minimal leaf size
    lennode = length(x);
    allX = [Xnode_pvars x];
    % convert to integer
    minleaf = ceil(minleaf);
    [all_split, split_pos] = unique(x);
%    LinReg_left = LinearRegressor_Data(allX(1:minleaf,:),c(1:minleaf),w(1:minleaf));
%    LinReg_right = LinearRegressor_Data(allX(minleaf+1:end,:),c(minleaf+1:end),w(minleaf+1:end));
    position = find(split_pos >= minleaf,1,'first');
    critval = bestcrit;
    cutval = 0;
    xy = x .* c;
    x2 = x .^2;
    y2 = c .^2;
    last_pos = 1;
    ally_right = sum(c);
    x2_right = sum(x2);
    xy_right = sum(xy);
    y2_right = sum(y2);
    xright = sum(x);
    ally_left = 0;
    x2_left = 0;
    xy_left = 0;
    y2_left = 0;
    xleft = 0;
    nleft = 0;
    nright = lennode;
    wleft = 0;
    my_right = ally_right / nright;
    wright = (xy_right - xright * my_right) / x2_right;
    loss = y2_right - 2 * wright * xy_right + wright^2 * x2_right ...
     + y2_left - 2 * wleft * xy_left + wleft^2 * x2_left;
    if -loss > critval
        critval = -loss;
        cutval = -inf;
    end
    while(split_pos(position) <= lennode - minleaf)
        if mod(position,10009)==0
            disp(['Position: ' num2str(position)]);
        end
        the_pos = split_pos(position);
        % change on y
        y_change = sum(c(last_pos:the_pos));
        y2_change = sum(y2(last_pos:the_pos));
        ally_left = ally_left + y_change;
        ally_right = ally_right - y_change;
        y2_left = y2_left + y2_change;
        y2_right = y2_right - y2_change;
        nleft = nleft + the_pos - last_pos + 1;
        nright = nright - (the_pos - last_pos + 1);
        my_left = ally_left / nleft;
        my_right = ally_right / nright;
        % change on x
        x2_change =  sum(x2(last_pos:the_pos));
        x_change = sum(x(last_pos:the_pos));
        x2_left = x2_left + x2_change;
        x2_right = x2_right - x2_change;
        xleft = xleft + x_change;
        xright = xright - x_change;
        % change on xy
        xy_change = sum(xy(last_pos:the_pos));
        xy_left = xy_left + xy_change;
        xy_right = xy_right - xy_change;
        % figure out w
        wright = (xy_right - xright * my_right) / x2_right;
        wleft = (xy_left - xleft * my_left) / x2_left;
        loss = y2_right - 2 * wright * xy_right + wright^2 * x2_right ...
         + y2_left - 2 * wleft * xy_left + wleft^2 * x2_left;
        % Use negative to be consistent with other classification and
        % regression trees
        if -loss > critval
            critval = -loss;
            cutval = (x(the_pos) + x(the_pos + 1)) / 2;
        end
        % Move one step to the right
        last_pos = the_pos + 1;
        position = position + 1;
    end
    if cutval == -inf
        critval = -inf;
    end
end