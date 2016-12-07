% Return the thinned superpixel boundaries

function boundaries = get_superpixel_boundaries(phi,speed)

    smallAreaThresh = 2e-5;
    
    if (islogical(phi))
        boundaries = bwmorph(bwmorph(phi,'skel',Inf),'spur',Inf);
    else
        contour = zero_crossing(phi);
        background = (phi>=0 | contour);
        background_clean = bwmorph(bwmorph(background,'clean',Inf),'spur',Inf);                

        mask = double(background_clean) - 0.5;
        phi = computeDistanceFunction2d(phi,[1,1],mask);
        skeleton = doHomotopicThinning(phi, mask);
        
        % inside is the original superpixels plus the holes
        inside = (~background_clean | (background_clean & ~contour));
        
        % Fill narrow holes
        near_background = imfilter(double(~inside),ones(3,3)) > 0;
        near_skeleton = imfilter(double(skeleton),ones(3,3)) > 0;
        inside(near_skeleton & near_background) = 0;
        
        L = bwlabel(inside,4);
        stats = regionprops(L,'Area');

        
        % Set small regions to be the background, so that they would be
        % thinned
        bad_superpixel = [stats(:).Area] < (smallAreaThresh * prod(size(phi)));

        for i=1:length(stats)
            if (bad_superpixel(i))
                L(L==i) = 0;
            end
        end
        inside = (L>0);

        phi = computeDistanceFunction2d(double(~inside)-0.5,[1,1]);
        distance_ordering = phi;
        
        if (nargin > 1)            
            distance_ordering = (1-speed.^0.25).*(1-speed) + (speed.^0.25).*(phi./max(phi(:)));
        end

        contour = zero_crossing(phi);
        background = (phi>=0 | contour);
        boundaries = bwmorph((doHomotopicThinning(distance_ordering,double(background)-0.5)),'clean', Inf);
        erode_kernel = [0,1,0; 1,0,1; 0,1,0];
        eroded_boundaries = imerode(boundaries, erode_kernel);
        boundaries(1,:) = eroded_boundaries(1,:);
        boundaries(:,1) = eroded_boundaries(:,1);
        boundaries(end,:) = eroded_boundaries(end,:);
        boundaries(:,end) = eroded_boundaries(:,end);
        boundaries = bwmorph(boundaries,'clean');
    end
