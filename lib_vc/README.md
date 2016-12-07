VCells.cpp by Jie Wang (jiewangustc@gmail.com)
   This code implements the basic functional of VCells algorithms described 
in "VCells: Simple and Efficient Superpixels Using Edge-Weighted Centroidal 
Voronoi Tessellations, TPAMI 34(6), 1241-1247" by Jie Wang and Xiaoqiang Wang.
The detailed technical discussions can be found in "An edge-weighted centroidal 
Voronoi tessellation model for image segmentation, 
Image Processing, IEEE Transactions on 18 (8), 1844-1858" by Jie Wang, Lili Ju 
and Xiaoqiang Wang. 

                      DISCLAIMER:  
   This code is for academic (non-commercial) use only.  Moreover, the code 
comes with absolutely NO warranty of any kind. I take no responsibility for 
the reliability of the results.

                      PLATFORM
   The code is tested under Visual Studio 2010 with Window 7 OS.

                      HOW TO USE THIS CODE
   The code currently only supports .bmp files. You can specify the image 
file name in the main function. You can adjust the key parameters in lines 
43-46. We provide an example image "127.bmp", which is taken from the "Berkeley 
Segmentation Dataset", and the corresponding results. By default, we use 
the Lab color space. Have fun.