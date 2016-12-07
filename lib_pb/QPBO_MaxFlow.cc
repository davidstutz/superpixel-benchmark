#include "QPBO_MaxFlow.h"
#include "graph.h"



MaxFlowQPBO::MaxFlowQPBO( const Matrix< float >& A, 
                          const Matrix< float >& B_right, 
                          const Matrix< float >& B_down, 
                          Matrix< unsigned char >& Seg ) :
    GenericQPBO( A, B_right, B_down, Seg ),
    idim_( A.rows() ),
    jdim_( A.cols() )
{
    // Run a multilabel segment
    // First, initialize the segmentation
    //Seg.Init (0, this->idim_-1, 0, this->jdim_-1);
    Seg.fill(0);

    // Create a graph
    typedef Graph< float, float, float > GraphType;
    GraphType g( this->idim_ * this->jdim_,
                 this->idim_ * ( this->jdim_ - 1 ) +
                 this->jdim_ * ( this->idim_ - 1 ) );
    
    // Add a node for each variable
    g.add_node( this->idim_ * this->jdim_ );
    
    // The unary terms are represented as edges from the source to the node,
    // or from the node to the sink, depending on whether the element of A
    // is positive or negative.  Although the API deals with negative 
    // coefficients, it's fairly straightforward to do this ourselves.
    for (int i=0; i<this->idim_; ++i)
        for (int j=0; j<this->jdim_; ++j)
            {
            // a_{i,j} x_{i,j} = a_{i,j} \bar{0} . x_{i,j} when a_{i,j} >= 0
            if ( A[i][j] >= 0.0f ) g.add_tweights( this->node(i,j), A[i][j], 0.0f );
            else g.add_tweights( this->node(i,j), 0.0f, -A[i][j] );
            }   
    
    // The binary terms are represented as edges between two nodes.
    // However, every term x_{i,j} . x_{p,q} must be converted into the form
    // \bar{x}_{i,j} . x_{p,q} so that it can be associated with an edge from
    // x_{i,j} to x_{p,q}.  When doing this conversion, we generate a term
    // -x_{p,q}.  This is why we do the binary terms first.  Otherwise, we
    // would have to update the unary terms after already setting them once.

    // right
    for (int i=0; i<this->idim_; ++i)
        for (int j=0; j<this->jdim_-1; ++j)
        {
            // if the coefficient is not negative, then the function is
            // not submodular, and all we can do is truncate the function.
            if ( B_right[i][j] >= 0.0f ) 
            {
                //printf( "Truncating\n" );
                continue;
            }
            
            // switch x_{i,j} to \bar{x}_{i,j}
            // Now the term corresponds to an edge from x_{i,j} to x_{i,j+1}
            
            g.add_edge( this->node( i, j ), this->node( i, j + 1 ),
                        -B_right[i][j], 0.0f );
                        
            // and update the unary terms
            g.add_tweights( this->node(i,j+1), B_right[i][j], 0.0f );
        }
    
    // down
    for (int i=0; i<this->idim_-1; ++i)
        for (int j=0; j<this->jdim_; ++j)
        {
            // if the coefficient is not negative, then the function is
            // not submodular, and all we can do is truncate the function.
            if ( B_down[i][j] >= 0.0f )
            {
                //printf( "Truncating\n" );
                continue;
            }           
            
            // switch x_{i,j} to \bar{x}_{i,j}
            // Now the term corresponds to an edge from x_{i,j} to x_{i+1,j}
            
            g.add_edge( this->node( i, j ), this->node( i + 1, j ),
                       -B_down[i][j], 0.0f );
            
            // and update the unary terms
            g.add_tweights( this->node(i+1,j), B_down[i][j], 0.0f );
        }
    


    // solve
    g.maxflow();
    
    // record the answer
    for (int i=0; i<this->idim_; ++i)
        for (int j=0; j<this->jdim_; ++j)
            Seg[i][j] = g.what_segment( this->node(i,j) );
}


