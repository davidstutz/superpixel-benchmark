#ifndef _EDISON_WRAPPER_MEX_H_
#define _EDISON_WRAPPER_MEX_H_

/* general include files */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tmwtypes.h>
#include <time.h>

/* edison include files */
#include "segm/msImageProcessor.h"

#include "edge/BgImage.h"
#include "edge/BgDefaults.h"
#include "edge/BgEdge.h"
#include "edge/BgEdgeList.h"
#include "edge/BgEdgeDetect.h"


const kernelType DefualtKernelType = Uniform;
const unsigned int DefualtSpatialDimensionality = 2;

// const char * OutputFields = { "fimage", "labels", "modes", "regSize", "conf", "grad" };

bool CmCDisplayProgress = false; /* disable display promt */

template<class T>
void GetScalar(const mxArray* x, T& scalar);

#endif // _EDISON_WRAPPER_MEX_H_