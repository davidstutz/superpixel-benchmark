#ifndef LazyElimination_txx_
#define LazyElimination_txx_

#include "LazyElimination.h"

template< typename T >
void LazyElimination< T >::eliminate_node(int i, int j, int level, bool lazily )
   {
   //
   // Eliminates node (i,j) 
   // The method may be called in a lazy or active fashion.  
   // In the active state, the node is always eliminated; 
   // in the lazy state, the node is only eliminated if any of its edges 
   // are positive.  A node can only be eliminated once its
   // parents have been eliminated.  As a result, a node being eliminated lazily
   // may actively eliminate its parents.
   //

   // check boundary conditions, and whether this node has already 
   // been eliminated
   if ( node_exists( i, j ) == false ) return;

   // Do not eliminate the node if all its edges are negative, and this
   // invocation is not coming from a child that has negative edges.
   if ( lazily && lazy_elimination_ &&
      U_[i][j] <= (T)0 &&
      R_[i][j] <= T(0) && 
      D_[i][j] <= (T)0 &&
      L_[i][j] <= T(0) ) return;

   // make sure parents have been eliminated
   // recall nodes at level zero have no parents
   if ( level > 0 )
      {
      // The location of the parents depends on the location and level
      // of the current node.

      // Even Level
      if ( level % 2 == 0 )
         {
         int step = 1 << ( level - 2 ) / 2;

         // parents are diagonally NW/NE/SE/SW
         eliminate_node( i - step, j - step, level - 1, false );
         eliminate_node( i - step, j + step, level - 1, false );
         eliminate_node( i + step, j + step, level - 1, false );
         eliminate_node( i + step, j - step, level - 1, false );
         }

      // Odd Level
      else
         {
         int step = 1 << ( level - 1 ) / 2;

         // parents are directly N/E/S/W
         eliminate_node( i - step, j, level - 1, false);
         eliminate_node( i, j + step, level - 1, false );
         eliminate_node( i + step, j, level - 1, false );
         eliminate_node( i, j - step, level - 1, false );
         }
      }

   // re-check that node edges still justify eliminating.
   if ( lazily &&
      lazy_elimination_ &&
      U_[i][j] <= (T)0 &&
      R_[i][j] <= T(0) && 
      D_[i][j] <= (T)0 &&
      L_[i][j] <= T(0) ) return;

   // Mark this node as eliminated
   this->eliminated_[i][j] = true;

   // Compute the approximation of eliminating this node
   T a[5];
   a[0] = A_[i][j];
   a[1] = U_[i][j];
   a[2] = R_[i][j];
   a[3] = D_[i][j];
   a[4] = L_[i][j];
   T b[8];
   this->compute_coefficients(a, b);

//   if ( this->verbose_ )
//      {
//      printf("Eliminated (%d,%d)\n",i,j);
//      printf("   %f %f %f %f %f => %f %f %f %f %f %f %f %f\n",
//         a[0],a[1],a[2],a[3],a[4],
//         b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7]);
//      }

   //------------------------------------------------------------------------
   //
   //                    Push weight to children
   //
   //------------------------------------------------------------------------

   //------------------------------------------------------------------------
   //                       Even level edges
   //------------------------------------------------------------------------   
   if (level % 2 == 0)
      {

      int step = 1 << (level/2);

      // Push the unary terms to the children
      addA(i-step, j,      b[0]); 
      addA(i,      j+step, b[1]); 
      addA(i+step, j,      b[2]); 
      addA(i,      j-step, b[3]); 

      // Push the edge terms to the children.  The children are
      // the next adjacent nodes to be eliminated
      // We need to accumulate, since each edge gets a contribution 
      // from two places.
      // Note the U, R, L and D edges from nodes at odd level represent
      // the edges rotated 45 degrees (anticlockwise) from the usual U,R,L,D

      if ( (i/step) % 2 == 1 )
         {
         addU(i, j+step, b[4]);  // xu xr
         addL(i, j+step, b[5]);  // xd xr
         addD(i, j-step, b[6]);  // xl xd
         addR(i, j-step, b[7]);  // xl xu
         }

      else
         {
         addD(i-step, j, b[4]);  // xu xr
         addR(i+step, j, b[5]);  // xd xr
         addU(i+step, j, b[6]);  // xl xd
         addL(i-step, j, b[7]);  // xl xu
         }
      }

   //------------------------------------------------------------------------
   //                       Odd level edges
   //------------------------------------------------------------------------
   else
      {
      int step = 1<<((level-1)/2);

      // Push the unary terms to the children
      addA(i-step, j-step, b[0]); 
      addA(i-step, j+step, b[1]); 
      addA(i+step, j+step, b[2]); 
      addA(i+step, j-step, b[3]); 

      // Push the binary terms to the children
      if ( (i+j) / (2*step) % 2 == 0 )
         {
         addR(i-step, j-step, b[4]);
         addU(i+step, j+step, b[5]);
         addL(i+step, j+step, b[6]);
         addD(i-step, j-step, b[7]);
         }

      else
         {
         addL(i-step, j+step, b[4]);
         addD(i-step, j+step, b[5]);
         addR(i+step, j-step, b[6]);
         addU(i+step, j-step, b[7]);
         }
      }
   }


