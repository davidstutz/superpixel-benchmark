#ifndef Elimination_txx_
#define Elimination_txx_

#include "Elimination.h"
#include "Elimination+SSE.txx"
#include <stdlib.h>

/*
template< typename T >
void Elimination< T >::computeAPseudoInverseTranspose() 
   {
   // This is a matrix to solve a set of linear equations
   rhMatrix A(16, 9);
   int count = 0;
   for (int i4=0; i4<=1; i4++)   
   for (int i3=0; i3<=1; i3++)   
   for (int i2=0; i2<=1; i2++)   
   for (int i1=0; i1<=1; i1++)   
      {
      A[count][0] = i1;
      A[count][1] = i2;
      A[count][2] = i3;
      A[count][3] = i4;
      A[count][4] = i1*i2;
      A[count][5] = i2*i3;
      A[count][6] = i3*i4;
      A[count][7] = i4*i1;
      A[count][8] = 1.0;
   
      count++;
      }

   // Now, form the pseudo-inverse
   rhMatrix B = ( (A.transpose() * A).inverse() * A.transpose() ).transpose();
   for ( int row = 0; row < 16; ++row )
      {
      for ( int col = 0; col < 8; ++col )
         printf( "%f, ", B[row][col] );

      printf( "\n" );
      }
   }
*/


template< typename T >
void Elimination< T >::compute_coefficients( const T *a, T * newvals) const
   {
   // At the input, a is a matrix consisting of the values of the cost function
   // in a 4-neighbourhood.
   // newvals will be returned as the values of the terms in the 
   // reduced function.

   // We fill out the vector.  This gives the values
   // of the function for all possible assignments to the variables x_i

   T val[16];
   val[0]  = a[0];
   val[1]  = val[0] + a[1];
   val[2]  = val[0] + a[2];
   val[3]  = val[1] + a[2];
   val[4]  = val[0] + a[3];
   val[5]  = val[1] + a[3];
   val[6]  = val[2] + a[3];
   val[7]  = val[3] + a[3];
   val[8]  = val[0] + a[4];
   val[9]  = val[1] + a[4];
   val[10] = val[2] + a[4];
   val[11] = val[3] + a[4];
   val[12] = val[4] + a[4];
   val[13] = val[5] + a[4];
   val[14] = val[6] + a[4];
   val[15] = val[7] + a[4];

   // We split into two ways of doing this, depending on
   // expected number of positive values

   // New values for the variables
   memset( newvals, 0, 8 * sizeof( T ) );

   // Now, if the value is greater than zero, set to zero
   for ( int i = 0; i < 16; ++i )
      {
      if ( val[i] > (T)0 ) continue;

      // And add the appropriate row of APseudoInverseTranspose
      for ( int j = 0; j < 8; ++j )
         newvals[j] += val[i] * this->APseudoInverseTranspose_[i][j];
      }	
   }


template< typename T >
void Elimination< T >::traverse1( 
        const PaddedMatrix< T >& A,
        const PaddedMatrix< T >& B_right, 
        const PaddedMatrix< T >& B_down,
        PaddedMatrix< T >& A2,
        PaddedMatrix< T >& B2_dr,
        PaddedMatrix< T >& B2_ur )
   {
   const int idim = A.rows();
   const int jdim = A.cols();

   // Traverses the matrix and generates the skew array
   // It is assumed that the image has a border of size 1, 
   // so that we can index without worries.

   // Declare the arrays we need
   T a[5];
   T b[8];

   assert( A2.rows() == A.rows() );
   assert( A2.cols() == A.cols() );
   assert( B2_dr.rows() == A.rows() );
   assert( B2_dr.cols() == A.cols() );
   assert( B2_ur.rows() == A.rows() );
   assert( B2_ur.cols() == A.cols() );

   // Initialize the unary terms
   A2 = A;
   /*
   for ( unsigned int row = 0; row < A.rows(); ++row )
   for ( unsigned int col = 0; col < A.cols(); ++col )
   A2[row][col] = A[row][col];
   */

   // We delete the elements with (i+j) odd
   for ( int i = 0; i < idim; ++i )
      {     
      for ( int j = (i+1) % 2; j < jdim; j += 2 )
         {       
         // Form the vector of neighbor values.
         // The 2nd order terms go clockwise from 12 o'clock
         a[0] = A[i][j];
         a[1] = B_down[i-1][j];
         a[2] = B_right[i][j];
         a[3] = B_down[i][j];
         a[4] = B_right[i][j-1];

         // Now, get the new values
         this->compute_coefficients( a, b );

//         if ( this->verbose_ )
//            {
//            printf("Eliminated (%d,%d)\n",i,j);
//            printf("   %f %f %f %f %f => %f %f %f %f %f %f %f %f\n",
//               a[0],a[1],a[2],a[3],a[4],
//               b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7]);
//            }

         // Now, store them in the new image
         // The A array simply overwrites the old one
         A2[i-1][j] += b[0]; 
         A2[i][j+1] += b[1]; 
         A2[i+1][j] += b[2]; 
         A2[i][j-1] += b[3]; 

         // The new second-order terms must be in a new array
         // These hold the terms as indicated (where xu, xr, xd, xl
         // represent the nodes up, right, down and left)
         // General idea is as follows.  The nodes being removed (i.e. i+j odd)
         // hold the terms for the nodes down and to the right,
         // whereas nodes left (i.e. i+j even) hold the terms for
         // the node itself and the one down to the right.
         // We need to accumulate, since each edge gets a contribution 
         // from two places.
         B2_dr[i-1][j] += b[4];  // xu xr
         B2_ur[i+1][j] += b[5];  // xd xr
         B2_dr[i][j-1] += b[6];  // xl xd
         B2_ur[i][j-1] += b[7];  // xl xu
         }
      }
   }


