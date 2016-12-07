/*
 * File:        FMM_Core.c
 * Copyright:   (c) 2005-2006 Kevin T. Chu
 * Revision:    $Revision: 1.17 $
 * Modified:    $Date: 2006/08/13 13:32:08 $
 * Description: Implementation of FMM_Core functions
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "FMM_Heap.h"
#include "FMM_Core.h"


/*======================= FMM_Core Macros =========================*/
#define FMM_CORE_ABS(x)       ((x) > 0 ? (x) : -1.0*(x))

#define FMM_CORE_IDX(idx, num_dims, grid_idx, grid_dims)           \
{                                                                  \
  int macro_i;                         /* loop variable */         \
  int macro_num_gridpts_per_grid_idx = 1;                          \
  idx = 0;                                                         \
  for (macro_i = 0; macro_i < num_dims; macro_i++) {               \
    idx += macro_num_gridpts_per_grid_idx*grid_idx[macro_i];       \
    macro_num_gridpts_per_grid_idx *= grid_dims[macro_i];          \
  }                                                                \
}

#define FMM_CORE_IDX_OUT_OF_BOUNDS(out_of_bounds, num_dims, grid_idx,     \
                                   grid_dims)                             \
{                                                                         \
  int macro_i;       /* loop variable */                                  \
  out_of_bounds = 0;                                                      \
  for (macro_i = 0; macro_i < num_dims; macro_i++) {                      \
    if (  (grid_idx[macro_i] < 0)                                         \
       || (grid_idx[macro_i] > grid_dims[macro_i]-1) ) {                  \
      out_of_bounds = 1;                                                  \
      break;                                                              \
    }                                                                     \
  }                                                                       \
}


/*=============== FMM_Core Helper Function Declarations ==============*/

/* 
 * FMM_Core_updateNeighbors() updates the neighbors of the specified
 * grid point.
 */
double FMM_Core_updateNeighbors(FMM_CoreData *fmm_core_data, int *grid_idx); 


/*=============== Fast Marching Method Data Structures ==============*/
struct FMM_CoreData {

  /* field data */
  int num_dims;
  FMM_FieldData *fmm_field_data;
  int grid_dims[FMM_CORE_MAX_NDIM];
  double dx[FMM_CORE_MAX_NDIM];

  /* function pointer to grid update function */
  initializeFrontFuncPtr initializeFront;
  updateGridPointFuncPtr updateGridPoint;

  /* internal data */
  int* heapnode_handles;
  int* gridpoint_status;
  FMM_Heap* trial_points;
  FMM_Heap* known_points;

};


/*=============== FMM_Core API Function Definitions ==============*/

FMM_CoreData* FMM_Core_createFMM_CoreData(
  FMM_FieldData *fmm_field_data,
  int num_dims,
  int *grid_dims,
  double *dx,
  initializeFrontFuncPtr initializeFront,
  updateGridPointFuncPtr updateGridPoint)
{
  FMM_CoreData *fmm_core_data;     /* pointer to new FMM_CoreData */
  int num_gridpoints;              /* number of grid points */
  int initial_heap_size;           /* initial size for FMM_Heap */
  int i;                           /* loop variable */
  int *ptr;                        /* integer pointer loop variable */

  /* check that num_dimension is supported */
  if ( (num_dims < 2) || (num_dims > FMM_CORE_MAX_NDIM) ) {
    fprintf(stderr,
    "ERROR: Invalid number of dimensions.  Only NDIM < %d supported.\n",
    FMM_CORE_MAX_NDIM);
  } 

  /* allocate memory for FMM_CoreData */
  fmm_core_data = (FMM_CoreData*) malloc( sizeof(FMM_CoreData) );

  /* compute number of grid points */
  num_gridpoints = 1;
  for (i = 0; i < num_dims; i++) num_gridpoints *= grid_dims[i];

  /* initialize FMM data */
  fmm_core_data->heapnode_handles = (int*) malloc(num_gridpoints*sizeof(int));
  fmm_core_data->gridpoint_status = (int*) malloc(num_gridpoints*sizeof(int));
  fmm_core_data->num_dims = num_dims;
  fmm_core_data->fmm_field_data = fmm_field_data;
  fmm_core_data->initializeFront = initializeFront;
  fmm_core_data->updateGridPoint = updateGridPoint;

  /* initialize grid_dims and dx to zero */
  for (i = 0; i < FMM_CORE_MAX_NDIM; i++) {
    fmm_core_data->grid_dims[i] = 0;
    fmm_core_data->dx[i] = 0.0;
  }

  /* copy of grid_dims and dx from user specified arguments */
  /* NOTE:  data is only copied for the number of           */
  /*        dimensions specified for the computation.       */
  for (i = 0; i < num_dims; i++) {
    fmm_core_data->grid_dims[i] = grid_dims[i];
    fmm_core_data->dx[i] = dx[i];
  }

  /* create an FMM_Heap to store the trial points */
  /* NOTE: using default heap growth factor by    */
  /*       specifying 0 for the second argument   */
  initial_heap_size = 0;
  for (i = 0; i < num_dims; i++) initial_heap_size += grid_dims[i];
  fmm_core_data->trial_points = 
    FMM_Heap_createHeap(num_dims,initial_heap_size,0); 

  /* initialize heapnode handles to have a default value of -1 */
  ptr = fmm_core_data->heapnode_handles;
  for (i = 0; i < num_gridpoints; i++, ptr++) {
    *ptr = -1;
  }

  /* initialize gridpoint status of all cells to FAR */
  ptr = fmm_core_data->gridpoint_status;
  for (i = 0; i < num_gridpoints; i++, ptr++) {
    *ptr = FAR;
  }

  return fmm_core_data;
}


