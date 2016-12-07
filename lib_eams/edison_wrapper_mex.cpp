#include "mex.h"
#include <math.h>

#include "edison_wrapper_mex.h"
/*
 *
 * image - of type single, after permuting [3 2 1]
 */

/*
 * main enterance point 
 */
void mexFunction(
    int		  nlhs, 	/* number of expected outputs */
    mxArray	  *plhs[],	/* mxArray output pointer array */
    int		  nrhs, 	/* number of inputs */
    const mxArray	  *prhs[]	/* mxArray input pointer array */
    )
{
    if ( nrhs != 3 )
        mexErrMsgIdAndTxt("edison_wraper:main","Must have three inputs");
    if ( mxGetClassID(prhs[0]) != mxSINGLE_CLASS )
        mexErrMsgIdAndTxt("edison_wraper:main","fim must be of type single");
    if ( mxGetClassID(prhs[1]) != mxUINT8_CLASS )
        mexErrMsgIdAndTxt("edison_wraper:main","rgbim must be of type uint8");
    if ( mxGetClassID(prhs[2]) != mxSTRUCT_CLASS )
        mexErrMsgIdAndTxt("edison_wraper:main","parameters argument must be a structure");
    
    
    /* first argument - the image in whatever feature space it is given in */
    float * fimage = (float*)mxGetData(prhs[0]);
    const int* image_dims = mxGetDimensions(prhs[0]);
    int image_ndim = mxGetNumberOfDimensions(prhs[0]);
    unsigned int w = image_dims[1];
    unsigned int h = image_dims[2];
    unsigned int N = image_dims[0];
    int ii;
    
    unsigned char * rgbim = (unsigned char*)mxGetData(prhs[1]);
    const int * rgb_dims = mxGetDimensions(prhs[1]);
    if ( rgb_dims[1] != w || rgb_dims[2] != h )
        mexErrMsgIdAndTxt("edison_wraper:main","size of fim and rgbim do not match");
    
    /* second input - parameters structure */
    //   'steps'       - What steps the algorithm should perform:
    //               1 - only mean shift filtering
    //               2 - filtering and region fusion
    //               3 - full segmentation process [default]
    int steps;
    GetScalar(mxGetField(prhs[2], 0, "steps"), steps);
    
    //   'synergistic' - perform synergistic segmentation [true]|false
    bool syn;
    GetScalar(mxGetField(prhs[2], 0, "synergistic"), syn);
    
    //   'SpatialBandWidth' - segmentation spatial radius (integer) [7]
    unsigned int spBW;
    GetScalar(mxGetField(prhs[2], 0, "SpatialBandWidth"), spBW);

    //  'RangeBandWidth'   - segmentation feature space radius (float) [6.5]
    float fsBW;
    GetScalar(mxGetField(prhs[2], 0, "RangeBandWidth"), fsBW);

    //   'MinimumRegionArea'- minimum segment area (integer) [20]
    unsigned int minArea;
    GetScalar(mxGetField(prhs[2], 0, "MinimumRegionArea"), minArea);
    
    //   'SpeedUp'          - algorithm speed up {0,1,2} [1]
    int SpeedUp;
    enum SpeedUpLevel sul;
    GetScalar(mxGetField(prhs[2], 0, "SpeedUp"), SpeedUp);
    switch (SpeedUp) {
        case 1:
            sul = NO_SPEEDUP;
            break;
        case 2:
            sul = MED_SPEEDUP;
            break;
        case 3:
            sul = HIGH_SPEEDUP;
            break;
        default:
            mexErrMsgIdAndTxt("edison_wraper:main","wrong speedup value");
    }
    
    //   'GradientWindowRadius' - synergistic parameters (integer) [2]
    unsigned int grWin;
    GetScalar(mxGetField(prhs[2], 0, "GradientWindowRadius"), grWin);
    
    //   'MixtureParameter' - synergistic parameter (float 0,1) [.3]
    float aij;
    GetScalar(mxGetField(prhs[2], 0, "MixtureParameter"), aij);
    
    //   'EdgeStrengthThreshold'- synergistic parameter (float 0,1) [.3]
    float edgeThr;
    GetScalar(mxGetField(prhs[2], 0, "EdgeStrengthThreshold"), edgeThr);
    
    msImageProcessor ms;
    ms.DefineLInput(fimage, h, w, N); // image array should be after permute
    if (ms.ErrorStatus)
        mexErrMsgIdAndTxt("edison_wraper:edison","Mean shift define latice input: %s", ms.ErrorMessage);
    
    kernelType k[2] = {DefualtKernelType, DefualtKernelType};
    int P[2] = {DefualtSpatialDimensionality, N};
    float tempH[2] = {1.0, 1.0};
    ms.DefineKernel(k, tempH, P, 2); 
    if (ms.ErrorStatus)
        mexErrMsgIdAndTxt("edison_wraper:edison","Mean shift define kernel: %s", ms.ErrorMessage);

    mxArray * mxconf = NULL;
    float * conf = NULL;
    mxArray * mxgrad = NULL;
    float * grad = NULL;
    mxArray * mxwght = NULL;
    float * wght = NULL;
    
    if (syn) {
        /* perform synergistic segmentation */
        int maps_dim[2] = {w*h, 1};
        /* allcate memory for confidence and gradient maps */
        mxconf = mxCreateNumericArray(2, maps_dim, mxSINGLE_CLASS, mxREAL);
        conf = (float*)mxGetData(mxconf);
        mxgrad = mxCreateNumericArray(2, maps_dim, mxSINGLE_CLASS, mxREAL);
        grad = (float*)mxGetData(mxgrad);
        
        BgImage rgbIm;
        rgbIm.SetImage(rgbim, w, h, rgb_dims[0] == 3);
        BgEdgeDetect edgeDetector(grWin);
        edgeDetector.ComputeEdgeInfo(&rgbIm, conf, grad);
        
        mxwght = mxCreateNumericArray(2, maps_dim, mxSINGLE_CLASS, mxREAL);
        wght = (float*)mxGetData(mxgrad);
        
        for ( ii = 0 ; ii < w*h; ii++ ) {
            wght[ii] = (grad[ii] > .002) ? aij*grad[ii]+(1-aij)*conf[ii] : 0;
        }
        ms.SetWeightMap(wght, edgeThr);
        if (ms.ErrorStatus)
            mexErrMsgIdAndTxt("edison_wraper:edison","Mean shift set weights: %s", ms.ErrorMessage);

    }
    ms.Filter(spBW, fsBW, sul);
    if (ms.ErrorStatus)
        mexErrMsgIdAndTxt("edison_wraper:edison","Mean shift filter: %s", ms.ErrorMessage);

    if (steps == 2) {
        ms.FuseRegions(fsBW, minArea);
        if (ms.ErrorStatus)
            mexErrMsgIdAndTxt("edison_wraper:edison","Mean shift fuse: %s", ms.ErrorMessage);
    }
    
    if ( nlhs >= 1 ) {
        // first output - the feture space raw image
        plhs[0] = mxCreateNumericArray(image_ndim, image_dims, mxSINGLE_CLASS, mxREAL);
        fimage = (float*)mxGetData(plhs[0]);
        ms.GetRawData(fimage);
    }
    
    int* labels;
    float* modes;
    int* count;
    int RegionCount = ms.GetRegions(&labels, &modes, &count);

    if ( nlhs >= 2 ) {
        // second output - labeled image
        plhs[1] = mxCreateNumericArray(2, image_dims+1, mxINT32_CLASS, mxREAL);
        int* plabels = (int*)mxGetData(plhs[1]);
        for (ii=0; ii< w*h; ii++)
            plabels[ii] = labels[ii];
    }
    delete [] labels;
    int arr_dims[2];
    if ( nlhs >= 3 ) {
        // third output - the modes
        arr_dims[0] = N;
        arr_dims[1] = RegionCount;
        plhs[2] = mxCreateNumericArray(2, arr_dims, mxSINGLE_CLASS, mxREAL);
        fimage = (float*)mxGetData(plhs[2]);
        for (ii=0;ii<N*RegionCount; ii++)
            fimage[ii] = modes[ii];
    }
    delete [] modes;
    
    if ( nlhs >= 4 ) {
        // fourth output - region sizes (# of pixels)
        arr_dims[0] = 1;
        arr_dims[1] = RegionCount;
        plhs[3] = mxCreateNumericArray(2, arr_dims, mxINT32_CLASS, mxREAL);
        int * pc = (int*)mxGetData(plhs[3]);
        for (ii=0;ii<RegionCount; ii++)
            pc[ii] = count[ii];
    }   
    delete [] count;
    
    if ( !syn )
        return;
    
    if ( nlhs >= 5)
        // fifth output - gradient map
        plhs[4] = mxgrad;
    else
        mxDestroyArray(mxgrad);
    
    if ( nlhs >= 6)
        plhs[5] = mxconf;
    else
        mxDestroyArray(mxconf);
   
}

