/*
 * File:        FMM_Heap.c
 * Copyright:   (c) 2005-2006 Kevin T. Chu
 * Revision:    $Revision: 1.15 $
 * Modified:    $Date: 2006/11/02 02:32:33 $
 * Description: C heap library for supporting fast marching method 
 */

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include "FMM_Heap.h" 

/*
 * FMM_Heap Constants
 */
#define DEFAULT_HEAP_MEM_SIZE (64)
#define DEFAULT_HEAP_GROWTH_FACTOR (2)

/*
 * FMM_Heap Macros
 */

#define HEAP_POS(i) 	( d_nodes[(i)].heap_pos )
#define PARENT_N(i) 	\
	( d_heap[ (int)( (d_nodes[(i)].heap_pos+1)/2 -1 )] )
#define CHILD_LEFT_N(i) 	\
	( d_heap[ (int)(2*(d_nodes[(i)].heap_pos+1) -1 )] )
#define CHILD_RIGHT_N(i) 	\
	( d_heap[ (int)( 2*(d_nodes[(i)].heap_pos+1) )] )
#define PARENT_H(i) 		( (int)( ((i)+1)/2 -1 ) )
#define CHILD_LEFT_H(i) 	( (int)( 2*((i)+1) -1 ) )
#define CHILD_RIGHT_H(i) 	( (int)( 2*((i)+1) ) )


/*
 * Definition of FMM_Heap structure.
 */
struct FMM_Heap {
  int* d_heap;
  FMM_HeapNode* d_nodes;
  int d_num_dims;
  int d_heap_size;
  int d_heap_mem_size;
  double d_heap_growth_factor;
};


/*================== Helper Functions Declarations ==================*/

/*
 * FMM_Heap_makeNewHeap() allocates memory for and initializes all
 * nodes in the heap to have a big value (DBL_MAX).
 * The amount of memory allocated is dynamically adjusted
 * to accomodate the number of nodes in the heap.
 */
void FMM_Heap_makeNewHeap(FMM_Heap* heap, int heap_mem_size);

/*
 * FMM_Heap_growHeap() increases the amount of the memory allocated for
 * the heap by the heap growth factor.
 *
 * NOTE:  this is an EXPENSIVE operation because all FMM_HeapNodes
 *        must be copied to the new array AND because all of the
 *        "back-pointers" from the grid must be updated to reflect
 *        the new locations of the FMM_HeapNodes.
 */
void FMM_Heap_growHeap(FMM_Heap* heap);

/*
 * FMM_Heap_upHeap() bubbles the specified position up the heap until
 * the value of the corresponding node is greater than its parent.
 */
void FMM_Heap_upHeap(FMM_Heap* heap, int heap_pos);

/*
 * FMM_Heap_downHeap() bubbles the specified position down the heap
 * until the value of the corresponding node is smaller than its parent.
 */
void FMM_Heap_downHeap(FMM_Heap* heap, int heap_pos);

/*===================================================================*/


/*==================== Function Definitions =========================*/

FMM_Heap* FMM_Heap_createHeap(int num_dims, int heap_mem_size, 
  double growth_factor)
{
  FMM_Heap* heap;

  /* Check inputs */
  if (heap_mem_size <= 0) heap_mem_size = DEFAULT_HEAP_MEM_SIZE;
  if (growth_factor < 1) growth_factor = DEFAULT_HEAP_GROWTH_FACTOR; 

  heap = (FMM_Heap*) malloc(sizeof(FMM_Heap));
  heap->d_num_dims = num_dims;
  heap->d_heap_size = 0;
  heap->d_heap_mem_size = heap_mem_size;
  heap->d_heap_growth_factor = growth_factor;

  FMM_Heap_makeNewHeap(heap, heap_mem_size);

  return heap;
}

void FMM_Heap_destroyHeap(FMM_Heap* heap)
{
  free(heap->d_heap);
  free(heap->d_nodes);
  free(heap);
}