void FMM_Core_destroyFMM_CoreData(FMM_CoreData *fmm_core_data)
{
  free(fmm_core_data->heapnode_handles);
  free(fmm_core_data->gridpoint_status);
  FMM_Heap_destroyHeap(fmm_core_data->trial_points);
  if (fmm_core_data->known_points != FMM_CORE_NULL)
    FMM_Heap_destroyHeap(fmm_core_data->known_points);
  free(fmm_core_data);
}


void FMM_Core_initializeFront(FMM_CoreData *fmm_core_data)
{
  int num_dims = fmm_core_data->num_dims; 
  int *grid_dims = fmm_core_data->grid_dims;
  FMM_FieldData *fmm_field_data = fmm_core_data->fmm_field_data;
  int num_gridpoints;

  /* list of known points */
  FMM_Heap *known_points; 
  int initial_heap_size;
  int grid_idx[FMM_CORE_MAX_NDIM];

  /* auxilliary variables */
  int i;         /* loop variable */

  /* compute the number of grid points and initial heap size */
  num_gridpoints = 1;
  initial_heap_size = 0;
  for (i = 0; i < num_dims; i++) {
    num_gridpoints *= grid_dims[i];
    initial_heap_size += grid_dims[i];
  }

  /* create FMM_Heap to contain known points      */
  /* NOTE: using default heap growth factor by    */
  /*       specifying 0 for the second argument   */
  known_points = FMM_Heap_createHeap(num_dims,initial_heap_size,0);
  fmm_core_data->known_points = known_points;

  /* let user-provided callback function find and initialize the front */
  fmm_core_data->initializeFront(
    fmm_core_data, 
    fmm_field_data, 
    fmm_core_data->num_dims, 
    fmm_core_data->grid_dims, 
    fmm_core_data->dx);


  /* 
   * Set initial set of trial points (i.e. all of the 
   * neighbors of the initial set of known points).
   * For all "known" points: 
   *   (1) update their neighbors 
   *   (2) add their neighbors to the list of trial points
   */
  
  while (!FMM_Heap_isEmpty(known_points)) {
    /* extract grid index of next known point */
    FMM_HeapNode node = FMM_Heap_extractMin(known_points,
                                            FMM_CORE_NULL,
                                            FMM_CORE_NULL);

    
    /* update neighbors if the value of the node is */
    /* less than DBL_MAX                            */
    if (node.value < DBL_MAX) {

      /* set grid_idx */
      for (i = 0; i < num_dims; i++) {
        grid_idx[i] = node.grid_idx[i];
      }
      for (i = num_dims; i < FMM_CORE_MAX_NDIM; i++) {
        grid_idx[i] = 0;
      }

      FMM_Core_updateNeighbors(fmm_core_data, grid_idx);
    }

  } /* end loop over "known" points */

  /* clean up memory */
  FMM_Heap_destroyHeap(known_points);
  fmm_core_data->known_points = FMM_CORE_NULL;
};


/*
 * FMM_Core_setInitialFrontPoint() first makes a local copy of the grid_idx
 * because the FMM_CORE_IDX calculation and FMM_Heap_insertNode() function
 * require that grid_idx is an array of size FMM_CORE_MAX_NDIM 
 * (= FMM_HEAP_MAX_NDIM).
 */
