function seeds = get_initial_seeds(img, num_seeds, speed, method)
if nargin < 4
    method = 'original';
end
if strcmp(method,'original')
    seeds = get_seeds_orig(img, num_seeds, speed);
elseif strcmp(method, 'random')
    seeds = get_seeds_random(img, num_seeds);
end

% randomly pick num_seeds image locations that are not close to edges
% (seed placement is not particularly "nice", but this is as random
% as it gets...)
function seeds = get_seeds_random(img, num_seeds)
nonedges = find(imfilter(im2double(edge(img,'canny')), ...
    fspecial('gaussian', [5 5], 1.5)) == 0);
[I,J] = ind2sub(size(img), ...
    nonedges(floor(length(nonedges) * rand(num_seeds,1) + 1)));
seeds = [J I];
return;

% start with seeds placed on a regular grid, then add a small random
% displacement to each seed, them move them to local minima of gradient
function seeds = get_seeds_orig(img, num_seeds, speed)
    size_grid = floor(0.5 + sqrt(prod(size(img)) / num_seeds));
    rows = size(img,1) / size_grid;
    cols = size(img,2) / size_grid;
    size_grid_row = size(img,1) / ceil(rows);
    size_grid_col = size(img,2) / ceil(cols);
    [y,x] = meshgrid(0:ceil(rows-1),0:ceil(cols-1));
    x = x .* size_grid_col + size_grid_col/2;
    y = y .* size_grid_row + size_grid_row/2;
    
%     % std dev of random displacement in pixels
%     sigma = floor(size_grid_row / 6);
%     % add small random perturbation to seed placement
%     x = x + randn(size(x)) * sigma;
%     y = y + randn(size(y)) * sigma;
%     % make sure seeds are still inside the image
%     x(x < 1) = 1; x(x > size(img,2)) = size(img,2);
%     y(y < 1) = 1; y(y > size(img,1)) = size(img,1);

    % Pick only local minima of the gradient magnitude
%     [dx,dy] = gradient(img);
%     mag = sqrt(dx.^2 + dy.^2);
%     mag = imfilter(mag, fspecial('average',5));
    mag = 1-speed;
    
    % Pick only local minima for initialized seeds
    minDistBetweenSeeds = min(size_grid_row,size_grid_col);
    seedRadius = 1;
    
    % 2*maxShift + 2*seedRadius must be less then minDistBetweenSeeds
    
    maxShift = floor((minDistBetweenSeeds - 2*seedRadius) / 2) - 1;
    [dy,dx] = local_min(mag,ceil(maxShift/2));
    new_x = dx(round(y(1,:)),round(x(:,1)))';
    new_y = dy(round(y(1,:)),round(x(:,1)))';
    x = new_x;
    y = new_y;
    off = 2;
    x = max(1+off,min(x,size(img,2)-off));
    y = max(1+off,min(y,size(img,1)-off));

    seeds = [x(:) y(:)];