int FMM_Heap_insertNode(FMM_Heap* heap, int *grid_idx, double value)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int d_heap_size = heap->d_heap_size;
  int i;

  /* insert node at bottom heap */
  d_heap[d_heap_size] = d_heap_size;
  for (i = 0; i< heap->d_num_dims; i++) {
    d_nodes[d_heap_size].grid_idx[i] = grid_idx[i];
  }
  d_nodes[d_heap_size].value = value;
  d_nodes[d_heap_size].heap_pos = d_heap_size;

  /* bubble it up the heap until the heap property is satisfied */
  FMM_Heap_upHeap(heap, d_heap_size);

  /* update heap size information and grow heap memory if necessary */
  heap->d_heap_size++;
  if (heap->d_heap_size == heap->d_heap_mem_size) FMM_Heap_growHeap(heap);

  return (heap->d_heap_size-1);
}

FMM_HeapNode FMM_Heap_extractMin(FMM_Heap* heap, FMM_HeapNode* moved_node, 
  int* moved_handle) 
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int d_heap_size = heap->d_heap_size;

  int root_handle = d_heap[0]; /* handle for root of heap */
  FMM_HeapNode moved_node_local;
  int moved_handle_local;
  FMM_HeapNode min_node = d_nodes[root_handle]; /* copy root of heap */

  /* 
   * if min_node was not located at the end of d_nodes, 
   * fill the gap in d_heap left by moving the FMM_HeapNode
   * at the end of d_nodes to the position occupied
   * by min_node and the FMM_HeapNode at the end of d_nodes
   * into an invalid state
   */
  if (root_handle != d_heap_size-1) {

    /* set moved node and handle */
    moved_node_local = d_nodes[d_heap_size-1]; 
    moved_handle_local = root_handle;  

    /* replace min_node wth moved_node and fix pointer from d_heap */
    d_nodes[root_handle] = moved_node_local; 
    d_heap[moved_node_local.heap_pos] = root_handle; 

    /* invalidate copy of moved_node */
    d_nodes[d_heap_size-1].value = DBL_MAX;

  } else {

    /* set position occupied by root node to invalid state */
    d_nodes[root_handle].value = DBL_MAX;
    
    /* set moved_node and moved_handle to invalid state */
    moved_node_local.value = DBL_MAX;
    moved_node_local.heap_pos = -1;
    moved_handle_local = -1;

  }

  /* 
   * move the last FMM_HeapNode in the heap (d_heap NOT d_nodes) 
   * to the root position and trickle it down until the heap 
   * property is satisfied using the FMM_Heap_downHeap() method.
   */
  HEAP_POS(d_heap[d_heap_size-1]) = 0;
  d_heap[0] = d_heap[d_heap_size-1];
  FMM_Heap_downHeap(heap, 0);

  /* 
   * copy the moved node data into moved_node and moved_handle
   *if they are not NULL
   */
  if (moved_handle) (*moved_handle) = moved_handle_local;
  if (moved_node) {
    (*moved_node) = moved_node_local;

    /* 
     * update moved_node's heap_pos field in case it changed
     * during the FMM_Heap_downHeap() operation
     */ 
    if (0 < moved_handle_local)
      moved_node->heap_pos = HEAP_POS(moved_handle_local);
  }

  /* remove the last node from the heap */
  d_heap[d_heap_size-1] = -1;
  heap->d_heap_size--;

  return min_node;
}

void FMM_Heap_updateNode(FMM_Heap* heap, int node_handle, double value)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;

  d_nodes[node_handle].value = value;  /* update value of node */

  /* bubble the node up/down the heap to reinstate heap property */
  if (    (HEAP_POS(node_handle) > 0) /* make sure there is parent to check */
       && (value < d_nodes[PARENT_N(node_handle)].value) ) {
    FMM_Heap_upHeap(heap, HEAP_POS(node_handle)); 
  } else {
    FMM_Heap_downHeap(heap, HEAP_POS(node_handle));
  }
}