template< typename T >
void Elimination< T >::traverse2 ( 
        const PaddedMatrix< T >&A,
        const PaddedMatrix< T >& B_dr, 
        const PaddedMatrix< T >& B_ur, 
        PaddedMatrix< T >& Anew,
        PaddedMatrix< T >& B_right,
        PaddedMatrix< T >& B_down )
   {
   const int idim = A.rows();
   const int jdim = A.cols();
   const int pdim = Anew.rows();
   const int qdim = Anew.cols();

   // We do the second stage traverse

   // Declare the arrays we need
   T a[5];
   T b[8];

   assert( Anew.rows() == pdim );
   assert( Anew.cols() == qdim );
   assert( B_right.rows() == pdim );
   assert( B_right.cols() == qdim );
   assert( B_down.rows() == pdim );
   assert( B_down.cols() == qdim );

   // Initialize Anew
   for ( int p = 0; p < pdim; ++p )
      for ( int q = 0; q < qdim; ++q )
         // Initialize with the old values
         Anew[p][q] = A[2*p][2*q];

   // Now, delete the remaining values in the odd rows and columns
   for ( int i = 1; i < idim; i += 2 )
      for ( int j = 1; j < jdim; j += 2 )
         {
         // We will be deleting the remaining values in odd numbered
         // rows and columns.

         // Fill the array of cost coefficients
         a[0] = A[i][j];
         a[1] = B_dr[i-1][j-1];
         a[2] = B_ur[i][j];
         a[3] = B_dr[i][j];
         a[4] = B_ur[i+1][j-1];

         // Compute the new values
         this->compute_coefficients( a, b );

//         if ( this->verbose_ )
//            {
//            printf("Eliminated (%d,%d)\n",i,j);
//            printf("   %f %f %f %f %f => %f %f %f %f %f %f %f %f\n",
//               a[0],a[1],a[2],a[3],a[4],
//               b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7]);
//            }

         // Now, store them in the new image
         Anew[(i-1)/2][(j-1)/2] += b[0]; 
         Anew[(i-1)/2][(j+1)/2] += b[1]; 
         Anew[(i+1)/2][(j+1)/2] += b[2]; 
         Anew[(i+1)/2][(j-1)/2] += b[3]; 

         // Second order terms
         B_right[(i-1)/2][(j-1)/2] += b[4];
         B_down [(i-1)/2][(j+1)/2] += b[5];
         B_right[(i+1)/2][(j-1)/2] += b[6];
         B_down [(i-1)/2][(j-1)/2] += b[7];
         }
   }


