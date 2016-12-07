#ifndef Elimination_h_
#define Elimination_h_

#include "matrix.h"
#include "Elimination+SSE.h"

template< typename T >
class Elimination
   {
   protected:
      // The pseudo-inverse for the least squares problem is really a
      // 9x16 matrix.  However, we don't care what the value of the constant
      // is in the approximated expression.  So we don't need to calculate
      // it, which means we can remove one row of the matrix.
      FixedMatrix< T, 16, 8 > APseudoInverseTranspose_;

      const bool verbose_;

   protected:

      void
         initialize();

      void
         traverse1( const PaddedMatrix< T >& A,
                    const PaddedMatrix< T >& B_right, 
                    const PaddedMatrix< T >& B_down,
                    PaddedMatrix< T >& A2,
                    PaddedMatrix< T >& B2_dr,
                    PaddedMatrix< T >& B2_ur );

      void
         traverse2( const PaddedMatrix< T >& A,
                    const PaddedMatrix< T >& B_dr, 
                    const PaddedMatrix< T >& B_ur, 
                    PaddedMatrix< T >& Anew, 
                    PaddedMatrix< T >& B_right,
                    PaddedMatrix< T >& B_down );

      void
         optimize( const PaddedMatrix< T >& A,
                   const PaddedMatrix< T >& B_right, 
                   const PaddedMatrix< T >& B_down, 
                   PaddedMatrix< unsigned char >& X_star );
   public:

      Elimination() :
            verbose_( false )
         {
         this->initialize();
         }

      void
         compute_coefficients( const T * a, T * newvals ) const;

      static void
         solve( const Matrix< T >& A,
                const Matrix< T >& B_right, 
                const Matrix< T >& B_down, 
                Matrix< unsigned char >& X_star );
   };

#endif // Elimination_h_
