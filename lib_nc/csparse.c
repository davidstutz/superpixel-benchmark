/*--
function w = csparse(i,j,v,m)
Input:
   [i,j] = index pair in MATLAB internal sparse matrix representation
       UINT32 type
       i is of total length of valid elements, 0 for first row
       j is of length n + 1
   v = value for each [i,j], of same length as i, full, double
   m = row dimension
Output:
   w = m x n matlab sparse matrix, w([i,j]) = v

Stella X. Yu, Nov 12, 2001.
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
    int m, n, nk, i, j;
    unsigned int *pi, *pj;
	mwIndex *ir, *jc;
	double *v, *pr;
    
    /* check argument */
    if (nargin !=4) {
        mexErrMsgTxt("Four input arguments required");
    }
    if (nargout> 1) {
        mexErrMsgTxt("Too many output arguments.");
    }

    /* get index */
    i = mxGetM(in[0]);
    j = mxGetN(in[0]);
	nk = i * j;
	if (nk==0) {
		return;
	}
    pi = mxGetData(in[0]);
    
    i = mxGetM(in[1]);
    j = mxGetN(in[1]);
	n = i * j - 1;
	if (n<=0) {
		return;
	}
	pj = mxGetData(in[1]);

	/* get value */
    i = mxGetM(in[2]);
    j = mxGetN(in[2]);
	if (i*j != nk) {
		mexErrMsgTxt("index and value dimension mismatch");
	}
	v = mxGetPr(in[2]);

	/* get dimension */
	m = mxGetScalar(in[3]);
	
	if (m<nk/n) {
	    mexErrMsgTxt("Impossible to fit all the nonzeros into the matrix you specififed");
	}

	/* output */
    out[0] = mxCreateSparse(m,n,nk,mxREAL);
	if (out[0]==NULL) {
		mexErrMsgTxt("Not enough memory for the output matrix");
	}
    pr = mxGetPr(out[0]);
	ir = mxGetIr(out[0]);
    jc = mxGetJc(out[0]);
	for (i=0; i<nk; i++) {
		ir[i] = pi[i];
		pr[i] = v[i];
	}
	for (j=0; j<=n; j++) {
		jc[j] = pj[j];
	}	
}  
