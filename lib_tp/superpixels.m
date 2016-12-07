% Compute the superpixels given an image.
% img - the input image. Either RGB or Grayscale. Must be double in range
% 0..1
% numSuperpixels - number of superpixels
% display_int - Interval of frames at which the progress of the evolution
% will be displayed. 0 if not display is needed (default).
% contour_color - color of the superpixel boundaries (default is red)
%
% Returns:
%     phi - the final evolved height function
%     boundary - a logical array representing superpixel boundaries
%     disp_img - the image with the boundaries overlaid on top of the image
%     frames - frames of the evolution in Matlab movie format if
%     display_int was above 0 and output frames parameter was given.
function [phi,boundary,disp_img,frames] = superpixels(img, numSuperpixels, timeStep, maxIterations, normSigmaFactor, display_int, contour_color)
    
    if (nargin < 3)
        timeStep = 0.5;
    end
    
    if (nargin < 4)
        maxIterations = 500;
    end

    if (nargin < 5)
        normSigmaFactor = 2.5;
    end   
            
    if (nargin < 6 || isempty(display_int))
        display_int = 0;
    end
    
    if (nargin < 7 || isempty(contour_color))
        contour_color = [1,0,0];
    end
    
   
    
    if (nargout < 4)
        phi = evolve_height_function_N(img, timeStep, maxIterations, 'superpixels', display_int, [], numSuperpixels, normSigmaFactor);
    else
        [phi,frames] = evolve_height_function_N(img, timeStep, maxIterations, 'superpixels', display_int, [], numSuperpixels, normSigmaFactor);
    end
    
    if (size(img,3) > 1)
        smooth_img = evolve_height_function_N(rgb2gray(img), 0.1, 10, 'curvature', 0, 0);
    else
        smooth_img = evolve_height_function_N(img, 0.1, 10, 'curvature', 0, 0);
    end

    [gx,gy] = height_function_der(255*smooth_img);
    mag = sqrt(gx.^2 + gy.^2);
    speed2 = exp(- mag/5);
    boundary = get_superpixel_boundaries(phi,speed2);
    disp_img = display_logical(img, boundary, contour_color);
    