template< typename T >
void LazyElimination< T >::switch_binary_terms()
   {
   // Change the binary terms in x.y to terms in x.bary, where x is the parent

   // Simply run over the unary terms and use the equality 
   // x . y = x.(1 - bary) = x - x.bary

   for ( int i = 0; i < this->idim_; ++i )
      for ( int j = 0; j < this->jdim_; ++j )
         {
         A_[i][j] += U_[i][j] + L_[i][j] + D_[i][j] + R_[i][j];
         U_[i][j] = -U_[i][j];
         R_[i][j] = -R_[i][j];
         D_[i][j] = -D_[i][j];
         L_[i][j] = -L_[i][j];
         }
   }


template< typename T >
void LazyElimination< T >::define_graph()
   {
   // We now assume that all edges are positive so that we can form the graph
   // and solve by maxflow

   // Now, run through the unary terms and set them
   for ( int i = 0; i < this->idim_; ++i )
      for ( int j = 0; j < this->jdim_; ++j )
         if ( !this->eliminated_[i][j] ) add_tweights( i,j, A_[i][j] );

   // Now do the binary terms
   for ( int i = 0; i < this->idim_; ++i )
      for ( int j = 0; j < this->jdim_; ++j )
         {
         // If this node is eliminated, then its edges are also
         if ( this->eliminated_[i][j] ) continue;

         // Otherwise, we need to find the level
         int level = level_of_node(i, j);

         // Deal with even and odd levels independently
         if (level % 2 == 0)  // Even levels
            {
            int step = 1<<(level/2);	// This is the distance to children
            if (node_exists(i-step, j)) add_edge(i,j, i-step, j, U_[i][j]);
            if (node_exists(i+step, j)) add_edge(i,j, i+step, j, D_[i][j]);
            if (node_exists(i, j-step)) add_edge(i,j, i, j-step, L_[i][j]);
            if (node_exists(i, j+step)) add_edge(i,j, i, j+step, R_[i][j]);
            }

         else	// Odd levels
            {
            int step = 1<<((level-1)/2);   // This is the distance to children

            if (node_exists(i-step,j-step)) 
               add_edge(i,j, i-step, j-step, U_[i][j]);

            if (node_exists(i-step,j+step)) 
               add_edge(i,j, i-step, j+step, R_[i][j]);

            if (node_exists(i+step,j+step)) 
               add_edge(i,j, i+step, j+step, D_[i][j]);

            if (node_exists(i+step,j-step)) 
               add_edge(i,j, i+step, j-step, L_[i][j]);
            }
         }
   }


