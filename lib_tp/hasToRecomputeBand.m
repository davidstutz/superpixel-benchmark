function flag = hasToRecomputeBand(phi,band, max_band_size)
    flag = (isscalar(band) || any(band(zero_crossing(phi)) > (max_band_size - 2)));