void FMM_Core_setInitialFrontPoint(
  FMM_CoreData *fmm_core_data, 
  int *grid_idx, 
  double value)
{
  int num_dims = fmm_core_data->num_dims; 
  int *grid_dims = fmm_core_data->grid_dims;
  int *gridpoint_status = fmm_core_data->gridpoint_status; 
  int grid_idx_local[FMM_CORE_MAX_NDIM];     /* local copy of grid_idx */

  /* auxilliary variables */
  int i;    /* loop variable */
  int idx;  /* data array index */

  /* make local copy of grid index */
  for (i = 0; i < num_dims; i++) {
    grid_idx_local[i] = grid_idx[i];
  }
  for (i = num_dims; i < FMM_CORE_MAX_NDIM; i++) {
    grid_idx_local[i] = 0;
  }

  /* set grid point status to KNOWN */
  FMM_CORE_IDX(idx, num_dims, grid_idx_local, grid_dims);
  gridpoint_status[idx] = KNOWN;

  /* insert grid point into known_points heap */
  FMM_Heap_insertNode(fmm_core_data->known_points,grid_idx_local,value);
}

/*
 * FMM_Core_markPointOutsideDomain() first makes a local copy of the grid_idx
 * because the FMM_CORE_IDX calculation and FMM_Heap_insertNode() function
 * require that grid_idx is an array of size FMM_CORE_MAX_NDIM 
 * (= FMM_HEAP_MAX_NDIM).
 */
void FMM_Core_markPointOutsideDomain(
  FMM_CoreData *fmm_core_data, 
  int *grid_idx)
{
  int num_dims = fmm_core_data->num_dims; 
  int *grid_dims = fmm_core_data->grid_dims;
  int *gridpoint_status = fmm_core_data->gridpoint_status; 
  int grid_idx_local[FMM_CORE_MAX_NDIM];     /* local copy of grid_idx */

  /* auxilliary variables */
  int i;    /* loop variable */
  int idx;  /* data array index */

  /* make local copy of grid index */
  for (i = 0; i < num_dims; i++) {
    grid_idx_local[i] = grid_idx[i];
  }
  for (i = num_dims; i < FMM_CORE_MAX_NDIM; i++) {
    grid_idx_local[i] = 0;
  }

  /* set grid point status to OUTSIDE_DOMAIN */
  FMM_CORE_IDX(idx, num_dims, grid_idx_local, grid_dims);
  gridpoint_status[idx] = OUTSIDE_DOMAIN;

}

/* 
 * NOTES:
 *  (1) There may be some error in the update of cells on the border 
 *      of the grid, but this error is not important as long as the 
 *      zero level set is sufficiently far away from the domain border.
 */
double FMM_Core_advanceFront(FMM_CoreData *fmm_core_data)
{
  int num_dims = fmm_core_data->num_dims;
  int* grid_dims = fmm_core_data->grid_dims;
  FMM_Heap *fmm_trial_points = fmm_core_data->trial_points;
  int *heapnode_handles = fmm_core_data->heapnode_handles;
  int *gridpoint_status = fmm_core_data->gridpoint_status;
  FMM_HeapNode moved_node;
  int moved_handle;
  FMM_HeapNode min_node;
  int idx;

  /* 
   * remove the point with the smallest value from the set of "trial" points.
   */
  min_node = FMM_Heap_extractMin(fmm_trial_points, &moved_node, &moved_handle);

  /* correct the handle for the moved node */
  if (-1 != moved_handle) {  /* update heapnode_data if necessary */
    FMM_CORE_IDX(idx, num_dims, moved_node.grid_idx, grid_dims);
    heapnode_handles[idx] = moved_handle;
  }

  /* set status of min node to "known" */
  FMM_CORE_IDX(idx, num_dims, min_node.grid_idx, grid_dims);
  gridpoint_status[idx] = KNOWN;

  /* update neighbors */
  return FMM_Core_updateNeighbors(fmm_core_data, min_node.grid_idx);

}

int FMM_Core_moreGridPointsToUpdate(FMM_CoreData *fmm_core_data)
{
  return ( FMM_Heap_isEmpty(fmm_core_data->trial_points) ?
           FMM_CORE_FALSE : FMM_CORE_TRUE);
}

int* FMM_Core_getGridPointStatusDataArray(FMM_CoreData *fmm_core_data)
{
  return (fmm_core_data->gridpoint_status);
}


/*=============== FMM_Core Helper Function Definitions ==============*/