void FMM_Heap_clear(FMM_Heap* heap)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int i;

  /* reset heap size to zero */
  heap->d_heap_size = 0;

  /* 
   * initialize the value of all nodes to DBL_MAX and all 
   * heap pointers to -1 
   */
  for (i = 0; i < heap->d_heap_mem_size; i++) {
    d_heap[i] = -1;
    d_nodes[i].value = DBL_MAX;
  }
}


/**
 * FMM_Heap_isEmpty() returns true if the heap is empty and false otherwise.
 */
int FMM_Heap_isEmpty(FMM_Heap* heap)
{
  if (0 == heap->d_heap_size) return 1;
  else return 0;
}
 
/**
 * FMM_Heap_getNode() returns the specified node
 */
FMM_HeapNode FMM_Heap_getNode(FMM_Heap* heap,int node_handle) 
{
  return heap->d_nodes[node_handle];
}

int FMM_Heap_getHeapSize(FMM_Heap* heap) 
{
  return heap->d_heap_size;
}

int FMM_Heap_getHeapMemSize(FMM_Heap* heap) 
{
  return heap->d_heap_mem_size;
}
 
void FMM_Heap_printHeapData(FMM_Heap* heap)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int d_heap_size = heap->d_heap_size;

  printf("\nprintHeapData...\n");
  printf("FMM_Heap: this = %ld\n", (long int) heap);
  printf("d_heap = %ld\n", (long int) d_heap);
  printf("d_nodes = %ld\n", (long int) d_nodes); 
  printf("d_heap_size = %d\n", d_heap_size); 
  printf("d_heap_mem_size = %d\n", heap->d_heap_mem_size); 
  printf("d_heap_growth_factor = %f\n\n", heap->d_heap_growth_factor); 
}

/*================== Helper Functions Definitions ===================*/

void FMM_Heap_makeNewHeap(FMM_Heap* heap, int heap_mem_size)
{
  int i;

  /* allocate memory for heap */
  heap->d_heap = (int*) malloc(heap_mem_size*sizeof(int));
  heap->d_nodes = (FMM_HeapNode*) malloc(heap_mem_size*sizeof(FMM_HeapNode));

  /* initialize the value of all nodes to DBL_MAX and all heap pointers to -1 */
  for (i = 0; i < heap_mem_size; i++) {
    heap->d_heap[i] = -1;
    heap->d_nodes[i].value = DBL_MAX;
  }

}

void FMM_Heap_growHeap(FMM_Heap* heap)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int d_heap_size = heap->d_heap_size;
  double d_heap_growth_factor = heap->d_heap_growth_factor;
  int i;
  int *old_heap; 
  FMM_HeapNode *old_nodes; 

  /* compute new heap memory size */
  heap->d_heap_mem_size = 
     (int) (heap->d_heap_mem_size*d_heap_growth_factor+1);  
  
  /* save pointer to old heap and allocate memory for new heap */
  old_heap = d_heap;
  old_nodes = d_nodes;
  FMM_Heap_makeNewHeap(heap, heap->d_heap_mem_size);

  /* copy nodes from old heap to new heap */
  d_heap_size = heap->d_heap_size;
  d_heap = heap->d_heap;
  d_nodes = heap->d_nodes;
  for (i=0;i<d_heap_size;i++) {
    d_heap[i] = old_heap[i];
    d_nodes[i] = old_nodes[i];
  }

  /* reclaim memory for old_heap */
  free(old_heap); 
  free(old_nodes); 
}

