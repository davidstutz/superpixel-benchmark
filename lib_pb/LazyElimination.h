#ifndef LazyElimination_h_
#define LazyElimination_h_

#include "Elimination.h"
#include "graph.h"

template< typename T >
class LazyElimination : public Elimination< T >
   {
   protected:
      // Define the dimensions of the problem
      const int idim_;
      const int jdim_;

      Matrix< T >& A_;	// Unary terms
      Matrix< T >& U_;	// Binary term - Up
      Matrix< T >& R_;	// Binary term - Right
      Matrix< T >& D_;	// Binary term - Down
      Matrix< T >& L_;	// Binary term - Left

      typedef Graph< T, T, T> GraphType;
      GraphType * g_;

      Matrix< bool > eliminated_;		// Tells if a node is alive
      Matrix< unsigned char > levels_;		// Determines the level of nodes

      // Flag to control lazy elimination or not
      const bool lazy_elimination_;

   protected:

      // Accessing these nodes
      void addA( int i, int j, T val )
         { 
         if ( i < 0 || i >= this->idim_ ||
            j < 0 || j >= this->jdim_ ) return;
         A_[i][j] += val;
         }

      void addU( int i, int j, T val )
         {
         if ( i < 0 || i >= this->idim_ ||
            j < 0 || j >= this->jdim_ ) return;
         U_[i][j] += val;
         }

      void addR( int i, int j, T val )
         {
         if ( i < 0 || i >= this->idim_ ||
            j < 0 || j >= this->jdim_ ) return;
         R_[i][j] += val;
         }

      void addD( int i, int j, T val )
         {
         if ( i < 0 || i >= this->idim_ ||
            j < 0 || j >= this->jdim_ ) return;
         D_[i][j] += val;
         }

      void addL( int i, int j, T val)
         {
         if ( i < 0 || i >= this->idim_ ||
            j < 0 || j >= this->jdim_ ) return;
         L_[i][j] += val;
         }

      // Main methods
      void eliminate_node (int i, int j, int level, bool lazily );
      void switch_binary_terms ();

      inline bool node_exists (int i, int j) const
         {
         return ( i >=0 && i < this->idim_ && 
                  j >=0 && j < this->jdim_ && 
                  !this->eliminated_[i][j] );
         }

      void define_graph ();

      unsigned char back_substitute( 
         Matrix< unsigned char >& xStar, int i, int j );

      void init_levels ();

      //---------------------------------------------------------------------
      // Interface to the Boykov maxflow code
      void init_graphcuts();

      inline int nodenum( int i, int j ) const
         {
         return j + jdim_ * i;
         }

      inline void add_tweights (int i, int j, T val) const
         { 
         g_->add_tweights( nodenum(i, j), val, (T)0 );
         }

      inline void add_edge(int i1, int j1, int i2, int j2, T val) const
         {
         g_->add_edge( nodenum(i1, j1), nodenum(i2, j2), (T)0, val );
         }

      // How to get the level of a node
      unsigned char level_of_node (int i, int j) const
         {
         return levels_[i][j];
         }

   public:

      LazyElimination( Matrix< T >& A, 
         Matrix< T >& U,
         Matrix< T >& R,
         Matrix< T >& D,
         Matrix< T >& L,
         bool lazy_elimination = true ) :
            idim_( A.rows() ),
            jdim_( A.cols() ),
            A_( A ),
            U_( U ),
            R_( R ),
            D_( D ),
            L_( L ),
            g_( NULL ),
            eliminated_( A.rows(), A.cols() ),
            levels_( A.rows(), A.cols() ),
	    lazy_elimination_ (lazy_elimination)

         {
         // Initialize the levels array
         this->init_levels();

         // And the "isEliminated" flags
         this->eliminated_.fill( false );
         }

      virtual ~LazyElimination()
         { 
         if (g_) delete g_;
         }

      void segment( Matrix< unsigned char >& X_Star );

      // The static interface
      static void solve( 
         const Matrix< T >& A, 
         const Matrix< T >& B_right, 
         const Matrix< T >& B_down, 
         Matrix< unsigned char >& X_star );

   };

#endif // LazyElimination_h_
