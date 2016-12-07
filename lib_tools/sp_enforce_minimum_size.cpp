/**
 * Adapted from the SLIC code.
 * See lib_slic/README.md for details.
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
    int* input;
    int* output;
    
    if(nrhs == 1) {
        
        if(mxGetClassID(prhs[0]) != mxINT32_CLASS) {
            mexErrMsgTxt("Input argument is not int32 class.");
        }
        
        rows = mxGetM(prhs[0]);
        cols = mxGetN(prhs[0]);

        input = (int*) mxGetPr(prhs[0]);
    }
    else {
        mexErrMsgTxt("Invalid number of arguments, expected one argument!");
    }
    
    int* dx4 = new int[4];
    dx4[0] = -1;
    dx4[1] = 0;
    dx4[2] = 1;
    dx4[3] = 0;
    
    int* dy4 = new int[4];
    dy4[0] = 0;
    dy4[1] = -1;
    dy4[2] = 0;
    dy4[3] = 1;
    
    int sz = rows*cols;
    int SUPSZ = 5;
    
    int label = 0;
    int* xvec = new int[sz];
    int* yvec = new int[sz];
    int oindex = 0;
    int adjlabel = 0;
    
    int* labels = new int[sz];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            labels[i*cols + j] = input[j*rows + i];
        }
    }
    
    int *nlabels = new int[sz];
    for (int i = 0; i < sz; i++) {
        nlabels[i] = -1;
    }

    for (int j = 0; j < rows; j++) {
        for (int k = 0; k < cols; k++) {
            if (0 > nlabels[oindex]) {

                nlabels[oindex] = label;
                
                //--------------------
                // Start a new segment
                //--------------------
                xvec[0] = k;
                yvec[0] = j;

                //-------------------------------------------------------
                // Quickly find an adjacent label for use later if needed
                //-------------------------------------------------------
                {
                    for (int n = 0; n < 4; n++) {
                        int x = xvec[0] + dx4[n];
                        int y = yvec[0] + dy4[n];
                        if( (x >= 0 && x < cols) && (y >= 0 && y < rows) )
                        {
                            int nindex = y*cols + x;
                            if (nlabels[nindex] >= 0) {
                                adjlabel = nlabels[nindex];
                            }
                        }
                    }
                }

                int count = 1;
                for (int c = 0; c < count; c++) {
                    for(int n = 0; n < 4; n++) {
                        int x = xvec[c] + dx4[n];
                        int y = yvec[c] + dy4[n];

                        if ((x >= 0 && x < cols) && (y >= 0 && y < rows )) {
                            int nindex = y*cols + x;

                            if (0 > nlabels[nindex] && labels[oindex] == labels[nindex]) {
                                xvec[count] = x;
                                yvec[count] = y;
                                nlabels[nindex] = label;
                                count++;
                            }
                        }

                    }
                }

                //-------------------------------------------------------
                // If segment size is less then a limit, assign an
                // adjacent label found before, and decrement label count.
                //-------------------------------------------------------
                if (count <= SUPSZ) {
                    for (int c = 0; c < count; c++) {
                        int ind = yvec[c]*cols+xvec[c];
                        nlabels[ind] = adjlabel;
                    }

                    label--;
                }

                label++;
            }

            oindex++;
        }
    }
    
    plhs[0] = mxCreateNumericMatrix(rows, cols, mxINT32_CLASS, mxREAL);
    output = (int*) mxGetPr(plhs[0]);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            output[j*rows + i] = nlabels[i*cols + j];
        }
    }
    
    delete[] nlabels;
    delete[] labels;
    delete[] dx4;
    delete[] dy4;
    delete[] xvec;
    delete[] yvec;
}