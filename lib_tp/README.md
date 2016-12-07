Instalation and running instructions for superpixel code
--------------------------------------------------------

DISCLAIMER: 
This is a beta version of the superpixel code. It hasn't been
thoroughly tested on different architectures. It was tested on
a Linux system with Matlab 7.1, with gcc and g++ compilers.
The affinity function that is used in the code is simple and 
is not sufficient to provide good results on all possible images. 
The domain of the images affects the choice of the affinity 
function.

This file contains instructions on running the superpixel code.

Instalation:
------------

- Put all the code into a directory (for example 'my_dir')

- Run 'cd my_dir'

- Start matlab

- The subdirectory 'lsmlib' needs to be added to matlab's path. This is done
in demo_superpixels.m, or you can do it manually by typing 'addpath('lsmlib')'

- Run 'make'. To work, MEX needs to be properly configured. Ignore the warnings.

- Load an image 'img = im2double(imread(image_filename))'. This ensures that 
the loaded image is in double format with every component in 0..1 range. Another
method for loading images can be used as long as the resulting image is a 2D
array of doubles.
The image can be RGB or grayscale, although it only matters for displaying purposes.

- Run the superpixels() function in superpixels.m. The header describes the
different parameters.

- demo_superpixels.m gives one example of using the code.

Examples:

1) Getting 1500 superpixels without intermediate display

img = im2double(imread('lizard.jpg'));
[phi,boundary,disp_img] = superpixels(img, 1500);
imagesc(disp_img);

2) Getting superpixels with an intermediate display every 5 timesteps

img = im2double(imread('lizard.jpg'));
[phi,boundary,disp_img] = superpixels(img, 1500, 5);
imagesc(disp_img);

3) Getting superpixels while saving every frame of the evolution into a movie

img = im2double(imread('lizard.jpg'));
[phi,boundary,disp_img, frames] = superpixels(img, 1500, 1);
disp('Press any key to view the evolution movie');
pause;
movie(frames);


