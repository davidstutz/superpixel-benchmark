/************************************************************************

   File:		2D_DT_Library.c

   Author(s):		Alexander Vasilevskiy

   Created:		24 May 2000

   Last Revision:	$Date$

   Description:	

   $Revision$

   $Log$

	
   Copyright (c) 2000 by Alexander Vasilevskiy, Centre for Intelligent Machines,
   McGill University, Montreal, QC.  Please see the copyright notice
   included in this distribution for full details.

 ***********************************************************************/
#include <stdio.h>
#include <math.h>
#include "mex.h"

#define INFINITY 999999

// detects edges between black and white regions
void EdgeDetect(double *in,double *out,int WIDTH,int HEIGHT){
int x,y;
 for(y=0; y <HEIGHT; y++) {
   for(x=0; x<WIDTH;  x++) {
     if ((x==0)||(y==0)||(y==HEIGHT-1)||(x==WIDTH-1)) out[y*WIDTH + x]=INFINITY;
     else
       if ( (in[y*WIDTH + x]!=INFINITY)&&
	    ((in[(y-1)*WIDTH + x]==INFINITY)||
	     (in[(y+1)*WIDTH + x]==INFINITY)||
	     
	     (in[y*WIDTH + x+1]==INFINITY)||
	     (in[y*WIDTH + x-1]==INFINITY)))  out[y*WIDTH + x]=0; else out[y*WIDTH + x]=INFINITY;  
      
   }
 }
}

// assignes sign to distance transform
void PutSign(double *in,double *out,int WIDTH,int HEIGHT){
    for(int j=0; j<HEIGHT; j++) 
      for(int i=0; i<WIDTH; i++) 
	if (in[j*WIDTH + i] == INFINITY) out[j*WIDTH + i]=-out[j*WIDTH + i];
}


double MINforward(double *in,double d1,double d2,int i,int j,int WIDTH,int HEIGHT){
  double mask[5];
  double min=INFINITY;
  int k;
  if ((i>0)&&(j>0)) mask[0]=in[(j-1)*WIDTH + (i-1)]+d2; else mask[0]=INFINITY;
  if (j>0) mask[1]=in[(j-1)*WIDTH + i]+d1; else mask[1]=INFINITY;
  if ((i<WIDTH-1)&&(j>0)) mask[2]=in[(j-1)*WIDTH + i+1]+d2; else mask[2]=INFINITY;
  mask[3]=in[j*WIDTH + i];
  if (i>0) mask[4]=in[j*WIDTH + i-1]+d1; else mask[4]=INFINITY;
  for(k=0;k<5;k++) if (mask[k]<min) min=mask[k];
  return min;
}
double MINbackward(double *in,double d1,double d2,int i,int j,int WIDTH,int HEIGHT){
  double mask[5];
  double min=INFINITY;
  int k;
  mask[0]=in[j*WIDTH + i];
  if (i<WIDTH-1) mask[1]=in[j*WIDTH + i+1]+d1; else mask[1]=INFINITY;
  if ((j<HEIGHT-1)&&(i<WIDTH-1)) mask[2]=in[(j+1)*WIDTH + i+1]+d2; else mask[2]=INFINITY;
  if (j<HEIGHT-1) mask[3]=in[(j+1)*WIDTH + i]+d1; else mask[3]=INFINITY;
  if ((i>0)&&(j<HEIGHT-1)) mask[4]=in[(j+1)*WIDTH + i-1]+d2; else mask[4]=INFINITY;
  for(k=0;k<5;k++) if (mask[k]<min) min=mask[k];
  return min;
}

void nNeighbor(double *in,double d1,double d2,int WIDTH,int HEIGHT){
  int i,j;
 
  for (j=0;j<HEIGHT;j++) 
    for(i=0;i<WIDTH;i++) 
      in[j*WIDTH + i] = MINforward(in,d1,d2,i,j,WIDTH,HEIGHT);
  
  for (j=HEIGHT-1;j>-1;j--) 
    for(i=WIDTH-1;i>-1;i--) 
      in[j*WIDTH + i]=MINbackward(in,d1,d2,i,j,WIDTH,HEIGHT);
 
}
void ComputeDistanceTransform(double *in,double *out,int WIDTH,int HEIGHT) {
  
  EdgeDetect(in,out,WIDTH,HEIGHT);
  nNeighbor(out,1,1.351,WIDTH,HEIGHT);
  PutSign(in,out,WIDTH,HEIGHT);

}

void mexFunction(
    int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[])
{
  double *in, *out;
  int iWidth, iHeight;
  
  /* Check for proper number of arguments. */
  if (nrhs != 1) {
    mexErrMsgTxt("One input required.");
  } else if (nlhs > 1) {
    mexErrMsgTxt("Too many output arguments");
  }
  
  /* The input must be a noncomplex scalar double.*/
  iWidth = mxGetM(prhs[0]);
  iHeight = mxGetN(prhs[0]);

  /* Create matrix for the return argument. */
  plhs[0] = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);
  
  /* Assign pointers to each input and output. */
  in = mxGetPr(prhs[0]);
  out = mxGetPr(plhs[0]);
  
  ComputeDistanceTransform(in, out, iWidth, iHeight);
}
