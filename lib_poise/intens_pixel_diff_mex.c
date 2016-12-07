/* Copyright (C) 2010 Joao Carreira

 This code is part of the extended implementation of the paper:
 
 J. Carreira, C. Sminchisescu, Constrained Parametric Min-Cuts for Automatic Object Segmentation, IEEE CVPR 2010
 */

/*---
function [w] = intens_pixel_diff_mex(I, i, j)
Input:
    I - gray level image, must be double
    [i,j] = indices of pixels

Output:
    cw = real vector of the same size as ci, 
         absolute difference between pixel values at positions i and j

--*/

# include "mex.h"
# include "math.h"

void mexFunction(
  int nargout,
  mxArray *out[],
  int nargin,
  const mxArray *in[]) {

  /* declare variables */
  int nr, nc, np, a, total;
  unsigned int *i, *j;
  double *I;
  double *w;

  /* check argument */
  if (nargin<3) {
      mexErrMsgTxt("Three input arguments required");
  }
  if (nargout>1) {
      mexErrMsgTxt("Too many output arguments");
  }

  int diff_type = 0;
  if (nargin == 4) {
    double* diff_opt = mxGetData(in[3]);
    if (diff_opt[0] == 0) {
      /* L1 difference */
      diff_type = 0;
    } else {
      /* max (L-inf) */
      diff_type = 1;
    }
  }

  nr = mxGetM(in[0]);
  nc = mxGetN(in[0]);

  I = mxGetData(in[0]);

  /* get the ij-index pair */
  if (!mxIsUint32(in[1]) || !mxIsUint32(in[2])) {
      mexErrMsgTxt("Index pair shall be of type UINT32");
  }
  i = mxGetData(in[2]);
  j = mxGetData(in[1]);    
  np = mxGetM(in[2]) * mxGetN(in[2]);

  /* create output */
  total = mxGetM(in[1]) * mxGetN(in[1]);
  out[0] = mxCreateDoubleMatrix(total,1,mxREAL);
  if (out[0]==NULL) {
    mexErrMsgTxt("Not enough memory for the output vector");
  }
  w = mxGetPr(out[0]);

  /* computation, indexing is zero-based, in contrast to matlab */
  if (diff_type == 0) {
    for (a=0; a<np; a++) {
      w[a] = fabs(I[i[a]-1] - I[j[a]-1]);
    }
  } else {
    for (a=0; a<np; a++) {
      w[a] = I[i[a]-1] > I[j[a]-1] ? I[i[a]-1] : I[j[a]-1];
    }
  }
}
