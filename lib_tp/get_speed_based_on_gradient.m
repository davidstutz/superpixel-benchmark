function [speed,speed_x,speed_y,doublet,curvature] = get_speed_based_on_gradient(img, doDoublet, normSigma, phi, speed, speed_x, speed_y)
    
    % Flag controlling relative vs. absolute gradient magnitude
    if (nargin < 2 || isempty(doDoublet))
        doDoublet = 0;
    end
    
    if (nargin < 3 || isempty(normSigma))
        normSigma = 5;
    end

    
    if (nargin < 5 || isempty(speed))
        [gx,gy] = height_function_der(255*img);
        mag = sqrt(gx.^2 + gy.^2);
        ss_mag = corrDn(mag, [1], 'repeat', [2 2]);
        stdev = normSigma;
        nTaps = round(3*stdev) * 2 + 1;
        lpImpResp = fspecial('gaussian',[1 nTaps],stdev);   % sample to 3 std devs
        % scale s.t. max value of impulse response is 1.0 (vs. sums to 1.0)
        lpImpResp = lpImpResp / max(lpImpResp);
        smooth_ssmag0 = imfilter(ss_mag, lpImpResp);
        smooth_ssmag = imfilter(smooth_ssmag0, lpImpResp');
        % upBlur  (check this since it looks like the respons
        % eis not as large as it should be.
        f = [0.5 1.0 0.5]';
        res = upConv(smooth_ssmag,f,'reflect1',[2 1]);
        smooth_mag = upConv(res,f','reflect1',[1 2]);
        % scale so that a long strgiht edge does not compete with itself
        % i.e. with contrast normalization, a long straight edge should
        % produce max(max(mag)) == max(max(smooth_nag))
        smooth_mag = smooth_mag / (sqrt(2*pi) * stdev);

        if (size(smooth_mag,1) ~= size(mag,1))
            smooth_mag = smooth_mag(1:end-1,:);
        end
        if (size(smooth_mag,2) ~= size(mag,2))
            smooth_mag = smooth_mag(:,1:end-1);
        end


        % normalized response s.t.
        %   - we control the gradient mag that is mapped to half height
        %   - the result is mapped to [0,127]
        magHalfHeight = 10.0;
        normGradMag = 127 * (mag ./ (magHalfHeight + smooth_mag));
        speed = exp(-normGradMag/10);

        [speed_x,speed_y] = height_function_der(speed);
    end
    
    if (doDoublet)
        [dx,dy,dxx,dyy,dxy] = height_function_der(phi);
        
        speed = get_full_speed(dx,dy,dxx,dyy,dxy,speed,speed_x,speed_y,1);
    end
