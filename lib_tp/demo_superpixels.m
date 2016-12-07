% Runs the superpixel code on the lizard image

addpath('lsmlib');
img = im2double(imread('lizard.jpg'));
[phi,boundary,disp_img] = superpixels(img, 10000);
csvwrite('lizard.csv', boundary);
imshow(boundary)
% imagesc(disp_img);