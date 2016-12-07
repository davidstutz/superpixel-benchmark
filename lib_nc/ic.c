/*---
function cw = ic(emag,ephase,ci,cj,mask)
Input:
    emag = matrix of image size, edge magnitudes
    ephase = phase map for edges, discrete values
    [ci,cj] = c index representation for entries in the 
        pairwise affinity matrix
    mask = label matrix of image size, pixels of the same value
        have affinity of 1 regardless of emag and ephase
Output:
    cw = real vector of the same size as ci, 
        maximum emag of intervening contours

Stella X. Yu, July 9 2003.

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
    int nr, nc, np, total, to_bias;
    int k, a, ai, aj, b, bi, bj, p, pi, pj, q, qi, qj, di, dj, s, step;
    double z, maxe, slope;
	unsigned int *ci, *cj;
	double *emag, *phase, *cw, *mask;
    
    /* check argument */
    if (nargin<4) {
        mexErrMsgTxt("Four input arguments required");
    }
    if (nargout>1) {
        mexErrMsgTxt("Too many output arguments");
    }

    /* get pb and phase */
	nr = mxGetM(in[0]);
	nc = mxGetN(in[0]);
	a = mxGetM(in[1]);
	b = mxGetN(in[1]);	
	total = mxGetM(in[2]) * mxGetN(in[2]);
	np = mxGetM(in[3]) * mxGetN(in[3]) - 1;
	if ( nr != a || nc != b || nr*nc != np || np<=0) {
	    mexErrMsgTxt("Dimension mismatches between the inputs");
	}
    emag = mxGetData(in[0]);
    phase = mxGetData(in[1]);
    
    /* get the c-index pair */
    if (!mxIsUint32(in[2]) || !mxIsUint32(in[3])) {
        mexErrMsgTxt("Index pair shall be of type UINT32");
    }
    ci = mxGetData(in[2]);
    cj = mxGetData(in[3]);    
    
    /* get the mask */
    to_bias = 0;
    if (nargin>4) {
        a = mxGetM(in[4]);
        b = mxGetN(in[4]);
        if (a*b>0) {
            if (a==nr && b==nc && mxIsDouble(in[4])) {
                to_bias = 1;
                mask = mxGetData(in[4]);
            } else {
                mexErrMsgTxt("Incorrect size or type for mask");
            }
        }
    }
     
    /* create output */
    out[0] = mxCreateDoubleMatrix(total,1,mxREAL);
    if (out[0]==NULL) {
	    mexErrMsgTxt("Not enough memory for the output matrix");
	}
	cw = mxGetPr(out[0]);
	
    /* computation */ 
    for (a=0; a<np; a++) { 

        aj = a / nr; /* col */
        ai = a % nr; /* row */
                
        for (k=cj[a]; k<cj[a+1]; k++) {
        
            b = ci[k];
            
            maxe = 0.0;
            if (!((a==b) || (to_bias && mask[a]==mask[b] && mask[a]>0 && to_bias))) {
            
                bj = b / nr; 
                bi = b % nr;

            	/* (p,q): adjacent pixels moving along the line connecting a and b */
            	p = a;
            	pj = aj;
            	pi = ai;
                
                /* scan */            
                dj = abs(aj - bj);
                di = abs(ai - bi);
            
                if (dj >= di) {  /* step along j */
             	    step = (bj>aj) ? 1 : -1;            	    
           	        slope = (float)(bi-ai) / (float)(bj-aj);
            	    for (s=0; s<dj; s++) {            	           
            	        qj = pj + step;
            	        qi = (int) (0.5 + slope * (qj-aj) + ai);
            	        q = qi + qj * nr;
            	        if (1) {
			  z = emag[p];
            	            if (z>maxe) {
            	                maxe = z;
            	            }
            	        }
            	        p = q;
            	        pj = qj;
            	        pi = qi;
            	    }
            	            	    
            	} else {  /* step along i */       	
            	    step = (bi>ai) ? 1 : -1;
            	    slope = (float) (bj-aj) / (float)(bi-ai);
            	    for (s=0; s<di; s++) {            	   
            	        qi = pi + step;
            	        qj = (int) (0.5 + slope * (qi-ai) + aj);
            	        q = qi + qj * nr;            	        
            	        if (1) {
			  z = emag[p];
            	            if (z>maxe) {
            	                maxe = z;
            	            }
            	        }
            	        p = q;
            	        pj = qj;
            	        pi = qi;
            	   }
            	}             	
            	maxe = maxe * 0.5;
            }
		    cw[k] = maxe;
		} /* b */
    } /* a */
        
}    
