#include "QPBO_Elim.h"
#include "Elimination.h"

ElimQPBO::ElimQPBO( const Matrix< float >& A, 
                    const Matrix< float >& B_right, 
                    const Matrix< float >& B_down, 
                    Matrix< unsigned char >& X ) : 
GenericQPBO( A, B_right, B_down, X )
   {
    Elimination< float >::solve( A, B_right, B_down, X );
   }


