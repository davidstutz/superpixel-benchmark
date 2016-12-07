#ifndef QPBO_MaxFlow_h_
#define QPBO_MaxFlow_h_

// Solves a Quadratic Pseudo Boolean Optimization problem defined over a 
// four-connect lattice using Max-Flow.  The coefficients of the quadratic 
// terms must be negative for a valid flow graph representation.  If positive
// coefficients are encountered, the method will simply ignore them.

#include "QPBO_Generic.h"

class MaxFlowQPBO : public GenericQPBO
{
    protected:
        const int idim_;
        const int jdim_;
        
        int
        node( int i, int j )
        {
            return i * this->jdim_ + j;
        }

    public:
        // See QPBO_Generic.h for details about these parameters.
        MaxFlowQPBO( const Matrix< float >& A, 
                     const Matrix< float >& B_right, 
                     const Matrix< float >& B_down, 
                     Matrix< unsigned char >& Seg );
};

#endif
