# README

The original README of the SUNRGBD [1] dataset can be found below. The 
converted dataset will be made available if possible.

    [1] S. Song, S. P. Lichtenberg, J. Xiao.
        SUN RGB-D: A RGB-D scene understanding benchmark suite.
        IEEE Conference on Computer Vision and Pattern Recognition, 2015, pp. 567–576.

Instructions to convert the dataset manually can be found in `docs/DATASETS.md`.

## Original README

****************************************************************************************
Data: Image depth and label data are in SUNRGBD.zip
image: rgb image
depth: depth image  to read the depth see the code in SUNRGBDtoolbox/read3dPoints/.
extrinsics: the rotation matrix to align the point could with gravity
fullres: full resolution depth and rgb image
intrinsics.txt  : sensor intrinsic
scene.txt  : scene type
annotation2Dfinal  : 2D segmentation
annotation3Dfinal  : 3D bounding box
annotation3Dlayout : 3D room layout bounding box

****************************************************************************************
Label: 
In SUNRGBDtoolbox/Metadata 
SUNRGBDMeta.mat:  2D,3D bounding box ground truth and image information for each frame.
SUNRGBD2Dseg.mat:  2D segmetation ground truth. 
The index in "SUNRGBD2Dseg(imageId).seglabelall"  mapping the name to "seglistall". 
The index in "SUNRGBD2Dseg(imageId).seglabel" are mapping the object name in "seg37list".
 
****************************************************************************************
In SUNRGBDtoolbox/traintestsplit
allsplit.mat: stores the training and testing split.

****************************************************************************************
Code:
SUNRGBDtoolbox/demo.m : Examples to load and visualize the data.
SUNRGBDtoolbox/readframeSUNRGBD.m : Example code to read SUNRGBD annotation from ".json" file.
Some of the code are modified base on RMRC 3D detection challenge. 
http://ttic.uchicago.edu/~rurtasun/rmrc/indoor.php
*****************************************************************************************


Citation:
Please cite our paper if you use this data:
S. Song, S. Lichtenberg, and J. Xiao.
SUN RGB-D: A RGB-D Scene Understanding Benchmark Suite
Proceedings of 28th IEEE Conference on Computer Vision and Pattern Recognition (CVPR2015)

The dataset contains RGB-D images from NYU depth v2 [1], Berkeley B3DO [2], and SUN3D [3]. Besides this paper, you are required to also cite the following papers if you use this dataset:
[1]N. Silberman, D. Hoiem, P. Kohli, R. Fergus. Indoor segmentation and support inference from rgbd images. In ECCV, 2012.
[2] A. Janoch, S. Karayev, Y. Jia, J. T. Barron, M. Fritz, K. Saenko, and T. Darrell. A category-level 3-d object dataset: Putting the kinect to work. In ICCV Workshop on Consumer Depth Cameras for Computer Vision, 2011.
[3] J. Xiao, A. Owens, and A. Torralba. SUN3D: A database of big spaces reconstructed using SfM and object labels. In
ICCV, 2013
