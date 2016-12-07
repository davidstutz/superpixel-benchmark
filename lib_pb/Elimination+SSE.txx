#ifndef Elimination_SSE_txx_
#define Elimination_SSE_txx_

#include "Elimination+SSE.h"

#if ELIMINATION_SSE
#include <xmmintrin.h>

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <exception>
#include <stdexcept>

#define _mm_splat_ps( v, i ) _mm_shuffle_ps( v, v, _MM_SHUFFLE(i,i,i,i) )


template<>
void Elimination< float >::compute_coefficients( 
      const float *a, float * newvals ) const
   {
   //printf( "SSE Optimized Routine\n" );

   // At the input, a is a matrix consisting of the values of the 
   // cost function in a 4-neighbourhood.
   // newvals will be returned as the values of the terms in the 
   // reduced function.

   // We fill out the vector.  This gives the values
   // of the function for all possible assignments to the variables x_i

   __m128 values[4];
   __m128 a4 = _mm_set_ps1( a[4] );

   values[0] = _mm_add_ps( _mm_add_ps( _mm_set_ps1( a[0] ),
      _mm_set_ps( a[1], 0.0f, a[1], 0.0f ) ),
      _mm_set_ps( a[2], a[2], 0.0f, 0.0f ) );
   values[1] = _mm_add_ps( values[0], _mm_set_ps1( a[3] ) );
   values[2] = _mm_add_ps( values[0], a4 );
   values[3] = _mm_add_ps( values[1], a4 );

   // New values for the variables
   __m128 newvals1 = _mm_setzero_ps();
   __m128 newvals2 = _mm_setzero_ps();

   // For registry i/o efficiency, each row of matrix A needs to be sixteen 
   // byte aligned.  Fortunately, the matrix is eight columns wide, so
   // we only need to ensure the first element is sixteen byte aligned.
   // Furthermore, since these values never change, we can take the one-time
   // performance hit to re-copy the data to memory aligned addresses.
   // Otherwise, we might be better off to use the less efficient non-aligned
   // i/o commands.

   static
      ELIMINATION_SSE_ALIGN
      float coefficients[8*16];

   static bool initialized = false;

   if ( initialized == false )
      {
      // _aligned_malloc(8*16*sizeof(float),16)
      //coefficients = (float *)malloc( 8 * 16 * sizeof( float ) );

      // why isn't memcpy working?
      for ( int i = 0; i < 16; ++i )
         for ( int j = 0; j < 8; ++j )
            coefficients[i*8+j] = this->APseudoInverseTranspose_[i][j];

      initialized = true;
      }

   // To compute the new values, we iterate over the rows of A.  If the
   // corresponding element of val is less than zero, then we multiply
   // the entire row by val[i] and add it to newvals.
   //
   // For efficiency, we partially unroll the loop to handle four rows
   // at a time.  This allow the "if ( val[i] < 0 )" test to be conducted
   // on four elements simultaneously.

   ELIMINATION_SSE_ALIGN
   const float * pRow = coefficients;

   for ( int i = 0; i < 4; ++i )
      {
      // Derivative values which are zero or greater than zero result in a
      // product of zero.  Therefore, we only really need to worry about
      // function values less than zero.  Here, we get a bit mask indicating
      // which rows in the current set of four need to be multiplied by the
      // corresponding val[i] and added to newvals.

      register int approxMask = 
         _mm_movemask_ps( _mm_cmplt_ps( values[i], _mm_setzero_ps() ) );

      // For each row that needs to be multiplied, we break the row into
      // two chunks of for floats.  Each chunk is multiplied by the constant
      // val[i] and then added to the corresponding chunk of newvals.

      //--------------------------------------------------

      if ( approxMask & 1 )
         {		
         __m128 c = _mm_splat_ps( values[i], 0 );
         newvals1 = _mm_add_ps( newvals1, _mm_mul_ps( c, _mm_load_ps( pRow )));
         pRow += 4;
         newvals2 = _mm_add_ps( newvals2, _mm_mul_ps( c, _mm_load_ps( pRow )));	
         pRow += 4;
         }

      else
         {
         pRow += 8;
         }

      //--------------------------------------------------

      if ( approxMask & 2 )
         {			
         __m128 c = _mm_splat_ps( values[i], 1 );
         newvals1 = _mm_add_ps( newvals1, _mm_mul_ps( c, _mm_load_ps( pRow )));
         pRow += 4;
         newvals2 = _mm_add_ps( newvals2, _mm_mul_ps( c, _mm_load_ps( pRow )));	
         pRow += 4;
         }

      else
         {			
         pRow += 8;
         }

      //--------------------------------------------------

      if ( approxMask & 4 )
         {			
         __m128 c = _mm_splat_ps( values[i], 2 );
         newvals1 = _mm_add_ps( newvals1, _mm_mul_ps( c, _mm_load_ps( pRow )));
         pRow += 4;
         newvals2 = _mm_add_ps( newvals2, _mm_mul_ps( c, _mm_load_ps( pRow )));	
         pRow += 4;
         }

      else
         {			
         pRow += 8;
         }	

      //--------------------------------------------------

      if ( approxMask & 8 )
         {
         __m128 c = _mm_splat_ps( values[i], 3 );
         newvals1 = _mm_add_ps( newvals1, _mm_mul_ps( c, _mm_load_ps( pRow )));
         pRow += 4;
         newvals2 = _mm_add_ps( newvals2, _mm_mul_ps( c, _mm_load_ps( pRow )));	
         pRow += 4;
         }

      else
         {			
         pRow += 8;
         }
      }

   _mm_storeu_ps( newvals, newvals1 );
   _mm_storeu_ps( newvals + 4, newvals2 );
   }

#endif // SSE

#endif // Elimination_SSE_txx_
