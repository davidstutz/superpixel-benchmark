% Get the height derivative with respect to time assuming curvature motion
function delta_phi = height_function_change_rate(phi)

    [dx,dy,dxx,dyy,dxy] = height_function_der(phi);
    
    delta_phi = -(dxx.*(dy.^2) - 2*dx.*dy.*dxy + dyy.*(dx.^2)) ./ ...
                (eps+(dx.^2 + dy.^2));