template< typename T >
unsigned char LazyElimination< T >::back_substitute( 
        Matrix< unsigned char >& X_Star, 
        int i, int j )
   {
   // Back substitute the values

   // Value when out of range
   if ( i < 0 || i >= this->idim_ || j < 0 || j >= this->jdim_)
      {
      return this->lazy_elimination_ ? 1 : 0;
      }

   // If it has been done already, then return
   if ( X_Star[i][j] < 2 ) return X_Star[i][j];

   // Get the level of this node
   int level = this->level_of_node(i, j);

   // Even level first
   if (level % 2 == 0)
      {
      // Get the distance to parents and children
      int step = 1<<(level/2);	// This is the distance to children

      // Back substitute the children first
      T val = A_[i][j];
      if ( this->lazy_elimination_ )
         {
         if ( i - step >= 0 )
            {
            assert( X_Star[i-step][j] < 2 );
            val += U_[i][j] * ( 1 - X_Star[i-step][j] );
            }

         if ( j + step < jdim_ )
            {
            assert( X_Star[i][j+step] < 2 );
            val += R_[i][j] * ( 1 - X_Star[i][j+step] );
            }

         if ( i + step < idim_ )
            {
            assert( X_Star[i+step][j] < 2 );
            val += D_[i][j] * ( 1 - X_Star[i+step][j] );
            }

         if ( j - step >= 0 )
            {
            assert( X_Star[i][j-step] < 2 );
            val += L_[i][j] * ( 1 - X_Star[i][j-step] );
            }

         /*
         val += U_[i][j] * (1 - back_substitute (X_Star, i-step, j));
         val += R_[i][j] * (1 - back_substitute (X_Star, i, j+step));
         val += D_[i][j] * (1 - back_substitute (X_Star, i+step, j));
         val += L_[i][j] * (1 - back_substitute (X_Star, i, j-step));
         */
         }

      else
         {
         val += U_[i][j] * back_substitute (X_Star, i-step, j);
         val += R_[i][j] * back_substitute (X_Star, i, j+step);
         val += D_[i][j] * back_substitute (X_Star, i+step, j);
         val += L_[i][j] * back_substitute (X_Star, i, j-step);
         }

      X_Star[i][j] = ( val < (T)0 ) ? 1 : 0;
//      if ( this->verbose_ )
//         {
//         printf("BackSub (%d,%d)=%d\n",i,j,X_Star[i][j]);
//         printf("   %f = %f + %f + %f + %f + %f\n", 
//            val, A_[i][j], U_[i][j], R_[i][j], D_[i][j], L_[i][j]);
//         }

      return X_Star[i][j];
      }

   else
      {
      // Get the distance to parents and children
      int step = 1<<((level-1)/2);	// This is the distance to children

      // Push the unary terms to the children
      T val = A_[i][j];
      if ( this->lazy_elimination_ )
         {

         if ( i - step >= 0 && j - step >= 0 )
            {
            assert( X_Star[i-step][j-step] < 2 );
            val += U_[i][j] * ( 1 - X_Star[i-step][j-step] );
            }

         if ( i - step >= 0 && j + step < jdim_ )
            {
            assert( X_Star[i-step][j+step] < 2 );
            val += R_[i][j] * ( 1 - X_Star[i-step][j+step] );
            }

         if ( i + step < idim_ && j + step < jdim_ )
            {
            assert( X_Star[i+step][j+step] < 2 );
            val += D_[i][j] * ( 1 - X_Star[i+step][j+step] );
            }

         if ( i + step < idim_ && j - step >= 0 )
            {
            assert( X_Star[i+step][j-step] < 2 );
            val += L_[i][j] * ( 1 - X_Star[i+step][j-step] );
            }

         /*
         val += U_[i][j] * (1 - back_substitute (X_Star, i-step, j-step));
         val += R_[i][j] * (1 - back_substitute (X_Star, i-step, j+step));
         val += D_[i][j] * (1 - back_substitute (X_Star, i+step, j+step));
         val += L_[i][j] * (1 - back_substitute (X_Star, i+step, j-step));
         */
         }

      else
         {
         val += U_[i][j] * back_substitute (X_Star, i-step, j-step);
         val += R_[i][j] * back_substitute (X_Star, i-step, j+step);
         val += D_[i][j] * back_substitute (X_Star, i+step, j+step);
         val += L_[i][j] * back_substitute (X_Star, i+step, j-step);
         }

      X_Star[i][j] = ( val < (T)0 ) ? 1 : 0;
//      if ( this->verbose_ )
//         {
//         printf("BackSub (%d,%d)=%d\n",i,j,X_Star[i][j]);
//         printf("   %f = %f + %f + %f + %f + %f\n", 
//            val, A_[i][j], U_[i][j], R_[i][j], D_[i][j], L_[i][j]);
//         }

      return X_Star[i][j];
      }
   }


