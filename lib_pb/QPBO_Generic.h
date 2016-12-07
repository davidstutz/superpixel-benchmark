#ifndef QPBO_Generic_h_
#define QPBO_Generic_h_

// Generic Quadartic Pseduo Boolean Optimization Algorithm for a 4-connect
// lattice of variables.  This file doesn't actually do anything, except
// specify the generic interface to any optimization algorithm (such as
// max-flow, roof dual, elimination ("basic") and lazy elimination).
//
// *** Do not confuse this with Kolmogorov's "QPBO" library ***

#include "matrix.h"

class GenericQPBO
   {
   public:
   // The main interface to any optimization technique.
   // A contains the coefficients of the variables x_{i,j}
   // B_right contains the coefficients of x_{i,j} . x_{i,j+1}
   // B_down contains the coefficients of the x_{i,j} . x_{i+1,j}
   // X is the (sub)optimal labelling.  It will be resized to m x n
   // and each element will be set to either 0 or 1. 
   GenericQPBO( const Matrix< float >& A, 
                const Matrix< float >& B_right, 
                const Matrix< float >& B_down, 
                Matrix< unsigned char >& X )
      {
      const unsigned int rows = A.rows();
      const unsigned int cols = A.cols();
    
      assert( B_right.rows() == rows );
      assert( B_right.cols() == cols );
      assert( B_down.rows() == rows );
      assert( B_down.cols() == cols );
        }
   };

#endif // QPBO_Generic_h_