template<class T>
void GetScalar(const mxArray* x, T& scalar)
{
    if ( mxGetNumberOfElements(x) != 1 )
        mexErrMsgIdAndTxt("weight_sample_mex:GetScalar","input is not a scalar");
    void *p = mxGetData(x);
    switch (mxGetClassID(x)) {
        case mxLOGICAL_CLASS:
            scalar = *(bool*)p;
            break;
        case mxCHAR_CLASS:
            scalar = *(char*)p;
            break;
        case mxDOUBLE_CLASS:
            scalar = *(double*)p;
            break;
        case mxSINGLE_CLASS:
            scalar = *(float*)p;
            break;
        case mxINT8_CLASS:
            scalar = *(char*)p;
            break;
        case mxUINT8_CLASS:
            scalar = *(unsigned char*)p;
            break;
        case mxINT16_CLASS:
            scalar = *(short*)p;
            break;
        case mxUINT16_CLASS:
            scalar = *(unsigned short*)p;
            break;
        case mxINT32_CLASS:
            scalar = *(int*)p;
            break;
        case mxUINT32_CLASS:
            scalar = *(unsigned int*)p;
            break;
#ifdef A64BITS            
        case mxINT64_CLASS:
            scalar = *(int64_T*)p;
            break;
        case mxUINT64_CLASS:
            scalar = *(uint64_T*)p;
            break;
#endif /* 64 bits machines */            
        default:
            mexErrMsgIdAndTxt("GraphCut:GetScalar","unsupported data type");
    }
}

