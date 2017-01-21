#include "mex.h"
#include "matrix.h"

#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;

#include "eaw_imp.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
   
    if(nrhs != 3)
      mexErrMsgIdAndTxt( "MATLAB:convec:invalidNumInputs",
              "image weights wavelet_type");
    
    if(nlhs > 4)
      mexErrMsgIdAndTxt( "MATLAB:convec:maxlhs",
              "Too many output arguments.");
    
    if(!mxIsDouble(prhs[0]) || mxIsComplex(prhs[1]) )
      mexErrMsgIdAndTxt( "MATLAB:convec:inputsNotComplex",
              "Must be double and real.");
   
   
   Grid I(prhs[0]) ;
   Grid W(prhs[1]) ;
   
   Grid A  ; 
   
   int wtype = mxGetPr(prhs[2])[0];

   if(wtype)
       iWRB(I, A, W);
   else
    iSPW(I, A, W) ;
   
   plhs[0] = A.mxa;
}
