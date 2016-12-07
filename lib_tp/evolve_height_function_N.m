function [phi,frames,numIter,speed_grad] = evolve_height_function_N(img, time_step, num_iterations, speed_type, display_interval, boundary_speed_interval, numSuperPixels, normSigmaFactor)

    storeFrames = (nargout == 2);
    frames = [];
    frameWait = 0;
    boundary_speed = 1;
    frame.cdata = uint8(zeros(size(img,1),size(img,2),3));
    frame.colormap = [];
    band = 1;
    band_ind = 1;
    MAX_BAND_SIZE = 5;
    old_coveredArea = 0;
    alreadyPutSeeds = 0;
    relativeAreaInc = inf;
    background_init = [];
    stoppingFrames = 0;
    phi = [];
    
    % Need that many consecutive frames that satisfy the stopping condition to stop
    numStoppingFrames = 1;
    
    if (nargin < 6 || isempty(boundary_speed_interval))
        boundary_speed_interval = 6;
    end
    if (nargin < 7 || isempty(numSuperPixels))
        numSuperPixels = 200;
    end
    
    if (nargin < 8 || isempty(normSigmaFactor))
        normSigmaFactor = 2.5;
    end;
            
    img = clip(img, 0, 1);
    if (size(img,3) == 1)
        imRGB = repmat(img,[1,1,3]);
    else
        imRGB = img;
        img = rgb2gray(img);
    end
    
    smooth_img = 1;
    
    if (strcmp(speed_type,'superpixels'))
        % Compute the speed of propagation
        smooth_img = evolve_height_function_N(img, 0.1, 10, 'curvature', 0, 0);
        
        expSuperPixelDist = sqrt(prod(size(img))/numSuperPixels);
        normSigma = floor(expSuperPixelDist / normSigmaFactor);
        if normSigma <= 0
            normSigma = 1.0;
        end;
        
        [speed_grad,speed_grad_x,speed_grad_y] = get_speed_based_on_gradient(smooth_img,[],normSigma,phi);
        speed = speed_grad;

        % Place initial seeds and compute the initial signed distance
        % function
        seeds = get_initial_seeds(smooth_img, numSuperPixels, speed_grad);
        binary_img = get_binary_img_from_seeds(seeds,smooth_img);
        background_init = ~binary_img;
        input_dist = double(binary_img == 0);
        input_dist(input_dist == 0) = 999999;
        phi = DT(input_dist);
        phi = imfilter(phi, fspecial('gaussian',[3,3],0.5));
    else% Curvature flow
        phi = img;
        speed = 1;
    end

    if (display_interval > 0)
        if (storeFrames)
            disp_img = display_contour(phi, imRGB);
            pause(frameWait);
            frame.cdata = disp_img;
            frames = frame;
            f = 2;
        else
            display_contour(phi, imRGB);
            pause;
        end
    end
    
    for i = 1:num_iterations
              
        if (display_interval > 0)
            disp(['Iteration: ',num2str(i)]);
        end
        
        hasToRecomputeBandFlag = hasToRecomputeBand(phi, band, MAX_BAND_SIZE);
        
        % Extend the speed if needed
        if ((strcmp(speed_type,'gradient') || strcmp(speed_type,'superpixels')) && hasToRecomputeBandFlag)
            extension_fields = cell(3,1);
            extension_fields{1} = speed_grad;
            extension_fields{2} = speed_grad_x;
            extension_fields{3} = speed_grad_y;
            
            [fm_phi,fields] = computeExtensionFields2d(phi, extension_fields,[1,1],MAX_BAND_SIZE);
            speed_grad_extended = fields{1};
            speed_grad_x_extended = fields{2};
            speed_grad_y_extended = fields{3};
            band_ind = (fields{1} > 0 & abs(fm_phi) < (MAX_BAND_SIZE - 1));
            band = zeros(size(fm_phi));
            band(fields{1} > 0) = abs(fm_phi(fields{1} > 0));
            old_phi = phi;
            phi(fields{1} > 0) = fm_phi(fields{1} > 0);
            
            if (~isscalar(boundary_speed))
                phi(boundary_speed==0) = old_phi(boundary_speed==0);
            end
        end
        
        % Update the current speed of evolution
        if (strcmp(speed_type,'superpixels'))
            if (i < 20)
                speed = speed_grad_extended;
            else
                speed = get_speed_based_on_gradient(smooth_img,1,normSigma,phi,speed_grad_extended,speed_grad_x_extended,speed_grad_y_extended);
            end
        end

        % Evolve the height function one time step
        if (mod(i,boundary_speed_interval) == 1 && strcmp(speed_type, 'superpixels'))
            [phi,boundary_speed] = evolve_height_function(phi, speed, [], time_step, speed_type, band_ind, background_init);
        else
            [phi,boundary_speed] = evolve_height_function(phi, speed, boundary_speed, time_step, speed_type, band_ind);
        end
                
        % Stop based on the relative area increase
        coveredArea = sum(phi(:) < 0);
        relativeAreaInc = (coveredArea-old_coveredArea)/prod(size(phi));
        old_coveredArea = coveredArea;
        
        if (strcmp(speed_type, 'superpixels') && relativeAreaInc < 1e-4 && coveredArea / prod(size(phi)) > 0.5)
            stoppingFrames = stoppingFrames + 1;
        else
            stoppingFrames = 0;
        end
        
        if (stoppingFrames >= numStoppingFrames)
            numIter = i;
            break;
        end
        
        
        if (display_interval > 0 && mod(i,display_interval) == 0)
            
            if (storeFrames)
                disp_img = display_contour(phi, imRGB);
                frame.cdata = disp_img;
                frames(f) = frame;
                f = f+1;
                pause(frameWait);
            else
                display_contour(phi, imRGB);
                pause;
            end
        end
    end
    
    if (display_interval > 0)
        if (storeFrames)
            disp_img = display_contour(phi, imRGB);
            pause(frameWait);
            frame.cdata = disp_img;
            frames(f) = frame;
        else
            display_contour(phi, imRGB);
            pause;
        end
    end
    

    