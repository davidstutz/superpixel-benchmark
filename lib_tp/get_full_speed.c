#include "mex.h"
#include <math.h>

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a <= b ? a : b)

void mexFunction(
    int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[])
{
  double *grad_speed, *speed_x, *speed_y, *dx, *dy, *dxx, *dyy, *dxy, *speed, doubletWeight;
  int iWidth, iHeight;
  const double eps = 1e-16;
  
  /* The input must be a noncomplex scalar double.*/
  iHeight = mxGetM(prhs[0]);
  iWidth = mxGetN(prhs[0]);

  /* Create matrix for the return argument. */
  plhs[0] = mxCreateDoubleMatrix(iHeight,iWidth, mxREAL);
  
  /* Assign pointers to each input and output. */
  speed = mxGetPr(plhs[0]);
  dx = mxGetPr(prhs[0]);
  dy = mxGetPr(prhs[1]);
  dxx = mxGetPr(prhs[2]);
  dyy = mxGetPr(prhs[3]);
  dxy = mxGetPr(prhs[4]);
  grad_speed = mxGetPr(prhs[5]);
  speed_x = mxGetPr(prhs[6]);
  speed_y = mxGetPr(prhs[7]);
  doubletWeight = mxGetScalar(prhs[8]);
  
  for (int i = 1; i < iWidth - 1; i++)
  {
    for (int j = 1; j < iHeight - 1; j++)
    {
      double dx_2 = dx[i*iHeight+j]*dx[i*iHeight+j];
      double dy_2 = dy[i*iHeight+j]*dy[i*iHeight+j];
      double mag = sqrt(dx_2 + dy_2);
      double dx_norm = dx[i*iHeight+j] / (mag + eps);
      double dy_norm = dy[i*iHeight+j] / (mag + eps);
      double dCurvature = (dxx[i*iHeight+j]*dy_2 -
                           2*dx[i*iHeight+j]*dy[i*iHeight+j]*dxy[i*iHeight+j] +
                           dyy[i*iHeight+j]*dx_2) /
                           ((dx_2 + dy_2)*mag + eps);
      dCurvature = MAX(-1,MIN(dCurvature,1));
      double doublet = MAX(0,(dx_norm*speed_x[i*iHeight+j] + dy_norm*speed_y[i*iHeight+j]));
      double s = grad_speed[i*iHeight+j]*(1-0.3*dCurvature) - doubletWeight*doublet;
      speed[i*iHeight+j] = MAX(-1,MIN(1,s));
    }
  }
    
}
