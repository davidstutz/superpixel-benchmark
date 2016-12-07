% Displays the superpixel contours of level-set function phi overlaid over
% the image (img). If display_boundaries=1, one pixel wide boundaries will
% be displayed, and zero crossings otherwise.

function disp_img = display_contour(phi, img, display_boundaries, speed, color, brightness)
    if (nargin < 3 || isempty(display_boundaries))
        display_boundaries = 0;
    end
    
    if (nargin < 5 || isempty(color))
        color = [1,0,0];
    end

    if (nargin == 6)
        brightness = min(1, max(0, brightness));
        gamma = 1-brightness;
        img = img .^ gamma;
    end
    
    % Choose whether to display one pixel wide boundaries or skeletonized
    % zero crossings
    if (display_boundaries)
        if (nargin < 4 || isempty(speed))
            contour = get_superpixel_boundaries(phi);
        else
            contour = get_superpixel_boundaries(phi,speed);
        end
    else
        contour = zero_crossing(phi);
    end
    
    if (size(img,3) == 1)
        disp_img = repmat(img,[1,1,3]);
    else
        disp_img = img;
    end
    
    disp_img = display_logical(disp_img, contour, color); 
        
    if (nargout == 0)
        imagesc(disp_img); axis off;
    end

