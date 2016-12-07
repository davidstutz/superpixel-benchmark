function seg_new = fill_in_segmentation(img, seg, L, conn)
%
% seg_new = fill_in_segmentation(img, seg, <L>, <conn>)
%
%  Fills in the unlabeled pixels of a segmentation 'seg'.  Those pixels
%  that are pixels with a segmentation label L do not belong to any
%  valid region.  Fill them in by associating them with the neighboring
%  region with the most similar color (i.e. by Euclidean distance).  
%  By default, it is assumed seg contains labels 1:N and that L=0. 
%  (As is the case with a watershed segmentation.)
%
%  Connectivity is specified by 'conn'.  Default is 8, but 4 can also be
%  used.
%

if(nargin<4 || isempty(conn))
    conn = 8;
    if(nargin<3 || isempty(L))
        L = 0;
    end
end

[nrows,ncols,nchannels] = size(img);

% if(nchannels==3)
%     % Convert to Lab space
%     img = RGB2Lab(img);
% end

% Pad everything by one pixel around the borders so we can fix the
% unlabeled pixels on the original image borders
seg = zeropad(seg);
img = zeropad(img);
[nrows,ncols] = size(seg);

seg_new = seg;

% Create a lookup for the borders of the image so we don't try to use them
% (since neighbor lookups will fail there)
image_boundaries = false(nrows,ncols);
image_boundaries([1 end],:) = true;
image_boundaries(:,[1 end]) = true;

% Get the indices of unlabeled pixels that are not on the border of the
% image
L_index = find(seg==L & ~image_boundaries);
num_unlabeled = length(L_index);

switch(conn)
    
    case(8)
        % Offsets that move up, down, left, right, upleft, upright, downleft, and
        % downright in the image
        neighbor_offset = [-1 1 -nrows nrows -1-nrows -1+nrows 1-nrows 1+nrows];
    case(4)
        % Offsets that move up, down, left, right
        neighbor_offset = [-1 1 -nrows nrows];
    otherwise
        error('Only 8- and 4-connectivity recognized.');
end


% Indices of a 3x3 neighborhood around each of the watershed boundary
% pixels
win_index = L_index(:)*ones(1,conn) + ones(num_unlabeled,1)*neighbor_offset;

% set neighbors that are also in the seg=0 set to inf distance so they
% won't be chosen:
to_delete = find(seg(win_index)==0);

% In case we have a multi-channel image, add indices for the additional
% channels
b_index_orig = L_index;
if(nchannels>1)
    for(c = 2:nchannels)
        L_index = cat(3, L_index, L_index(:,:,c-1) + nrows*ncols);
        win_index = cat(3, win_index, win_index(:,:,c-1) + nrows*ncols);
    end
end

% Find the L2 color distance between the unlabeled pixels and
% their neighbors, setting the distance of those neighbors that are also
% unlabeled to infinite.
d = sum( (repmat(img(L_index), [1 conn]) - img(win_index)).^2, 3 );
d(to_delete) = inf;

% For each unlabeled pixel, find the neighbor with the most 
% similar color and put its segmentation region number in that
% pixel's position in the new segmentation.
[min_d, which_neighbor] = min(d, [], 2);
neighbor_index = (1:length(which_neighbor))' + (which_neighbor-1)*num_unlabeled;
win_index = win_index(:,:,1);
seg_new(b_index_orig) = seg(win_index(neighbor_index));

% Remove the extra padding
seg_new = seg_new(2:end-1,2:end-1);

if(nargout==0)
    seg = seg(2:end-1,2:end-1);
    
    figure
    subplot 121, imagesc(seg), axis image
    title('Original Segmentation')
    
    subplot 122, imagesc(seg_new), axis image
    title('Filled-In Segmentation')
end


function B = zeropad(A)

[nrows,ncols,nchannels] = size(A);
B = zeros(nrows+2,ncols+2,nchannels, class(A));
B(2:end-1,2:end-1,:) = A;
