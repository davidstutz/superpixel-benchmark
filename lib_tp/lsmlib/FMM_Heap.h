/*
 * File:        FMM_Heap.h
 * Copyright:   (c) 2005-2006 Kevin T. Chu
 * Revision:    $Revision: 1.14 $
 * Modified:    $Date: 2006/08/13 13:32:08 $
 * Description: Header file for C heap library for supporting FMM algorithm
 */

#ifndef included_FMM_Heap_h
#define included_FMM_Heap_h
 
#ifdef __cplusplus
extern "C" {
#endif

/*! \file FMM_Heap.h
 *
 * \brief
 * @ref FMM_Heap.h provides the heap data structure required to implement 
 * the Fast Marching Method.  
 * 
 * In addition to the operations on a typical heap, FMM_Heap provides some 
 * information on the movement of FMM_HeapNodes in the heap which is required 
 * to maintain "back pointers" from the grid to the nodes in the heap.  For 
 * details see "Level Set Methods and Fast Marching Methods" by J.A. Sethian. 
 *
 * It is designed to minimize the number of updates of "pointers" 
 * from the grid to the FMM_HeapNodes when the tentative values of the
 * distance function are updated.  This goal is achieved by storing 
 * the FMM_HeapNodes separately from the array that represents the heap;
 * an auxilliary array of integer "pointers" to the array of FMM_HeapNodes 
 * actually represents the heap. 
 *
 *
 * <h3> NOTES: </h3>
 * - At this point in time, this library is only works for up
 *   to 8 spatial dimensions.   This source of this 
 *   is the size of the grid_idx data member in 
 *   of the FMM_HeapNode structure.  
 *
 */


/*!
 * The Heap structure stores the internal data required to maintain
 * the state of the heap.
 */
typedef struct FMM_Heap FMM_Heap;


/*! 
 * maximum number of spatial dimensions in grid 
 */
#define FMM_HEAP_MAX_NDIM (8)

/*!
 * The FMM_HeapNode structure stores the index of a grid cell, the value 
 * of the function within that cell, and heap_pos that is used 
 * internally to maintain the heap.
 */
typedef struct HeapNode {
  int grid_idx[FMM_HEAP_MAX_NDIM];  	/* grid index     */
  double value;     			/* function value */
  int heap_pos;     			/* internal data  */
} FMM_HeapNode;


/*!
 * FMM_Heap_createHeap() dynamically allocates an empty heap with the 
 * specified amount of memory allocated for the heap and the specified 
 * growth factor.
 *
 * Arguments:
 *  - num_dims (in):       number of spatial dimensions for FMM calculation
 *  - heap_mem_size (in):  number of nodes to initially allocate memory for
 *  - growth_factor (in):  factor used to grow size of memory allocated for
 *                         heap when the heap exhausts its memory allocation
 *
 * Return value:           pointer to new heap
 *
 * NOTES:
 *  - To use the default amount of memory (64 FMM_HeapNodes), 
 *    set heap_mem_size to 0. 
 *
 *  - To use the default growth factor (2), set growth_factor = 0.
 *
 */
FMM_Heap* FMM_Heap_createHeap(int num_dims, int heap_mem_size, 
  double growth_factor);

/*!
 * FMM_Heap_destroyHeap() frees the memory used to store the heap.
 *
 * Arguments:
 *  - heap (in):  pointer to heap to be destroyed
 *
 * Return value:  none
 *
 */
void FMM_Heap_destroyHeap(FMM_Heap* heap);

/*!
 * FMM_Heap_insertNode() inserts a new node into the heap and returns
 * an integer handle to the node.
 *
 * Arguments:
 *  - heap (in):      pointer to heap 
 *  - grid_idx (in):  grid index of node to insert into heap
 *  - value (in):     value of node to insert into heap
 *
 * Return value:      integer handle to the FMM_HeapNode created for 
 *                    the new node
 *
 * NOTE: the integer handle that is returned by this function
 *       may be changed by an FMM_Heap_extractMin() operation and need 
 *       to be updated accordingly after calling FMM_Heap_extractMin().
 */
int FMM_Heap_insertNode(FMM_Heap* heap, int *grid_idx, double value);

/*!
 * FMM_Heap_extractMin() removes the FMM_HeapNode with the minimum 
 * function value from the heap and returns it as the return value.  
 * Additionally, this method may change the node handle for 
 * one node in the heap.  Information about the node whose
 * handle is changed is optionally returned through the 
 * moved_node and moved_handle arguments.  If no FMM_HeapNode needed 
 * to be moved during the operation, (*moved_handle) is set 
 * to -1 and (*moved_node) is given a very large value 
 * (invalid state).
 *
 * Arguments:
 *  - heap (in):           pointer to heap 
 *  - moved_node (out):    FMM_HeapNode of node in the heap that was moved
 *                         (if a node was actually moved)
 *  - moved_handle (out):  integer handle for FMM_HeapNode that was 
 *                         (if a node was actually moved)
 *
 * Return value:           FMM_HeapNode possessing minimum value
 *
 * NOTES: 
 *  - the user is responsible for providing the memory for
 *    moved_node and moved_handle.
 *
 *  - moved_node and moved_handle may be independently set
 *    to NULL if the moved node information is not needed.
 *
 */
FMM_HeapNode FMM_Heap_extractMin(FMM_Heap* heap, FMM_HeapNode* moved_node, 
  int* moved_handle);

/*!
 * FMM_Heap_updateNode() updates the value of function in the specified 
 * node and moves it up or down the heap so that the heap-property 
 * is preserved. 
 *
 * Arguments:
 *  - heap (in):         pointer to heap 
 *  - node_handle (in):  integer handle of node to update
 *  - value (in):        new value for updated node
 *
 * Return value:         none
 *
 */
void FMM_Heap_updateNode(FMM_Heap* heap, int node_handle, double value);

/*!
 * FMM_Heap_clear() empties out the heap.
 *
 * Arguments:
 *  - heap (in):         pointer to heap 
 *
 * Return value:         none
 *
 */
void FMM_Heap_clear(FMM_Heap* heap);

/*!
 * FMM_Heap_isEmpty() returns true (1) if the heap is empty and 
 * false (0) otherwise.
 *
 * Arguments:
 *  - heap (in):         pointer to heap 
 *
 * Return value:      true (1) if the heap is empty; false (0) otherwise
 *
 */
int FMM_Heap_isEmpty(FMM_Heap* heap);

/*!
 * FMM_Heap_getNode() returns the specified node
 *
 * Arguments:
 *  - heap (in):         pointer to heap 
 *  - node_handle (in):  integer handle of requested node
 *
 * Return value:         requested FMM_HeapNode
 *
 */
FMM_HeapNode FMM_Heap_getNode(FMM_Heap* heap,int node_handle);

/*!
 * FMM_Heap_getHeapSize() returns the current number of nodes in the heap
 *
 * Arguments:
 *  - heap (in):         pointer to heap 
 *
 * Return value:         current number of nodes in heap
 *
 */
int FMM_Heap_getHeapSize(FMM_Heap* heap);

/*!
 * FMM_Heap_getHeapMemSize() returns the current maximum number of nodes
 * that the heap can accomodate.
 *
 * Arguments:
 *  - heap (in):         pointer to heap 
 *
 * Return value:         maximum number of nodes that the heap can currently
 *                       accomodate before requiring memory reallocation
 *
 */
int FMM_Heap_getHeapMemSize(FMM_Heap* heap) ;

/*!
 * FMM_Heap_prinHeapData() prints all data members for the specified 
 * FMM_Heap structure.
 *
 * Arguments:
 *  - heap (in):         pointer to heap 
 *
 * Return value:         none
 *
 */
void FMM_Heap_printHeapData(FMM_Heap* heap); 

#ifdef __cplusplus
}
#endif

#endif