double FMM_Core_updateNeighbors(FMM_CoreData *fmm_core_data, int *grid_idx)
{
  int* grid_dims = fmm_core_data->grid_dims;
  FMM_Heap *fmm_trial_points = fmm_core_data->trial_points;
  FMM_FieldData *fmm_field_data = fmm_core_data->fmm_field_data;
  int *heapnode_handles = fmm_core_data->heapnode_handles;
  int *gridpoint_status = fmm_core_data->gridpoint_status;
  int num_dims = fmm_core_data->num_dims;

  /* variables for update calculation */
  int neighbor[FMM_CORE_MAX_NDIM];
  int offset[FMM_CORE_MAX_NDIM];
  double value;
  int heapnode_handle;

  /* auxilliary variables */
  int dir; 	       /* loop variable for spatial directions */
  int n;	       /* loop variable for neighbors */
  int m;	       /* extra loop variable */
  int idx;             /* data array index */
  int out_of_bounds;   /* boolean indicating if index is out of bounds */

  /* loop over coordinate directions */
  for (dir = 0; dir < num_dims; dir++) { 

    /* reset neighbor and offset */
    for (m = 0; m < FMM_CORE_MAX_NDIM; m++) {
      neighbor[m] = 0; offset[m] = 0; 
    }

    for (n = -1; n<=1; n+=2) { /* loop over neighbors */
      PointStatus neighbor_status;

      offset[dir] = n;

      for (m = 0; m < num_dims; m++) neighbor[m] = grid_idx[m]+offset[m];

      FMM_CORE_IDX_OUT_OF_BOUNDS(out_of_bounds, num_dims, neighbor, grid_dims);
      if (!out_of_bounds) {

        FMM_CORE_IDX(idx, num_dims, neighbor, grid_dims);
        neighbor_status = (PointStatus) gridpoint_status[idx];
        if (  (KNOWN != neighbor_status) 
           && (OUTSIDE_DOMAIN != neighbor_status) ) {

          /* compute trial values for neighbor */
          value = fmm_core_data->updateGridPoint(fmm_core_data, 
                                                 fmm_field_data,
                                                 neighbor,
                                                 fmm_core_data->num_dims, 
                                                 fmm_core_data->grid_dims, 
                                                 fmm_core_data->dx);
          if (value < 0) value *= -1; /* only absolute value matters here */

          if (FAR == neighbor_status) {

            /* set the status of the neighbor to TRIAL */
            FMM_CORE_IDX(idx, num_dims, neighbor, grid_dims);
            gridpoint_status[idx] = TRIAL;

            /* insert the new TRIAL point into the FMM_Heap */
            heapnode_handle = FMM_Heap_insertNode(fmm_trial_points, 
                                                  neighbor, value);

            /* set the heap node handle */
            heapnode_handles[idx] = heapnode_handle;

          } else { 
            /* 
             * neighbor has status TRIAL, so just update its value in 
             * the heap
             */
            FMM_CORE_IDX(idx, num_dims, neighbor, grid_dims);
            FMM_Heap_updateNode(fmm_trial_points, heapnode_handles[idx], 
                                value);
          } 
        } /* end update of neighbor point (not in "known" set) */

      } /* end case: grid index of neighbor is not out of bounds */

    } /* end loop over neighbors */
  } /* end loop over coordinate directions */

  return value;
}

void FMM_Core_initializeHomotopicThinning(FMM_CoreData *fmm_core_data,FMM_FieldData* &fmm_field_data,
                                          int* &grid_dims, FMM_Heap* &known_points) {
  
  int num_dims = fmm_core_data->num_dims; 
  grid_dims = fmm_core_data->grid_dims;
  int num_gridpoints;

  int initial_heap_size;

  /* compute the number of grid points and initial heap size */
  num_gridpoints = 1;
  initial_heap_size = 0;
  for (int i = 0; i < num_dims; i++) {
    num_gridpoints *= grid_dims[i];
    initial_heap_size += grid_dims[i];
  }

  /* create FMM_Heap to contain known points      */
  /* NOTE: using default heap growth factor by    */
  /*       specifying 0 for the second argument   */
  known_points = FMM_Heap_createHeap(num_dims,initial_heap_size,0);
  fmm_core_data->known_points = known_points;
  fmm_field_data = fmm_core_data->fmm_field_data;
  
}

void FMM_Core_setNextThinnedPoint(
    FMM_CoreData *fmm_core_data, 
    int *grid_idx, 
    double value)
{
  int num_dims = fmm_core_data->num_dims; 
  int *grid_dims = fmm_core_data->grid_dims;
  int *gridpoint_status = fmm_core_data->gridpoint_status; 
  int grid_idx_local[FMM_CORE_MAX_NDIM];     /* local copy of grid_idx */

  /* auxilliary variables */
  int i;    /* loop variable */
  int idx;  /* data array index */

  /* make local copy of grid index */
  for (i = 0; i < num_dims; i++) {
    grid_idx_local[i] = grid_idx[i];
  }
  for (i = num_dims; i < FMM_CORE_MAX_NDIM; i++) {
    grid_idx_local[i] = 0;
  }

  /* set grid point status to KNOWN */
  FMM_CORE_IDX(idx, num_dims, grid_idx_local, grid_dims);
  gridpoint_status[idx] = TRIAL;

  /* insert grid point into known_points heap */
  FMM_Heap_insertNode(fmm_core_data->known_points,grid_idx_local,value);
}
