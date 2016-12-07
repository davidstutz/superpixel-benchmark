ERGC library now consists in a single ergc.h file.
It contains all the functions to perform superpixel/voxels computation.
"example_ergc.cpp" and "example_refinement.cpp" show how to call the functions.
However, "example_ergc.cpp" can be used as a standalone in your own image processing pipeline.


"src" directory :
	  - example_ergc.cpp : sample code that computes superpixels/supervoxels for images/volumes of any spectrum
	  - example_refinement.cpp : sample code that shows how to use iteratively refinement functions
	  - ergc.h : includes all the functions to perform diffusion/superpixels computation
	  - Heap*.h : files to manage the heap within the fast marching algorithm 
	  - CImg.h : main CImg library file
	  - timer.h : time processing calculation utility file


"images" directory contains two images extracted from the Berkeley dataset

====================================================================================

Note on the timer.h file
=> provides a timer that works in a matlab fashion
a call to the "toc()" function returns the time (in seconds, double format) since the last call to "tic()"
This may be UNIX only ..


====================================================================================

COMPILATION :
some libraries are needed to compile ERGC
     - CImg (included in src directory)
     - X11 to display some images
     - LAPACK

To compile, just type in a shell
$ cmake .
$ make

Note: the compilation may be time consuming since ERGC relies on CImg library which consists in a single (big) .h file


====================================================================================
USAGE
====================================================================================
All binaries have the -h option
=======================================
USAGE for "example_ergc"

"example_ergc" has several options and may produce different outputs:
option -i : 
       input image file (default: "")
       can be all the image types supported by CImg

option -n : 
       desired number of superpixels (default: 400)
       default behavior of the application
       Almost n seeds are placed on a regular grid (same step size along each axis)

option -dx : 
       Seeds sampling wrt axis x (for custom grids, default: not used)
       if specified, -n is ignored
option -dy : 
       Seeds sampling wrt axis y (for custom grids, default: not used)
       if specified, -n is ignored
option -dz : 
       Seeds sampling wrt axis z (for custom grids, default: not used)
       if specified, -n is ignored

option -is : 
       Input seeds image (default: none)
       A custom seeds image can be given
       -n, -dx, -dy and -dz are ignored
       ! parameter m should be set to 0 !
       can be all the image types supported by CImg
       background pixels (not seeds) must be set to -1
       seeds must be numbered from 0 to n

option -p : 
       pertub grid seeds wrt a perturb map (default: true)
       by default gradient map used as perturb map
       only works with grid seeds
       
option -rgb2lab : 
       Computes superpixels/voxels in Lab colorspace (for RGB input image only, default: true)
       option ignored if the number of channels of im is not equal to 3

option -m : 
       compactness factor - m value of the paper (default: 0)
       see paper
       its value has to be fixed depending on the range of the input image pixel values ..

option -ol : 
       Outputs the Labeled region map (default: "")
       main output for many applications

option -od : 
       Outputs the output geodesic Distance map (default: "")

option -osp : 
       Outputs the SuperPixels image (default: "")
       all the pixels of a superpixel are set to the mean value of it

option -ospo : 
       Outputs the resulting SuperPixels with the Original image as background (default: "")

option -v : verbose (default: 0)
       display the number of seeds (equal to the number of final superpixels)

examples : 
./bin/example_ergc -i images/118035.jpg -n 150 -ol output/lab.png
=> computes 150 superpixels on 118035.jpg image and outputs labeled region map only 

./bin/example_ergc -i images/118035.jpg -n 600 -m 10 -ol output/lab.png -od output/distance.cimg -osp output/means.bmp -ospo output/ospo.jpg
=> computes 600 superpixels with a spatial constraint value of 10 and outputs
   - the labeled region map in a png file
   - the distance map in a cimg file (format of CImg library)
   - the superpixels image in a bmp file
   - the superpixels image with original image as baskground in a jpg file

./bin/example_ergc -i images/118035.jpg -is your.own.seed.image -ol output/lab.png
=> computes the diffusion according to your.own.seed.image and outputs labeled region map only 
   - labels are set according to your.own.seed.image

./bin/example_ergc -i images/118035.jpg -dx 50 -dy 5 -m 10 -rgb2lab false -p false -ol output/lab.png
=> computes the diffusion according to a custom grid seed map
   - sampling step: 50 according to x axis
   - sampling step: 5 according to y axis
   - do not convert to Lab (processing directly in RGB colorspace)
   - do not perturb seeds

=======================================
USAGE for "example_refinement"
option -i :
	same as above
option -n :
	same as above
option -nr :
       number of refinement steps
       each refinement step adds a new seeds
option -rgb2lab :
	same as above
option -m :
	same as above
option -v :
	same as above
options -ol -od -osp -ospo : 
	same as above


Note: the refinement function has not been extensively tested with volumes, so bugs can appear with these datas.


====================================================================================


For any suggestion, or bug report, please send an email to :
pierre.buyssens@gmail.com

Stay tuned on https://sites.google.com/site/pierrebuyssens/ergc for incoming updates of ERGC 