void FMM_Heap_upHeap(FMM_Heap* heap, int heap_pos)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int parent_pos;
  int tmp;

  parent_pos = PARENT_H(heap_pos);
  while ( (heap_pos > 0) &&
          (d_nodes[d_heap[heap_pos]].value < 
             d_nodes[d_heap[parent_pos]].value) )
  {
    /* swap heap positions in d_nodes */
    HEAP_POS(d_heap[heap_pos]) = parent_pos;
    HEAP_POS(d_heap[parent_pos]) = heap_pos;

    /* swap node handle with parent node handle in d_heap */
    tmp = d_heap[heap_pos];
    d_heap[heap_pos] = d_heap[parent_pos];
    d_heap[parent_pos] = tmp;

    /* update heap_pos and parent heap_pos */
    heap_pos = parent_pos;
    parent_pos = PARENT_H(heap_pos);
  } 

}

void FMM_Heap_downHeap(FMM_Heap* heap, int heap_pos)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int d_heap_size = heap->d_heap_size;
  int left_pos; 
  int right_pos;
  double cur_value;
  double left_value; 
  double right_value;
  int tmp;

  int done = 0;
  while ( !done && (d_heap_size > CHILD_LEFT_H(heap_pos)) ) {

    left_pos = CHILD_LEFT_H(heap_pos);
    right_pos = CHILD_RIGHT_H(heap_pos);
    cur_value = d_nodes[d_heap[heap_pos]].value;
    left_value = d_nodes[d_heap[left_pos]].value;
    right_value = DBL_MAX;

    if (right_pos < d_heap_size) {
      right_value = d_nodes[d_heap[right_pos]].value;
    }

    if ( (cur_value <= left_value) && (cur_value <= right_value) ) {
      /* heap_pos is min, so we're done */ 
      done = 1;
    } else if ( (left_value < cur_value) && (left_value <= right_value) ){
      /* left child is min */

      /* swap heap positions in d_nodes */
      HEAP_POS(d_heap[heap_pos]) = left_pos;
      HEAP_POS(d_heap[left_pos]) = heap_pos;

      /* swap node handle with left child node handle in d_heap */
      tmp = d_heap[heap_pos];
      d_heap[heap_pos] = d_heap[left_pos];
      d_heap[left_pos] = tmp;

      /* set heap_pos to the left child of heap_pos */
      heap_pos = left_pos;

    } else if (right_pos < d_heap_size) {
      /* right child is min */

      /* swap heap positions in d_nodes */
      HEAP_POS(d_heap[heap_pos]) = right_pos;
      HEAP_POS(d_heap[right_pos]) = heap_pos;

      /* swap node handle with right child node handle in d_heap */
      tmp = d_heap[heap_pos];
      d_heap[heap_pos] = d_heap[right_pos];
      d_heap[right_pos] = tmp;

      /* set heap_pos to the right child of heap_pos */
      heap_pos = right_pos;

    } 
  }

}




/* ****** DEBUGGING ******** */
void FMM_Heap_checkHeap(FMM_Heap* heap)
{
  int *d_heap = heap->d_heap;
  FMM_HeapNode* d_nodes = heap->d_nodes;
  int d_heap_size = heap->d_heap_size;
  int i;

  /* check heap property */
  for (i = 0; 2*(i+1)-1 < d_heap_size; i++) {
    double parent = d_nodes[d_heap[i]].value;
    if (2*(i+1)-1 < d_heap_size) {
      double child_left = d_nodes[d_heap[CHILD_LEFT_H(i)]].value;
      if (parent > child_left) 
        printf("ERROR: Heap property failed - left child!!!: %d\n",i);
    }
    if (2*(i+1) < d_heap_size) {
      double child_right = d_nodes[d_heap[CHILD_RIGHT_H(i)]].value;
      if (parent > child_right) 
        printf("ERROR: Heap property failed - right child!!!: %d\n", i);
    }  
  }  

  /* check that heap_pos "back pointers" are correctly set */
  for (i = 0; i < d_heap_size; i++) {
    if (d_nodes[d_heap[i]].heap_pos != i)
      printf("ERROR: invalid back pointer:  %d\n", i);
  }  
}
