/*---
function [i,j] = imnb(r,sz) or
         [i,j] = imnb(r,samp1) or
         [i,j] = imnb(r,samp1,samp2)  
Input:    
    r = radius, [r_i,r_j] or r -> [r,r]
    sz = image size, [nr,nc] or nr -> [nr,nr]
    samp1 = 0-1 image, with 1 for pixels of interest
    samp2 = 0-1 image, neighbours of interest, default = all
Output:
    [i,j] = column vectors of UINT32, C-index for a sparse matrix
        j from pixels of 1's in samp1 
        i from pixels of 1's in samp2
        For each pixel in j, i has the indii of its neighbours
        
Stella X. Yu, July 11 2003.        

--*/


# include "mex.h"
# include "math.h"

void mexFunction(
    int nargout,
    mxArray *out[],
    int nargin,
    const mxArray *in[]
)
{
    /* declare variables */
    int nr, nc, np, nb, total, to_samp1, to_samp2;    
    double *sz, *samp1, *samp2;
    int r_i, r_j, i, j, is, ie, js, je, ii, jj, b, s, t;
    int points_needed, count2;
    unsigned int *qi, *qj;
    
    /* check argument */
    if (nargin < 2) {
        mexErrMsgTxt("Two input arguments required");
    }
    if (nargout > 2) {
        mexErrMsgTxt("Too many output arguments.");
    }

    /* get neighbourhood size */
    i = mxGetM(in[0]);
    j = mxGetN(in[0]);
    sz = mxGetData(in[0]);
    r_i = (int)sz[0];
    if (j>1 || i>1) {
        r_j = (int)sz[1];		
    } else {
        r_j = r_i;
    }
    if (r_i<=0 || r_j<=0) { 
        mexErrMsgTxt("r should be positive");
    }
        
    /* get image size */
    i = mxGetM(in[1]);
    j = mxGetN(in[1]);
    if (!mxIsDouble(in[1])) {
        mexErrMsgTxt("arguments should be double");
    }
    samp1 = mxGetData(in[1]);
    to_samp1 = 0;
    np = (i*j);
    if (np==1) {
            nr = (int) samp1[0];
            nc = nr;   
    } else if (np==2) {
            nr = (int) samp1[0];
            nc = (int) samp1[1];
    } else  {
            to_samp1 = 1;
            nr = i;
            nc = j;
    }   
    np = nr * nc;
    
    nb = (r_i + r_i + 1) * (r_j + r_j + 1); 
    if (nb>np) {
        nb = np;
    }    

    /* get neighbour map */
    to_samp2 = 0;
    if (nargin==3) {
        i = mxGetM(in[2]);
        j = mxGetN(in[2]);
        if (!(i==nr && j==nc && mxIsDouble(in[2]))) {
            mexErrMsgTxt("samp2 should be a double matrix that matches samp1");
        }
        to_samp2 = 1;        
        samp2 = mxGetData(in[2]);

	/* count non-zero elements in samp2 */
	count2 = 0;
	for (ii=0; ii<i*j; ii++) {
	  count2 += samp2[ii];
	}
	points_needed = count2*nb;
	/*	printf("%d points instead of %d\n",points_needed, np*nb); */
    } else {
      points_needed = np*nb;
    }
    

    
        
    out[0] = mxCreateNumericMatrix(points_needed, 1, mxUINT32_CLASS, mxREAL);
	out[1] = mxCreateNumericMatrix(np+1,  1, mxUINT32_CLASS, mxREAL);
	qi = mxGetData(out[0]);
	qj = mxGetData(out[1]);
	if (out[0]==NULL || out[1]==NULL) {
	    mexErrMsgTxt("Not enough space for the output matrix.");
	}

    /* computation */    	
    total = 0;
    s = -1;
    for (j=0; j<nc; j++) {        
        for (i=0; i<nr; i++) {
             s = s + 1;              
             qj[s] = total;   
             
             if (!(to_samp1) || (to_samp1 && samp1[s])) {
                 js = j - r_j;
                 if (js<0) { js = 0; }
                 je = j + r_j + 1;
                 if (je>nc) { je = nc; }
             
                 is = i - r_i;
                 if (is<0) { is = 0; }
                 ie = i + r_i + 1;
                 if (ie>nr) { ie = nr; }
             
                 b = js * nr;
                 for (jj=js; jj<je; jj++) {
                     for (ii=is; ii<ie; ii++) {
                         t = ii + b;
                         if (!(to_samp2) || (to_samp2 && samp2[t])) {
                             qi[total] = t;
                             total = total + 1;
                         }                         
                     }
                     b = b + nr;
                 }
             }
        }
        qj[np] = total;
    }
    
    mxSetM(out[0],total);
    mxSetData(out[0], mxRealloc(qi, total*sizeof(unsigned int)));
}
