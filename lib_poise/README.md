Superpixel merging code used in:

Ahmad Humayun, Fuxin Li, James M. Rehg. The Middle Child Problem: Revisiting Parametric Min-cut for Robust Object Proposals. ICCV 2015. 

The code is optimized for Piotr Dollar's latest StructEdges package which can be found at https://github.com/pdollar/edges. If you want to use other boundary detection algorithms, please go inside merge_superpix_mex.m and change the boundary detection algorithm to the appropriate one that you'll need.

If you use the code in your research or application, please cite the paper:

Ahmad Humayun, Fuxin Li, James M. Rehg. The Middle Child Problem: Revisiting Parametric Min-cut for Robust Object Proposals. ICCV 2015. 

In linux and Mac, please compile the mex files by:

mex intens_pixel_diff_mex.c
mex prctile_feats.cpp
mex merge_superpixels2.cpp

Windows DLL files are provided.

The simplest way to call the code is:

sp_seg = merge_superpix_mex(I, model, desired_sups)

where I is the image, and desired_sups is the desired number of superpixels. Please load the models/forest/modelBsds.mat and feed it as the "model" parameter to the code.

There are some more parameters which usually don't need to be set, details can be found in the m-file itself.

Copyright 2015 Georgia Institute of Technology.

Fuxin Li, Ahmad Humayun.

Contact: lif@eecs.oregonstate.edu