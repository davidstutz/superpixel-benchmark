/*--
function C = [parmatV(V,p,K)] returns X'*V for partition matrix X.
Input:
    V = M times N matrix
    p = M times 1 label vector, indicating a partition matrix
        X = sparse([1:M]',p,ones(M,1),M,K)
    K = scalor, ie. X's dimension (max label), default K = N
Output:
    C = X' * V, K x N matrix
Comments:
    This operation speeds up by avoiding multiplication of a 0-1 partition 
    matrix with any matrix.  It also assumes a compact representation for X.
    V, p, and C are all real matrices.
    
--*/

# include "math.h"
# include "mex.h"

void mexFunction(
    int nargout,
    mxArray *out[],
    int nargin,
    const mxArray *in[]
)
{
    /* declare variables */
    int M,N,K,i,j,s,t,u;
    double *V, *p, *C;
    
    /* check argument */
    if (nargin <2) {
        mexErrMsgTxt("Two input arguments required");
    }
    if (nargout>1) {
        mexErrMsgTxt("Too many output arguments.");
    }
     
    /* get all inputs */
    M = mxGetM(in[0]);
    N = mxGetN(in[0]);
    
    i = mxGetM(in[1]) * mxGetN(in[1]);
    if (i!=M) {
        mexErrMsgTxt("Dimension mismatch.");
    }
    
    if (nargin==3) {
        K = mxGetScalar(in[2]);
    } else {
        K = N;
    }
    
    V = mxGetPr(in[0]);
    p = mxGetPr(in[1]);
    
    
    /* computation starts */
    out[0] = mxCreateDoubleMatrix(K,N,0);
    if (out[0]==NULL) {
        mexErrMsgTxt("Not enough space for the output matrix.");
    }
    C = mxGetPr(out[0]);
    
    /* multiply */
    for (i=0; i<M; i++) {
        j = (int) p[i] - 1;
        s = j;
        u = i;
        for (t=0; t<N; t++) {       
            C[s] = C[s] + V[u];
            s = s + K;
            u = u + M;
        }
    }    
} 
