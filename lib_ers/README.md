/********************************************************************
*			Entropy Rate Superpixel Segmentation code   *
*								    *	
*	Ming-Yu Liu (UMD), Oncel Tuzel (MERL),                      *
*	Srikumar Ramalingam (c) and Rama Chellappa (UMD)	    *
*     						                    *
/********************************************************************

/* Copyright 2011, Ming-Yu Liu

All Rights Reserved 

Permission to use, copy, modify, and distribute this software and 
its documentation for any non-commercial purpose is hereby granted 
without fee, provided that the above copyright notice appear in 
all copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
the author not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior 
permission. 

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
*/

/********************************************************************
* 						General information *
/********************************************************************
Thanks for your interest in our work. This is a MATLAB wrapper for the
Entropy Rate Superpixel Segmentation algorithm. For more detail about 
the algorithm please refer to the project webpage 
http://www.umiacs.umd.edu/~mingyliu 
or the following paper

	  Ming-Yu Liu, Oncel Tuzel, Srikumar Ramalingam, and Rama Chellappa 
	  "Entropy Rate Superpixel Segmentation" Proceedings of the IEEE 
	  Conference on Computer Vision and Pattern Recognition(CVPR), 
	  Colorado Spring, June 2011. 
		  
If you use this software, you should cite 
the aforementioned paper in any resulting publication.

If you have any questions regarding the code please contact 
Ming-Yu Liu via <mingyliu AT umiacs DOT umd DOT edu>.

/********************************************************************
* 			Installation and usage			    *
/********************************************************************

1) Unzipped the files to $HOME_DIR   

2) We have provided pre-compiled mex functions. You may not need to 
recompile the mex files again. If you need to recompile the codes. 
In the matlab command prompt, type >>make. This should do the job.

3) Type demo to see a demonstration of the usage of the code.

mex_ers.mex : compute the superpixel segmentation for an input image.
usage:
	[labels] = mex_ers(image,nC);
	[labels] = mex_ers(image,nC,lambda,sigma);
	[labels] = mex_ers(image,nC,lambda,sigma,conn8);
Input: 
	image  : the input grey scale image or color image
	nC     : the number of desired superpixels
	lambda : the balancing parameter ( default = 0.5; )
	sigma  : the kernel bandwidth  ( default = 5.0; )
	conn8  : the flag of using 8-connected grid graph structure ( default = 1 )
	         if set to 0, the algorithm will use 4-connected graph instead.
Output: 
	labels : the output image. Each pixel is labeled with the label 
	         of the superpixel it belongs to. Note that the labeling
			 begins from 0 to nC-1
			 
/********************************************************************
* 			  Revision History:			    *
/********************************************************************
August 26, 2011; - Fixed two bugs causing compiling errors in Linux environment.
August 11, 2011; - Enable the input of color images.
August 11, 2011; - Enable the use of 4-connected grid graph structure.
July 1, 2011; - First version for the general public