template< typename T >
void LazyElimination< T >::init_levels ()
   {
   // Builds the array of levels

   // Initialize the array
   //levels_.Init (0, idim_-1, 0, jdim_-1);

   // Fill out the array
   for (int lev=0; ; lev++)
      {
      int step = 2<<(lev/2);  // The horizontal distance between nodes with
                              // the same label
      int count = 0;

      // The even levels
      if (lev%2 == 0)
         {
         for (int i=0; i<idim_; i+=step/2)
            for (int j= (i+step/2)%step; j<jdim_; j+=step)
               {
               count++;
               levels_[i][j] = static_cast< unsigned char >( lev );
               }
         }

      // The odd levels
      else 
         {
         for (int i=step/2; i<idim_; i+=step)
            for (int j=step/2; j<jdim_; j+=step)
               {
               count++;
               levels_[i][j] = static_cast< unsigned char >( lev );
               }
         }

      // Break if the level is greater than the size
      if (count == 0)
         {
         levels_[0][0] = static_cast< unsigned char >( lev );
         break;
         }
      }

   // Print it out as an image
   //put_array_to_pix ("levels.tif", levels_);
   }


template< typename T >
void LazyElimination< T >::init_graphcuts ()
   {
   // First, initialize the segmentation
   const int buffer = 10;	// Just a few more to make sure I can count
   g_ = new GraphType (idim_*jdim_ + buffer, 2 * idim_*jdim_ + buffer);

   // Set up the nodes and edges
   //for (int i=0; i<idim_*jdim_; i++)
   g_->add_node( idim_*jdim_ );
   }


