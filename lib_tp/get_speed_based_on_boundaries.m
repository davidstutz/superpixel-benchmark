function speed = get_speed_based_on_boundaries(phi, background_init)

    background = (phi>=0 | zero_crossing(phi));
    mask = double(background & background_init) - 0.5;
    phi = computeDistanceFunction2d(phi,[1,1],mask);
    
    skeleton = doHomotopicThinning(phi, mask);

    speed = ones(size(phi));
    speed(skeleton) = 0;
