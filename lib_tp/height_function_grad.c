#include <math.h>
#include "mex.h"

#define min( x, y ) ((x) > (y) ? (x) : (y))
#define max( x, y ) ((x) < (y) ? (x) : (y))

void mexFunction(
    int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[])
{
  //mxArray *dx_minus_array, *dx_plus_array, *dy_minus_array, *dy_plus_array, *grad_minus_array, *grad_plus_array;
  //double *phi, *dx_plus, *dx_minus, *dy_plus, *dy_minus, *grad_plus, *grad_minus, *grad, *speed;
  double *phi, dx_plus, dx_minus, dy_plus, dy_minus, grad_plus, grad_minus, *grad, *speed;
  int iWidth, iHeight;
  
  /* The input must be a noncomplex scalar double.*/
  iWidth = mxGetM(prhs[0]);
  iHeight = mxGetN(prhs[0]);

  /* Create matrix for the return argument. */
  plhs[0] = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);

  /*
  dx_minus_array = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);
  dx_plus_array = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);
  dy_minus_array = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);
  dy_plus_array = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);
  grad_minus_array = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);
  grad_plus_array = mxCreateDoubleMatrix(iWidth,iHeight, mxREAL);

  dx_plus = mxGetPr(dx_plus_array);
  dx_minus = mxGetPr(dx_minus_array);
  dy_plus = mxGetPr(dy_plus_array);
  dy_minus = mxGetPr(dy_minus_array);
  grad_plus = mxGetPr(grad_plus_array);
  grad_minus = mxGetPr(grad_minus_array);
  */
  
  /* Assign pointers to each input and output. */
  phi = mxGetPr(prhs[0]);
  speed = mxGetPr(prhs[1]);
  grad = mxGetPr(plhs[0]);
  
  for (int j = 1; j < iHeight-1; j++)
  {
    for (int i = 1; i < iWidth-1; i++)
    {
       /*
       dx_plus[j*iWidth+i] = (phi[j*iWidth+i+1] - phi[j*iWidth+i]);
       dy_plus[j*iWidth+i] = (phi[(j+1)*iWidth+i] - phi[j*iWidth+i]);
       dx_minus[j*iWidth+i] = (phi[j*iWidth+i] - phi[j*iWidth+i-1]);
       dy_minus[j*iWidth+i] = (phi[j*iWidth+i] - phi[(j-1)*iWidth+i]);


       grad_plus[j*iWidth+i] = sqrt(max(dx_minus[j*iWidth+i],0) * max(dx_minus[j*iWidth+i],0) +
                        min(dx_plus[j*iWidth+i],0) * min(dx_plus[j*iWidth+i],0) +
                        max(dy_minus[j*iWidth+i],0) * max(dy_minus[j*iWidth+i],0) +
                        min(dy_plus[j*iWidth+i],0) * min(dy_plus[j*iWidth+i],0));

       grad_minus[j*iWidth+i] = sqrt(min(dx_minus[j*iWidth+i],0) * min(dx_minus[j*iWidth+i],0) +
                         max(dx_plus[j*iWidth+i],0) * max(dx_plus[j*iWidth+i],0) +
                         min(dy_minus[j*iWidth+i],0) * min(dy_minus[j*iWidth+i],0) +
                         max(dy_plus[j*iWidth+i],0) * max(dy_plus[j*iWidth+i],0));

       grad[j*iWidth+i] = max(speed[j*iWidth+i],0) * grad_plus[j*iWidth+i] +
                          min(speed[j*iWidth+i],0) * grad_minus[j*iWidth+i];
       */

       dx_plus = (phi[j*iWidth+i+1] - phi[j*iWidth+i]);
       dy_plus = (phi[(j+1)*iWidth+i] - phi[j*iWidth+i]);
       dx_minus = (phi[j*iWidth+i] - phi[j*iWidth+i-1]);
       dy_minus = (phi[j*iWidth+i] - phi[(j-1)*iWidth+i]);


       grad_plus = sqrt(max(dx_minus,0) * max(dx_minus,0) +
                        min(dx_plus,0) * min(dx_plus,0) +
                        max(dy_minus,0) * max(dy_minus,0) +
                        min(dy_plus,0) * min(dy_plus,0));

       grad_minus = sqrt(min(dx_minus,0) * min(dx_minus,0) +
                         max(dx_plus,0) * max(dx_plus,0) +
                         min(dy_minus,0) * min(dy_minus,0) +
                         max(dy_plus,0) * max(dy_plus,0));

       grad[j*iWidth+i] = max(speed[j*iWidth+i],0) * grad_plus +
                          min(speed[j*iWidth+i],0) * grad_minus;
    }
  }
  
  /*
  mxDestroyArray(dx_minus_array);
  mxDestroyArray(dx_plus_array);
  mxDestroyArray(dy_minus_array);
  mxDestroyArray(dy_plus_array);
  mxDestroyArray(grad_minus_array);
  mxDestroyArray(grad_plus_array);
  */
}
