% Displays the logical mask contour overlaid on image (img) with the given
% color (3D vector) for displaying the mask.

function disp_img = display_logical(img, contour, color)
    
    if (size(img,3) == 1)
        img = repmat(img,[1,1,3]);
    end
    disp_img = img;
    
    ind = logical(zeros(size(img,1),size(img,2),3));
    ind(:,:,1) = contour;
    disp_img(ind) = color(1);
    ind = logical(zeros(size(img,1),size(img,2),3));
    ind(:,:,2) = contour;
    disp_img(ind) = color(2);
    ind = logical(zeros(size(img,1),size(img,2),3));
    ind(:,:,3) = contour;
    disp_img(ind) = color(3);
    disp_img = disp_img ./ max(disp_img(:));
    disp_img = uint8(disp_img * 255);
