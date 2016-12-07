#ifndef Matrix_h_
#define Matrix_h_

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <exception>
#include <stdexcept>

template< typename T >
class Matrix
   {
   protected:

      const int rows_;
      const int cols_;
      T * data_;

   public:

      Matrix( int rows, int cols ) :
            rows_( rows ),
            cols_( cols )
         {
	 if ( rows <= 0 || cols <= 0 ) 
             throw std::invalid_argument("invalid matrix dimensions");

         this->data_ = (T*)calloc( 
		static_cast< size_t >( this->rows_ ) * 
		static_cast< size_t >( this->cols_ ), sizeof( T ) );

         if ( this->data_ == NULL ) throw std::bad_alloc();
         }

      Matrix( const Matrix< T >& that ) :
            rows_( that.rows_ ),
            cols_( that.cols_ )
         {
         this->data_ = (T*)malloc( this->rows_ * this->cols_ * sizeof( T ) );
         if ( this->data_ == NULL ) throw std::bad_alloc();

         memcpy( this->data_, that.data_, 
		static_cast< size_t >( this->rows_ ) * 
		static_cast< size_t >( this->cols_ ) * sizeof( T ) );
         }

      ~Matrix()
         {
         free( this->data_ );
         }

      int rows() const
         {
         return this->rows_;
         }

      int cols() const
         {
         return this->cols_;
         }

      T* operator[]( int row )
         {
         assert( row < this->rows_ );
         return this->data_ + row * this->cols_;
         }

      const T* operator[]( int row ) const
         {
         assert( row < this->rows_ );
         return this->data_ + row * this->cols_;
         }

      void fill( const T value )
         {
         for ( int i = 0; i < this->rows_; ++i )
            for ( int j = 0; j < this->cols_; ++j )
               (*this)[i][j] = value;
         }
   };

template< typename T, int rowsT, int colsT >
class FixedMatrix
   {
   protected:

      T data_[rowsT * colsT];

   public:

      FixedMatrix()
         {
         memset( data_, 0,
            static_cast< size_t >( rowsT ) * 
            static_cast< size_t >( colsT ) * sizeof( T ) );		
         }

      FixedMatrix( const T* const data )
         {
         memcpy( this->data_, data, sizeof(T) * 
           static_cast< size_t >( rowsT ) * 
           static_cast< size_t >( colsT ) );
         }

      int rows() const
         {
         return rowsT;
         }

      int cols() const
         {
         return colsT;
         }

      T* operator[]( int row )
         {
         assert( row < rowsT );
         return this->data_ + row * colsT;
         }

      const T* operator[]( int row ) const
         {
         assert( row < rowsT );
         return this->data_ + row * colsT;
         }
   };

template< typename T >
class PaddedMatrix
   {
   //
   // Avoids boundary checking by adding extra elements at the beginning 
   // and end of each row and column.  
   // It's okay to read from these locations (they are all zero), 
   // but your code really shouldn't be writing to them.
   //

   protected:

      const int rows_;
      const int cols_;
      T* data_;

   public:

      PaddedMatrix( int rows, int cols ) :
            rows_( rows ),
            cols_( cols )
         {
	 if ( rows <= 0 || cols <= 0 ) 
            throw std::invalid_argument("invalid matrix dimensions");

         this->data_ = (T*)calloc( 
            ( static_cast< size_t >( this->rows_ ) + 2 ) * 
            ( static_cast< size_t >( this->cols_ ) + 2 ), sizeof( T ) );

         if ( this->data_ == NULL ) throw std::bad_alloc();
         }

      PaddedMatrix( const PaddedMatrix< T >& that ) :
            rows_( that.rows_ ),
            cols_( that.cols_ )
         {
         this->data_ = (T*)calloc( 
            ( this->rows_ + 2 ) * ( this->cols_ + 2 ) , sizeof( T ) );

         if ( this->data_ == NULL ) throw std::bad_alloc();

         memcpy( this->data_, that.data_, 
            ( this->rows_ + 2 ) * ( this->cols_ + 2 ) * sizeof( T ) );
         }

      PaddedMatrix( const Matrix< T >& that ) :
            rows_( that.rows() ),
            cols_( that.cols() )
         {
         this->data_ = (T*)calloc( 
            ( static_cast< size_t >( this->rows_ ) + 2 ) * 
            ( static_cast< size_t >( this->cols_ ) + 2 ), 
            sizeof( T ) );

         if ( this->data_ == NULL ) throw std::bad_alloc();

         for ( int row = 0; row < this->rows_; ++row )
            for ( int col = 0; col < this->cols_; ++col )
               (*this)[row][col] = that[row][col];
         }

      ~PaddedMatrix()
         {
         free( this->data_ );
         }		

      PaddedMatrix< T >&
      operator = ( const PaddedMatrix< T >& that )
         {
         assert( this->rows_ == that.rows_ );
         assert( this->cols_ == that.cols_ );

         memcpy( this->data_, that.data_, 
             ( static_cast< size_t >( this->rows_ ) + 2 ) * 
             ( static_cast< size_t >( this->cols_ ) + 2 ) * 
             sizeof( T ) );

         return *this;
         }

      int rows() const
         {
         return this->rows_;
         }

      int cols() const
         {
         return this->cols_;
         }		

      void fill( const T value )
         {
         for ( unsigned int i = 0; i < this->rows_; ++i )
            for ( unsigned int j = 0; j < this->cols_; ++j )
               (*this)[i][j] = value;
         }

      T* operator[]( int row )
         {
         assert( row >= -1 );
         assert( row <= this->rows_ );
         return this->data_ + ( row + 1 ) * ( this->cols_ + 2 ) + 1;
         }

      const T* operator[]( int row ) const
         {
         assert( row >= -1 );
         assert( row <= this->rows_ );
         return this->data_ + ( row + 1 ) * ( this->cols_ + 2 ) + 1;
         }		
   };


#endif // Matrix_h_
