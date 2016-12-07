function [labels] = random_walker_superpixels(image, region_height, region_width, beta)

    %Read image
    img = im2double(image);
    [height, width, channels] = size(img);
    
    seeds = [];
    seed_labels = [];
    
    label = 1;
    i = floor(region_height/2);
    while i < height
        
        j = floor(region_width/2);
        while j < width
            seeds = [seeds, sub2ind([height, width], i, j)];
            seed_labels = [seed_labels, label];
            
            label = label + 1;
            j = j + region_width;
        end;
        
        i = i + region_height;
    end;
    
    %Apply the random walker algorithms
    [labels, ~] = random_walker(img, seeds, seed_labels, beta);
end

