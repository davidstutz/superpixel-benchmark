function rgb_im_new = RGB2RGB(rgb_im)
% Just a placeholder.

    if size(rgb_im, 3) ~= 3
        error('im must have three color channels');
    end
    
    rgb_im_new = 255*im2single(rgb_im);
end

