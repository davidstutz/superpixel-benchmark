# README

The original README of the NYUV2 [1] dataset can be found below. The 
converted dataset will be made available if possible.

    [1] N. Silberman, D. Hoiem, P. Kohli, R. Fergus.
        Indoor segmentation and support inference from RGBD images.
        European Conference on Computer Vision, 2012, pp. 746–760.

Instructions to convert the dataset manually can be found in `docs/DATASETS.md`.

## Original README

NYU Depth V2 Dataset Matlab Toolbox
Authors: Nathan Silberman, Pushmeet Kohli, Derek Hoiem, Rob Fergus

*************************
RETRIEVING THE ACCEL DATA
*************************
To compile the get_accel_data function, you must alter compile.m with the
absolute location of the libFreenect include directory. Once compile,
get_accel_data.mex* can be called to extract the accelerometer data from the 
*.a files in each scene dump directory.