template< typename T >
void Elimination< T >::optimize ( 
        const PaddedMatrix< T >& A, 
        const PaddedMatrix< T >& B_right, 
        const PaddedMatrix< T >& B_down, 
        PaddedMatrix< unsigned char >& X_star 
        )
   {
   // This is going to be a recursive procedure to solve
   // clock_t start, finish;

   // Get the dimensions of the actual data
   const int idim = A.rows();
   const int jdim = A.cols();

   // Initialize the segmentation array
   //Seg.Init(-1, idim, -1, jdim);
   assert( (int)X_star.rows() == idim );
   assert( (int)X_star.cols() == jdim );

   // Stop the recursion
   //if ( idim <= 4 || jdim <= 4 )
   if ( idim == 1 && jdim == 1 )
      {
      for ( int i = 0; i < idim; ++i )
         for ( int j = 0; j < jdim; ++j )
            {
            X_star[i][j] = ( A[i][j] < (T)0 ) ? 1 : 0;
//            if ( this->verbose_ ) 
//               printf("Solved (%d,%d)=%d\n",i,j,X_star[i][j]);
            }
         return;
      }
      
   // Otherwise, we need to recurse
   const int pdim = (int)( idim / 2.0 + 0.5 );
   const int qdim = (int)( jdim / 2.0 + 0.5 );

   PaddedMatrix< T > A2( idim, jdim );
   PaddedMatrix< T > B2_ur( idim, jdim );
   PaddedMatrix< T > B2_dr( idim, jdim );

   PaddedMatrix< T > A3( pdim, qdim );
   PaddedMatrix< T > B3_r( pdim, qdim );
   PaddedMatrix< T > B3_d( pdim, qdim );
   
   // Do two passes
   //clock_t start = clock();
  
   traverse1( A, B_right,  B_down,  A2, B2_dr, B2_ur );  
   traverse2( A2, B2_dr, B2_ur, A3, B3_r,  B3_d  );

   //clock_t finish = clock();
   //printf ("Traverse %2.5f\n", (double)(finish - start) / CLOCKS_PER_SEC);

   PaddedMatrix< unsigned char > x4Star( pdim, qdim );
   this->optimize( A3, B3_r, B3_d, x4Star );

   // Timing phase 2
   // start = clock();

   // Fill in the even points from Seg_new
   for ( int p = 0; p < pdim; ++p )
      for ( int q = 0; q < qdim; ++q )
         X_star[2*p][2*q] = x4Star[p][q];

   // Now, the centres of the squares
   for (int i = 1; i < idim; i += 2 )
      for (int j = 1; j < jdim; j += 2 )
         {
         // Must evaluate the coefficient
         T val= A2[i][j];
         val += X_star[i-1][j-1] * B2_dr[i-1][j-1];
         val += X_star[i-1][j+1] * B2_ur[i][j];
         val += X_star[i+1][j-1] * B2_ur[i+1][j-1];
         val += X_star[i+1][j+1] * B2_dr[i][j];

         // Fill in the segmentation value
         X_star[i][j] = ( val < (T)0) ? 1 : 0;
//         if ( this->verbose_ )
//            {
//            printf("BackSub (%d,%d)=%d\n",i,j,X_star[i][j]);
//            printf("   %f = %f + %f + %f + %f\n",
//               val, 
//               B2_dr[i-1][j-1], B2_ur[i][j], 
//               B2_ur[i+1][j-1], B2_dr[i][j]);
//            }
         }

   // Now, the remaining pixels
   for ( int i = 0; i < idim; ++i )
      for ( int j = (i+1) % 2; j < jdim; j += 2 )
         {
         // Must evaluate the coefficient
         T val= A[i][j];
         val += X_star[i-1][j] * B_down [i-1][j];
         val += X_star[i][j+1] * B_right[i][j];
         val += X_star[i+1][j] * B_down[i][j];
         val += X_star[i][j-1] * B_right[i][j-1];

         // Fill in the segmentation value
         X_star[i][j] = ( val < (T)0 ) ? 1 : 0;
//         if ( this->verbose_ )
//            {
//            printf("BackSub (%d,%d)=%d\n",i,j,X_star[i][j]);
//            printf("   %f = %f + %f + %f + %f\n",
//               val, 
//               B_down[i-1][j], B_right[i][j], 
//               B_down[i][j], B_right[i][j-1]);
//            }
         }
   }


template< typename T >
void Elimination< T >::solve( 
        const Matrix< T >& A,
        const Matrix< T >& B_right, 
        const Matrix< T >& B_down, 
        Matrix< unsigned char >& X_star 
        )
   { 
   Elimination< T > elim;
   
   PaddedMatrix< T > A_( A );
   PaddedMatrix< T > Br_( B_right );
   PaddedMatrix< T > Bd_( B_down ); 
   PaddedMatrix< unsigned char > Xs_( X_star );

   elim.optimize( A_, Br_, Bd_, Xs_ );

   for ( int row = 0; row < A.rows(); ++row )
      for ( int col = 0; col < A.cols(); ++col )
         X_star[row][col] = Xs_[row][col];
   }