template< typename T >
void LazyElimination< T >::segment( Matrix< unsigned char >& X_Star )
   {
   // First step, eliminate nodes with positive edge weights.
   // Although the entire lattice could be traversed recursively from a 
   // single call to eliminate node (0,0), the call stack may get too big
   // for sufficiently large graphs.  Instead, we iterate through the levels
   // and try to eliminate nodes at successively higher levels.  If a node
   // needs to be eliminated, there is a good chance its parents will already
   // have been eliminated, keeping recusrion under control.

   // Get the number of levels to run
   int size = std::max( this->idim_, this->jdim_ );
   if (size % 2) ++size;
   unsigned int maxlevel = 0;
   while ( size > 0 )
      {
      maxlevel += 2;
      size /= 2;
      }

   // Now run
   for (int level=0; level<(int)maxlevel; level++)
      {
      int step = 1 << ((level+1)/2);	// Step between rows with elements
      if (level%2 == 0)
         {
         for (int i=0; i<this->idim_; i+=step )
            for (int j=(i+step) % (2*step); j<jdim_; j += 2*step )
               eliminate_node (i, j, level, true);
         }
      else
         {
         for (int i=step/2; i<this->idim_; i+=step)
            for (int j=step/2; j<this->jdim_; j += step )
               eliminate_node (i, j, level, true);
         }
      }

   if ( this->lazy_elimination_ )
      {
      // Next, swap to positive edges
      this->switch_binary_terms();

      // Now, create the graph
      this->init_graphcuts();
      this->define_graph();

      // Solve via graph cut
      this->g_->maxflow();

      // Now, get the values from the maxflow into the configuration 	      
      for ( int i = 0; i < this->idim_; ++i )
         for ( int j = 0; j < this->jdim_; ++j )
            {
            if ( this->eliminated_[i][j] )
               X_Star[i][j] = 2;
            else if ( g_->what_segment(nodenum(i, j)) == GraphType::SINK )
               {
               X_Star[i][j] = 1;
//               if ( this->verbose_ ) 
//                  printf("Solved (%d,%d)=%d\n", i ,j, X_Star[i][j] );
               }
            else
               {
               X_Star[i][j] = 0;
//               if ( this->verbose_ ) 
//                  printf("Solved (%d,%d)=%d\n", i ,j, X_Star[i][j] );
               }
            }
      }

   else
      {
      X_Star.fill( 2 );

      for ( int i = 0; i < this->idim_; ++i )
         for ( int j = 0; j < this->jdim_; ++j )
            { 
            //if ( i == 0 & j == 0 ) continue;
            //assert( eliminated_[i][j] == true );
            if ( !eliminated_[i][j] )
               {
               X_Star[i][j] = ( this->A_[i][j] < (T)0 ) ? 1 : 0;
//               if ( this->verbose_ ) 
//                  printf("Solved (%d,%d)=%d\n", i ,j, X_Star[i][j] );
               }
            }	      
      }


   /*
   // Now, get optimal labels to the eliminated nodes using back substitution
   for ( int i = 0; i < this->idim_; ++i )
   for ( int j = 0; j < this->jdim_; ++j )
   if ( X_Star[i][j] == 2 ) this->back_substitute( X_Star, i, j );
   */

   // Back Substitute, starting from the highest level and working backwards.
   for (int level=(int)maxlevel; level >= 0 ; --level)
      {
      // printf("BackSub Level %d\n", level);

      int step = 1 << ((level+1)/2);	// Step between rows with elements
      if (level%2 == 0)
         {
         for (int i=0; i<this->idim_; i+=step )
            for (int j=(i+step) % (2*step); j<jdim_; j += 2*step )
               if ( X_Star[i][j] == 2 ) this->back_substitute( X_Star, i, j );
         }
      else
         {
         for (int i=step/2; i<this->idim_; i+=step)
            for (int j=step/2; j<this->jdim_; j += step )
               if ( X_Star[i][j] == 2 ) this->back_substitute( X_Star, i, j );
         }
      }
   }


// The static interface
template< typename T >
void LazyElimination< T >::solve( 
        const Matrix< T >& Ain, 
        const Matrix< T >& B_right, 
        const Matrix< T >& B_down, 
        Matrix< unsigned char >& X_Star 
        )
   {
   const int idim = Ain.rows();
   const int jdim = Ain.cols();

   // Make a mutable copy of the unary terms
   Matrix< T > A( idim, jdim );
   for ( int i = 0; i < idim; ++i )
      for ( int j = 0; j < jdim; ++j )
         A[i][j] = Ain[i][j];	

   for ( int i = 0; i < idim; ++i )
      assert( B_right[i][jdim-1] == 0 );

   for ( int j = 0; j < jdim; ++j )
      assert( B_down[idim-1][j] == 0 );

   // Binary terms need to be re-arranged	
   Matrix< T > U( idim, jdim );
   Matrix< T > R( idim, jdim );
   Matrix< T > D( idim, jdim );
   Matrix< T > L( idim, jdim );

   for ( int i = 0; i < idim; ++i )
      for ( int j = (i+1) % 2; j < jdim; j += 2 )
         {
         U[i][j] = ( i > 0 ) ? B_down[i-1][j] : (T)0;
         R[i][j] = B_right[i][j];
         D[i][j] = B_down[i][j];
         L[i][j] = ( j > 0 ) ? B_right[i][j-1] : (T)0;
         }

   // Now, segment
   LazyElimination lazyElim( A, U, R, D, L );
   lazyElim.segment( X_Star );
   }

#endif // LazyElimination_txx_
