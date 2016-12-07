/*
Copyright 2011, Ming-Yu Liu

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

// Entropy Rate Superpixel Segmentation
#include "mex.h"
#include "matrix.h"
#include "MERCLazyGreedy.h"
#include "MERCInputImage.h"
#include "MERCOutputImage.h"

#include "Image.h"
#include "ImageIO.h"
#include <iostream>
#include <string>

using namespace std;

void mexFunction(int nlhs, mxArray *plhs[ ],int nrhs, const mxArray *prhs[ ]) 
{
	double lambda,sigma;
	int nC,kernel = 0;
	int row,col;
	int conn8;
	double *pLambda,*pSigma,*pNC;	
	double *data;
	double *out;
	double *pConn8;
	//size_t width,height;
	MERCLazyGreedy merc;	
	
	mexPrintf("Entropy Rate Superpixel Segmentation Version 0.2!!!\n");
	
	if(!(nrhs==2||nrhs==4||nrhs==5))
	{
		mexPrintf("Syntax Error!!!\n");
		mexPrintf("[labels] = mex_ers(image,nC)\n");
		mexPrintf("[labels] = mex_ers(image,nC,lambda,sigma)\n");
		mexErrMsgTxt("[labels] = mex_ers(image,nC,lambda,sigma,conn8)\n");
	}
	
    //if(nlhs > 1)
	//{
	//	mexErrMsgTxt("Too many output arguments.");
    //}
	
    /* Check data type of input argument  */
    if (!(mxIsDouble(prhs[0])))
	{
		mexErrMsgTxt("Input argument must be of type double.");
    }		

	//width  = mxGetN(prhs[0]);
	//height = mxGetM(prhs[0]);
	data   = mxGetPr(prhs[0]);

	if(nrhs==2)
	{
		pNC     = mxGetPr(prhs[1]);	
		lambda  = 0.5;
		sigma   = 5.0;
		conn8   = 1;
	}
	
	if(nrhs==4)
	{
		pNC     = mxGetPr(prhs[1]);	
		pLambda = mxGetPr(prhs[2]);
		pSigma  = mxGetPr(prhs[3]);
		lambda  = *pLambda;
		sigma   = *pSigma;
		conn8   = 1;
	}
	
	if(nrhs==5)
	{
		pNC     = mxGetPr(prhs[1]);	
		pLambda = mxGetPr(prhs[2]);
		pSigma  = mxGetPr(prhs[3]);
		pConn8  = mxGetPr(prhs[4]);
		lambda  = *pLambda;
		sigma   = *pSigma;
		conn8   = (int)(*pConn8);
	}
		
	nC = (int)(*pNC);
	
	
	int nDims = (int)mxGetNumberOfDimensions(prhs[0]);
	if(nDims == 3)
	{
		int width = mxGetN(prhs[0])/nDims;
		int height = mxGetM(prhs[0]);		
		//mexPrintf("Size = ( %d , %d ); Dimension = %d\n",width,height,nDims);
		Image<RGBMap> inputImage;
		MERCInputImage<RGBMap> input;
		// Create Iamge
		inputImage.Resize(width,height,false);
		// Read the image from MATLAB
		for (col=0; col < width; col++)
		{
			for (row=0; row < height; row++)
			{
				RGBMap color(
				(int)mxGetPr(prhs[0])[row+col*height+0*width*height],
				(int)mxGetPr(prhs[0])[row+col*height+1*width*height],
				(int)mxGetPr(prhs[0])[row+col*height+2*width*height]);
				inputImage.Access(col,row) = color;
				
			}
		}
		// Read the image for segmentation
		input.ReadImage(&inputImage,conn8);
		
		// Entropy rate superpixel segmentation
		merc.ClusteringTreeIF(input.nNodes_,input,kernel,sigma*nDims,lambda*1.0*nC,nC);
		vector<int> label = MERCOutputImage::DisjointSetToLabel(merc.disjointSet_);
		// Allocate memory for the labeled image.
		plhs[0] = mxCreateDoubleMatrix(height, width, mxREAL);
		out =  mxGetPr(plhs[0]);	
		// Fill in the labeled image
		for (col=0; col < width; col++)
			for (row=0; row < height; row++)
				out[row+col*height] = (double)label[col+row*width];		
	}
	else
	{
		int width = mxGetN(prhs[0]);
		int height = mxGetM(prhs[0]);			
		Image<uchar> inputImage;
		MERCInputImage<uchar> input;		
		// Create Iamge
		inputImage.Resize(width,height,false);
		// Read the image from MATLAB
		for (col=0; col < mxGetN(prhs[0]); col++)
			for (row=0; row < mxGetM(prhs[0]); row++)
				inputImage.Access(col,row) = (uchar)mxGetPr(prhs[0])[row+col*mxGetM(prhs[0])];
		// Read the image for segmentation
		input.ReadImage(&inputImage,conn8);
		
		// Entropy rate superpixel segmentation
		merc.ClusteringTreeIF(input.nNodes_,input,kernel,sigma,lambda*1.0*nC,nC);
		vector<int> label = MERCOutputImage::DisjointSetToLabel(merc.disjointSet_);
					
		// Allocate memory for the labeled image.
		plhs[0] = mxCreateDoubleMatrix(height, width, mxREAL);
		out =  mxGetPr(plhs[0]);	
		// Fill in the labeled image
		for (col=0; col < width; col++)
			for (row=0; row < height; row++)
				out[row+col*height] = (double)label[col+row*width];		
	}
	
    return;
}
