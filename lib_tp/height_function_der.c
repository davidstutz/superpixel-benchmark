#include "mex.h"

void mexFunction(
    int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[])
{
  double *phi, *dx, *dy, *dxx, *dyy, *dxy;
  int iWidth, iHeight;
  
  /* The input must be a noncomplex scalar double.*/
  iHeight = mxGetM(prhs[0]);
  iWidth = mxGetN(prhs[0]);

  /* Create matrix for the return argument. */
  plhs[0] = mxCreateDoubleMatrix(iHeight,iWidth, mxREAL);
  plhs[1] = mxCreateDoubleMatrix(iHeight,iWidth, mxREAL);
  plhs[2] = mxCreateDoubleMatrix(iHeight,iWidth, mxREAL);
  plhs[3] = mxCreateDoubleMatrix(iHeight,iWidth, mxREAL);
  plhs[4] = mxCreateDoubleMatrix(iHeight,iWidth, mxREAL);
  
  /* Assign pointers to each input and output. */
  phi = mxGetPr(prhs[0]);
  dx = mxGetPr(plhs[0]);
  dy = mxGetPr(plhs[1]);
  dxx = mxGetPr(plhs[2]);
  dyy = mxGetPr(plhs[3]);
  dxy = mxGetPr(plhs[4]);
  
  for (int i = 1; i < iWidth - 1; i++)
  {
    for (int j = 1; j < iHeight - 1; j++)
    {
      dx[i*iHeight+j] = (phi[(i+1)*iHeight+j] - phi[(i-1)*iHeight+j]) / 2;
      dy[i*iHeight+j] = (phi[i*iHeight+j+1] - phi[i*iHeight+j-1]) / 2;
      dxx[i*iHeight+j] = phi[(i+1)*iHeight+j] - 2*phi[i*iHeight+j] + phi[(i-1)*iHeight+j];
      dyy[i*iHeight+j] = phi[i*iHeight+j+1] - 2*phi[i*iHeight+j] + phi[i*iHeight+j-1];
      dxy[i*iHeight+j] = (phi[(i+1)*iHeight+j+1] + phi[(i-1)*iHeight+j-1] - 
                          phi[(i-1)*iHeight+j+1] - phi[(i+1)*iHeight+j-1]) / 4;
    }
  }
}