template<>
void Elimination< double >::initialize()
   {
   double data[16*8] = {
      -0.375, -0.375, -0.375, -0.375,  0.250,  0.250,  0.250,  0.250, 
       0.375, -0.125, -0.375, -0.125, -0.250,  0.250,  0.250, -0.250, 
      -0.125,  0.375, -0.125, -0.375, -0.250, -0.250,  0.250,  0.250, 
       0.125,  0.125, -0.125, -0.125,  0.250, -0.250,  0.250, -0.250, 
      -0.375, -0.125,  0.375, -0.125,  0.250, -0.250, -0.250,  0.250, 
       0.375,  0.125,  0.375,  0.125, -0.250, -0.250, -0.250, -0.250, 
      -0.125,  0.125,  0.125, -0.125, -0.250,  0.250, -0.250,  0.250, 
       0.125, -0.125,  0.125,  0.125,  0.250,  0.250, -0.250, -0.250, 
      -0.125, -0.375, -0.125,  0.375,  0.250,  0.250, -0.250, -0.250, 
       0.125, -0.125, -0.125,  0.125, -0.250,  0.250, -0.250,  0.250, 
       0.125,  0.375,  0.125,  0.375, -0.250, -0.250, -0.250, -0.250, 
      -0.125,  0.125,  0.125,  0.125,  0.250, -0.250, -0.250,  0.250, 
      -0.125, -0.125,  0.125,  0.125,  0.250, -0.250,  0.250, -0.250, 
       0.125,  0.125,  0.125, -0.125, -0.250, -0.250,  0.250,  0.250, 
       0.125,  0.125, -0.125,  0.125, -0.250,  0.250,  0.250, -0.250, 
      -0.125, -0.125, -0.125, -0.125,  0.250,  0.250,  0.250,  0.250
      };
    
   this->APseudoInverseTranspose_ = FixedMatrix< double, 16, 8 >( data );
   }


template<>
void Elimination< float >::initialize()
   {
   float data[16*8] = {
      -0.375f, -0.375f, -0.375f, -0.375f,  0.250f,  0.250f,  0.250f,  0.250f, 
       0.375f, -0.125f, -0.375f, -0.125f, -0.250f,  0.250f,  0.250f, -0.250f, 
      -0.125f,  0.375f, -0.125f, -0.375f, -0.250f, -0.250f,  0.250f,  0.250f, 
       0.125f,  0.125f, -0.125f, -0.125f,  0.250f, -0.250f,  0.250f, -0.250f, 
      -0.375f, -0.125f,  0.375f, -0.125f,  0.250f, -0.250f, -0.250f,  0.250f, 
       0.375f,  0.125f,  0.375f,  0.125f, -0.250f, -0.250f, -0.250f, -0.250f, 
      -0.125f,  0.125f,  0.125f, -0.125f, -0.250f,  0.250f, -0.250f,  0.250f, 
       0.125f, -0.125f,  0.125f,  0.125f,  0.250f,  0.250f, -0.250f, -0.250f, 
      -0.125f, -0.375f, -0.125f,  0.375f,  0.250f,  0.250f, -0.250f, -0.250f, 
       0.125f, -0.125f, -0.125f,  0.125f, -0.250f,  0.250f, -0.250f,  0.250f, 
       0.125f,  0.375f,  0.125f,  0.375f, -0.250f, -0.250f, -0.250f, -0.250f, 
      -0.125f,  0.125f,  0.125f,  0.125f,  0.250f, -0.250f, -0.250f,  0.250f, 
      -0.125f, -0.125f,  0.125f,  0.125f,  0.250f, -0.250f,  0.250f, -0.250f, 
       0.125f,  0.125f,  0.125f, -0.125f, -0.250f, -0.250f,  0.250f,  0.250f, 
       0.125f,  0.125f, -0.125f,  0.125f, -0.250f,  0.250f,  0.250f, -0.250f, 
      -0.125f, -0.125f, -0.125f, -0.125f,  0.250f,  0.250f,  0.250f,  0.250f
      };
    
   this->APseudoInverseTranspose_ = FixedMatrix< float, 16, 8 >( data );
   }

#endif // Elimination_txx_
