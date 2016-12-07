% Evolves the height function one time step.

function [new_phi,boundary_speed] = evolve_height_function(phi, speed, boundary_speed, time_step, speed_type, band_ind, background_init)

    if (isscalar(band_ind))
        band_ind = logical(ones(size(phi)));
    end
    
    if (strcmp(speed_type,'curvature'))
        delta_phi = height_function_change_rate(phi);
        boundary_speed = [];
    else
        if (isempty(boundary_speed))
            boundary_speed = get_speed_based_on_boundaries(phi,background_init);
        end
        speed = speed .* boundary_speed;
        
        delta_phi = height_function_grad(phi, speed);
    end
    
    ofs = 1;
    new_phi = phi;
    new_phi(band_ind) = phi(band_ind) - time_step * delta_phi(band_ind);
    new_phi = padarray(new_phi(1+ofs:end-ofs,1+ofs:end-ofs),[ofs,ofs],'replicate');