#include "mex.h"
#include "matrix.h"

#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;

#include "eaw_imp.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    if(nrhs != 4)
        mexErrMsgIdAndTxt( "MATLAB:convec:invalidNumInputs",
                "image wavelet_type dist_type sigma.");
    
    if(nlhs > 4)
        mexErrMsgIdAndTxt( "MATLAB:convec:maxlhs",
                "Too many output arguments.");
    
    if(!mxIsDouble(prhs[0]) || mxIsComplex(prhs[1]) )
        mexErrMsgIdAndTxt( "MATLAB:convec:inputsNotComplex",
                "Must be double and real.");
    
        
    Grid I(prhs[0]);
    Grid A, W ;
    
    int wtype = mxGetPr(prhs[1])[0];
    int distf = mxGetPr(prhs[2])[0];
    double sigma = mxGetPr(prhs[3])[0];
    
    if(distf)
      init(sigma) ;
    else
      init_exp(sigma) ;
   
    if(wtype)
        WRB(I, A, W);
    else
        SPW(I, A, W) ;
    
    plhs[0] = A.mxa ;
    plhs[1] = W.mxa ;
}
