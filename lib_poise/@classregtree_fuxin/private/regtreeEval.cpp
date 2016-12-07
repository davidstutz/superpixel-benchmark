#include <stdio.h>
#include <float.h>
#include <math.h>
#include <limits>
#include "mex.h"

void regtree_eval(const double *x, const double *nodes, const int *var, const double *cut, const int *children,
                  double **reg_ws, double **reg_pvars, int length, int nvars, int nnodes, const int *reg_sizes,
                  double *y, double *y_nodes)
{
    int i, j, curnode;
    int xstart;
    double value;
    for (i=0;i<length;i++)
    {
// The current example spans from xstart to xstart + nvars - 1
        xstart = i * nvars;
        curnode = 0;
// var[curnode] == 0 means no longer splitting
        while(var[curnode])
        {
// Decide to go to left child or right
            if (x[xstart + var[curnode] - 1] >= cut[curnode])
                curnode = children[curnode * 2 + 1] - 1;
            else
                curnode = children[curnode * 2] - 1;
        }
		if (curnode < 0 || curnode >= nnodes)
		{
			mexPrintf("Curnode failing!\n");
			break;
		}
        if (reg_sizes[curnode] < 1)
        {
            mexPrintf("Error: reg_ws doesn't match the nodes. Empty reg_ws at leaf node.");
            return;
        }
        value = reg_ws[curnode][0];
// At the leaf: use reg_ws and reg_pvars to get a linear regressor
        for(j=1;j<reg_sizes[curnode];j++)
		{
			if(reg_pvars[curnode][j-1] < 0 || reg_pvars[curnode][j-1] > nvars)
			{
				mexPrintf("Possible trouble at %d, reg_pvars wrong.\n",i);
			}
            value += reg_ws[curnode][j] * x[xstart + int(reg_pvars[curnode][j-1]) - 1];
		}
        y[i] = value;
		if (y_nodes)
			y_nodes[i] = nodes[curnode];
    }
}

// X, node, var, cut, children, reg_ws, reg_pvars
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int N,i, nnodes;
    double *x, *nodes, *cut, **reg_ws, **reg_pvars, *y;
    int *var, *children;
    double *dbl_var, *dbl_children;
    double *y_nodes;
    int *reg_sizes;
// Grab minleaf
    if (nrhs < 7)
    {
        mexPrintf("Usage: regtreeEval(x,node, splitting_var, cut_position, children, reg_ws, reg_pvars\n");
        return;
    }
    if (mxGetClassID(prhs[0]) != mxDOUBLE_CLASS)
    {
        mexPrintf("Single input/outputs not yet supported for now, will add support later.");
        return;
    }
    x = mxGetPr(prhs[0]);
    nodes = mxGetPr(prhs[1]);
    dbl_var = mxGetPr(prhs[2]);
    dbl_children = mxGetPr(prhs[4]);
// cut, reg_ws and reg_pvars are cell arrays
    if (!mxIsCell(prhs[3]) || !mxIsCell(prhs[5]) || !mxIsCell(prhs[6]))
    {
        mexPrintf("Cut, reg_ws and reg_pvars must be cell arrays.");
        return;
    }
// number of nodes is the length of node
    nnodes = mxGetM(prhs[1]) > mxGetN(prhs[1]) ? mxGetM(prhs[1]) : mxGetN(prhs[1]);
// Start reading the 3 cell arrays
    reg_ws = new double *[nnodes];
    reg_pvars = new double *[nnodes];
    reg_sizes = new int [nnodes];
    cut = new double [nnodes];
    var = new int [nnodes];
    children = new int [nnodes*2];
    for (i=0;i < nnodes;i++)
    {
        double *ptr;
        mxArray *this_w;
        ptr = mxGetPr(mxGetCell(prhs[3],i));
// Only one cut
        cut[i] = ptr[0];
        this_w = mxGetCell(prhs[5],i);
        var[i] = floor(dbl_var[i]);
        children[i*2] = floor(dbl_children[i]);
        children[i*2+1] = floor(dbl_children[i+nnodes]);
		// Empty
		if (!this_w)
		{
			reg_ws[i] = 0;
			reg_pvars[i] = 0;
			reg_sizes[i] = 0;
		}
		else
		{
			reg_ws[i] = mxGetPr(this_w);
			reg_pvars[i] = mxGetPr(mxGetCell(prhs[6],i));
			reg_sizes[i] = mxGetM(this_w) > mxGetN(this_w) ? mxGetM(this_w) : mxGetN(this_w);
		}
    }
    N = mxGetN(prhs[0]);
    plhs[0] = mxCreateDoubleMatrix(N,1,mxREAL);
    if (nlhs > 1)
    {
        plhs[1] = mxCreateDoubleMatrix(N,1,mxREAL);
    }
	y = mxGetPr(plhs[0]);
	if (nlhs > 1)
		y_nodes = mxGetPr(plhs[1]);
	else
		y_nodes = 0;
    regtree_eval(x, nodes, var, cut, children, reg_ws, reg_pvars, N , mxGetM(prhs[0]), nnodes, reg_sizes, y, y_nodes);
    delete[] reg_ws;
    delete[] reg_pvars;
    delete[] reg_sizes;
    delete[] var;
	delete[] cut;
    delete[] children;
	y = 0;
	y_nodes = 0;
	reg_ws = 0;
	reg_pvars = 0;
	reg_sizes = 0;
	var = 0;
	cut = 0;
	children = 0;
}