% Given the seed locations and their radius returns a binary image that is
% 1 inside the seeds and 0 elsewhere
function binary_img = get_binary_img_from_seeds(seeds,img,radius)

    if (nargin < 3)
        radius = 1;
    end
    
    binary_img = zeros(size(img));
    
    ind = sub2ind(size(img),round(seeds(:,2)),round(seeds(:,1)));
    
    binary_img(ind) = 1;
    if (radius == 1)
        binary_img = imdilate(binary_img, ones(1,1));
    else
        binary_img = imdilate(binary_img, strel('disk',radius-1));
    end
    
    binary_img = double(binary_img > 0);