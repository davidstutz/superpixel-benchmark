#ifndef QPBO_LazyElim_h_
#define QPBO_LazyElim_h_

// Solves a Quadratic Pseudo Boolean Optimization problem defined over a 
// four-connect lattice using the LazyElimination algorithm.

#include "QPBO_Generic.h"

class LazyElimQPBO : public GenericQPBO
   {
   public:
      // See QPBO_Generic.h for details about these parameters.
      LazyElimQPBO( const Matrix< float >& A, 
                    const Matrix< float >& B_right, 
                    const Matrix< float >& B_down, 
                    Matrix< unsigned char >& X_star );
   };

#endif // QPBO_LazyElim_h_
