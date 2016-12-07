/**
 * Copyright (c) 2016, David Stutz
 * Contact: david.stutz@rwth-aachen.de, davidstutz.de
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include<mex.h>
#include<matrix.h>
#include <algorithm>
#include <iostream>
#include <string>
#include "connected_components.h"

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    int rows = 0;
    int cols = 0;
    double* labels;
    double* components;
    double* output;
    int max_label = 0;
    int i = 0;
    int component_count = 0;
    
    if(nrhs == 1) {
        
        if(mxGetClassID(prhs[0]) != mxDOUBLE_CLASS) {
            mexErrMsgTxt("Input argument is not double class.");
        }
        
        rows = mxGetM(prhs[0]);
        cols = mxGetN(prhs[0]);

        labels = (double*) mxGetPr(prhs[0]);
    }
    else {
        mexErrMsgTxt("Invalid number of arguments, expected one argument!");
    }

    for (i = 0; i < rows*cols; i++) {
        if (labels[i] > max_label) {
            max_label = labels[i];
        }
    }
    
    components = new double[rows*cols];
    for (i = 0; i < rows*cols; i++) {
        components[i] = 0;
    }
    
    /* Note the order of rows and cols is switched ! */
    ConnectedComponents cc(2*max_label);
    cc.connected<double, double, std::equal_to<double>, bool>(labels, components, rows, 
            cols, std::equal_to<double>(), true);
    
    plhs[0] = mxCreateNumericMatrix(rows, cols, mxDOUBLE_CLASS, mxREAL);
    output = (double*) mxGetPr(plhs[0]);
    for(i = 0; i < rows*cols; i++) {
        output[i] = components[i];
    }
    
    delete[] components;
}