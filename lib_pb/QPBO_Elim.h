#ifndef QPBO_Elim_h_
#define QPBO_Elim_h_

// Solves a Quadratic Pseudo Boolean Optimization problem defined over a 
// four-connect lattice using the Elimination algorithm.

#include "QPBO_Generic.h"

class ElimQPBO : public GenericQPBO
   {
   public:
      // See QPBO_Generic.h for details about these parameters.
      ElimQPBO( const Matrix< float >& A, 
                const Matrix< float >& B_right, 
                const Matrix< float >& B_down, 
                Matrix< unsigned char >& X );
   };

#endif // QPBO_Elim_h_
