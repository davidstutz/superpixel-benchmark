#include <stdio.h>
#include <float.h>
#include <math.h>
#include <limits>
#include "mex.h"

double regtree_findbestsplit(const double *x, const double *y, int lennode, double &bestcrit, int minleaf)
{
    double *xy = new double[lennode];
    double *x2 = new double[lennode];
    double *y2 = new double[lennode];
    int *cutpoints = new int[lennode];
    int i,j;
    double loss;
    double ally_right, x2_right, xy_right, y2_right, xright, wright, my_right,
           ally_left, x2_left, xy_left, y2_left, xleft, wleft, my_left;
    int nleft, nright;
    int counter = 0, the_pos, last_pos = 0;
    double cutval = 0.0;

    // Everything to 0 first
    ally_right = x2_right = xy_right = y2_right = xright = 0.0;
    ally_left = x2_left = xy_left = y2_left = xleft = wleft = 0.0;
    nleft = 0, nright = lennode;
    
    // First iteration
    // Fill the matrices, find the cut points
    for (i=0;i<lennode;i++)
    {
        xy[i] = x[i] * y[i];
        x2[i] = x[i] * x[i];
        y2[i] = y[i] * y[i];
        ally_right += y[i];
        xy_right += xy[i];
        x2_right += x2[i];
        y2_right += y2[i];
        xright += x[i];
        if (i >= minleaf - 1 && i+1 <= lennode - minleaf && x[i+1] != x[i])
            cutpoints[counter++] = i;
    }
    cutpoints[counter] = -1;
    // Initial w and loss
    my_right = ally_right / nright;
    wright = (xy_right - xright * my_right) / x2_right;
    loss = y2_right - 2 * wright * xy_right + wright * wright * x2_right
     + y2_left - 2 * wleft * xy_left + wleft * wleft * x2_left;
    if (-loss > bestcrit)
    {
        bestcrit = -loss;
        cutval = -DBL_MAX;
    }
    for (i=0;cutpoints[i]!=-1;i++)
    {
        the_pos = cutpoints[i];
        double y_change, y2_change, x_change, x2_change, xy_change;
        y_change = y2_change = x_change = x2_change = xy_change = 0.0;
        // Find the changes
        for (j=last_pos;j<=the_pos;j++)
        {
            y_change += y[j];
            y2_change += y2[j];
            x_change += x[j];
            x2_change += x2[j];
            xy_change += xy[j];
        }
        ally_left += y_change;
        ally_right -= y_change;
        y2_left += y2_change;
        y2_right -= y2_change;
        x2_left += x2_change;
        x2_right -= x2_change;
        xleft += x_change;
        xright -= x_change;
        xy_left += xy_change;
        xy_right -= xy_change;
        nleft += the_pos - last_pos + 1;
        nright -= the_pos - last_pos + 1;
        my_left = ally_left / nleft;
        my_right = ally_right / nright;
        wright = (xy_right - xright * my_right) / x2_right;
        wleft = (xy_left - xleft * my_left) / x2_left;
        loss = y2_right - 2 * wright * xy_right + wright * wright * x2_right
         + y2_left - 2 * wleft * xy_left + wleft * wleft * x2_left;
        if (-loss > bestcrit)
        {
            bestcrit = -loss;
            cutval = (x[the_pos] + x[the_pos+1]) / 2.0;
        }
        last_pos = the_pos + 1;
    }
    delete[] xy;
    delete[] x2;
    delete[] y2;
    delete[] cutpoints;
    xy = x2 = y2 = 0;
    cutpoints = 0;
    return cutval;
}

// Tree, x, y, w, bestcrit, minleaf
// w unused for now
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int minleaf = 1;
    double bestcrit = -DBL_MAX;
    double bestcrit_save;
    double *x, *y;
    double cutval;
// Grab minleaf
    if (nrhs >= 6)
        minleaf = (int)ceil((mxGetPr(prhs[5])[0]));
    if (nrhs >= 5)
    {
        mxClassID bestcrit_class = mxGetClassID(prhs[4]);
        if (bestcrit_class == mxDOUBLE_CLASS)
            bestcrit = (double) (mxGetPr(prhs[4])[0]);
        else if (bestcrit_class == mxSINGLE_CLASS)
            bestcrit = (double) (((float *)mxGetPr(prhs[4]))[0]);
        else
        {
            mexPrintf("bestcrit must be either double or single!");
            return;
        }
    }        
    if (mxGetClassID(prhs[1]) != mxDOUBLE_CLASS || mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
    {
        mexPrintf("Single input/outputs not yet supported for now, will add support later.");
        return;
    }
    x = mxGetPr(prhs[1]);
    y = mxGetPr(prhs[2]);
    int lennode = mxGetN(prhs[2]) > mxGetM(prhs[2])? mxGetN(prhs[2]):mxGetM(prhs[2]);
    bestcrit_save = bestcrit;
    cutval = regtree_findbestsplit(x,y, lennode, bestcrit, minleaf);
    if (cutval == -DBL_MAX)
        bestcrit = bestcrit_save;
    plhs[0] = mxCreateDoubleScalar(bestcrit);
    plhs[1] = mxCreateDoubleScalar(cutval);
}