#include "QPBO_LazyElim.h"
#include "LazyElimination.h"

LazyElimQPBO::LazyElimQPBO(
   const Matrix< float >& A, 
   const Matrix< float >& B_right, 
   const Matrix< float >& B_down, 
   Matrix< unsigned char >& X_star ) :
      GenericQPBO( A, B_right, B_down, X_star )
   {    
    LazyElimination< float >::solve( A, B_right, B_down, X_star );
   }


