/*
 * File:        lsm_FMM_2d.c
 * Copyright:   (c) 2005-2006 Kevin T. Chu
 * Revision:    $Revision: 1.2 $
 * Modified:    $Date: 2006/08/13 13:30:52 $
 * Description: Implementation of 2D Fast Marching Method 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <deque>
#include "mex.h"
#include "FMM_Core.h"
#include "FMM_Heap.h"
#include "lsm_fast_marching_method.h"


/*===================== lsm_FMM_2d Data Structures ====================*/
struct FMM_FieldData {
  double *phi;                 /* original level set function (input) */
  double *distance_function;   /* distance function (output)          */

  int num_extension_fields;    /* number of extension fields          */
  double **source_fields;      /* source fields to extend off of zero */
                               /* level set (input)                   */
  double **extension_fields;   /* computed extension field (output)   */
};


/*======================= lsm_FMM_2d Constants ==========================*/
#define LSM_FMM_2D_NDIM                   (2)
#define LSM_FMM_2D_TRUE                   (1)
#define LSM_FMM_2D_FALSE                  (0)
#define LSM_FMM_2D_DEFAULT_UPDATE_VALUE   (0)


/*========================== Error Codes ============================*/
#define LSM_FMM_2D_ERR_SUCCESS                             (0)
#define LSM_FMM_2D_ERR_FMM_DATA_CREATION_ERROR             (1)
#define LSM_FMM_2D_ERR_INVALID_SPATIAL_DERIVATIVE_ORDER    (2)


/*========================= lsm_FMM_2d Macros ===========================*/
#define LSM_FMM_2D_IDX(i,j)     ((i) + grid_dims[0]*(j))
#define LSM_FMM_2D_ABS(x)       ((x) > 0 ? (x) : -1.0*(x))
#define LSM_FMM_2D_IDX_OUT_OF_BOUNDS(i,j,grid_dims)            \
  ( ((i)<0) || ((i)>(grid_dims)[0]-1) || ((j)<0) || ((j)>(grid_dims)[1]-1) ) 


/*================== Helper Functions Declarations ==================*/

struct FMM_ListNode {
  double value;
  int grid_idx[LSM_FMM_2D_NDIM];
};

/*
 * FMM_initializeFront_FieldExtension2d_Order1() implements the 
 * callback function required by FMM_Core::FMM_initializeFront() to find 
 * and initialize the front.  
 *
 * The approximation to the distance function is computed
 * using a first-order, O(h), interpolation scheme.  The
 * extension fields are calculated using a first-order
 * approximation to the grad(F)*grad(dist) = 0 equation.
 */
void FMM_initializeFront_FieldExtension2d_Order1(
  FMM_CoreData *fmm_core_data,
  FMM_FieldData *fmm_field_data,
  int num_dims,
  int *grid_dims,
  double *dx);

/* 
 * FMM_updateGridPoint_FieldExtension2d_Order1() computes and returns 
 * the updated distance of the specified grid point using values of 
 * neighbors that have status "KNOWN". 
 *
 * The approximation to the distance function is computed
 * using a first-order, O(h), finite-difference scheme.  The
 * extension fields are calculated using a first-order
 * approximation to the grad(F)*grad(dist) = 0 equation.
 */
double FMM_updateGridPoint_FieldExtension2d_Order1(
  FMM_CoreData *fmm_core_data,
  FMM_FieldData *fmm_field_data,
  int *grid_idx,
  int num_dims,
  int *grid_dims,
  double *dx);

/*==================== Function Definitions =========================*/

int computeExtensionFields2d(
  double *distance_function,
  double **extension_fields,
  double *phi,
  double *mask,
  double **source_fields,
  int num_extension_fields,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx)
{
  return computeExtensionFields2d_WithMaxVal(
  		distance_function,
  		extension_fields,
  		phi,
  		mask,
  		source_fields,
  		num_extension_fields,
  		spatial_derivative_order,
  		grid_dims,
  		dx,
                -1);
}

int computeExtensionFields2d_WithMaxVal(
  double *distance_function,
  double **extension_fields,
  double *phi,
  double *mask,
  double **source_fields,
  int num_extension_fields,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx,
  double max_value)
{
  /* fast marching method data */
  FMM_CoreData *fmm_core_data;

  /* pointers to callback functions */
  updateGridPointFuncPtr updateGridPoint;
  initializeFrontFuncPtr initializeFront;

  /* auxiliary variables */
  int num_gridpoints;       /* number of grid points */
  int i, j;                 /* loop variables */
  double *ptr;              /* pointer to field data */


  /******************************************************
   * set up appropriate grid point update and front
   * detection/initialization functions based on the
   * specified spatial derivative order
   ******************************************************/
  if (spatial_derivative_order == 1) {
    initializeFront = 
      &FMM_initializeFront_FieldExtension2d_Order1;
    updateGridPoint = &FMM_updateGridPoint_FieldExtension2d_Order1;
  } else if (spatial_derivative_order == 2) {
/*  KTC - ADD LATER
    initializeFront = 
      &FMM_initializeFront_FieldExtension2d_Order2;
    updateGridPoint = &FMM_updateGridPoint_FieldExtension2d_Order2;
*/
    fprintf(stderr,
           "ERROR: second-order spatial derivatives currently unsupported\n");
    return LSM_FMM_2D_ERR_INVALID_SPATIAL_DERIVATIVE_ORDER;
  } else {
    fprintf(stderr,
           "ERROR: Invalid spatial derivative order.  Only first-\n");
    fprintf(stderr,
           "       and second-order finite differences supported.\n");
    return LSM_FMM_2D_ERR_INVALID_SPATIAL_DERIVATIVE_ORDER;
  }

  /********************************************
   * set up FMM Field Data
   ********************************************/
  FMM_FieldData *fmm_field_data = 
    (FMM_FieldData*) malloc(sizeof(FMM_FieldData));
  if (!fmm_field_data) return LSM_FMM_2D_ERR_FMM_DATA_CREATION_ERROR;
  fmm_field_data->phi = phi;
  fmm_field_data->distance_function = distance_function;
  fmm_field_data->num_extension_fields = num_extension_fields;
  fmm_field_data->source_fields = source_fields;
  fmm_field_data->extension_fields = extension_fields;
   
  /********************************************
   * initialize phi and extension fields
   ********************************************/
  num_gridpoints = 1;
  for (i = 0; i < LSM_FMM_2D_NDIM; i++) {
    num_gridpoints *= grid_dims[i];
  }
  for (i = 0, ptr = distance_function; i < num_gridpoints; i++, ptr++) {
    *ptr = LSM_FMM_2D_DEFAULT_UPDATE_VALUE;
  }

  for (j = 0; j < num_extension_fields; j++) {
    for (i = 0, ptr = extension_fields[j]; i < num_gridpoints; i++, ptr++) {
      *ptr = LSM_FMM_2D_DEFAULT_UPDATE_VALUE;
    }
  }

  /********************************************
   * initialize FMM Core Data
   ********************************************/
  fmm_core_data = FMM_Core_createFMM_CoreData(
    fmm_field_data,
    LSM_FMM_2D_NDIM,
    grid_dims,
    dx,
    initializeFront,
    updateGridPoint);
  if (!fmm_core_data) return LSM_FMM_2D_ERR_FMM_DATA_CREATION_ERROR;

  /* initialize grid points around the front */ 
  FMM_Core_initializeFront(fmm_core_data); 

  /* mark grid points outside of domain */
  for (j = 0; j < grid_dims[1]; j++) {
    for (i = 0; i < grid_dims[0]; i++) {
      int idx_ij = LSM_FMM_2D_IDX(i,j);

      if ((mask) && (mask[idx_ij] < 0)) {
        int grid_idx[LSM_FMM_2D_NDIM];
        grid_idx[0] = i; grid_idx[1] = j;
        FMM_Core_markPointOutsideDomain(fmm_core_data, grid_idx);
      }
    }
  }

  /* update remaining grid points */
  double lastValue = -1;
  while (FMM_Core_moreGridPointsToUpdate(fmm_core_data) &&
         (max_value < 0 || lastValue < max_value)) {
    lastValue = FMM_Core_advanceFront(fmm_core_data);
  }

  /* clean up memory */
  FMM_Core_destroyFMM_CoreData(fmm_core_data);
  free(fmm_field_data);

  return LSM_FMM_2D_ERR_SUCCESS;
}

/* 
 * computeDistanceFunction2d() just calls computeExtensionFields2d()
 * with no source/extension fields (i.e. NULL source/extension field
 * pointers).
 */
int computeDistanceFunction2d(
  double *distance_function,
  double *phi,
  double *mask,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx)
{
  return computeExtensionFields2d(
           distance_function,
           0, /*  NULL extension fields pointer */
           phi,
           mask,
           0, /*  NULL source fields pointer */
           0, /*  zero extension fields to compute */
           spatial_derivative_order,
           grid_dims,
           dx);
}

void FMM_initializeFront_FieldExtension2d_Order1(
  FMM_CoreData *fmm_core_data,
  FMM_FieldData *fmm_field_data,
  int num_dims,
  int *grid_dims,
  double *dx)
{
  /* FMM Field Data variables */
  double *phi = fmm_field_data->phi;
  double *distance_function = fmm_field_data->distance_function; 
  int num_extension_fields = fmm_field_data->num_extension_fields; 
  double **source_fields = fmm_field_data->source_fields; 
  double **extension_fields = fmm_field_data->extension_fields; 
   
  /* grid variables */
  int grid_idx[LSM_FMM_2D_NDIM];
  int offset[LSM_FMM_2D_NDIM];
  int neighbor[LSM_FMM_2D_NDIM];
  int on_interface = LSM_FMM_2D_FALSE;
  int borders_interface = LSM_FMM_2D_FALSE;

  /* distance function variables */
  double phi_cur;
  double phi_minus;
  double phi_plus;
  double dist_minus;
  double dist_plus;
  double dist_dir;
  int use_plus; 

  double dist_inv_sq; 
  double dist_inv_sq_dir; 

  /* variables for extension field calculations */
  double *extension_fields_cur;
  double *extension_fields_div_dist_sq;
  double *extension_fields_minus;
  double *extension_fields_plus;

  /* auxilliary variables */
  int i,j;  /* loop variables for grid */
  int idx_ij;
  int idx_neighbor;
  int m;    /* loop variable for extension fields */
  int l;    /* extra loop variable */
  int dir;  /* loop variable over spatial dimensions */

  /* unused function parameters */
  (void) num_dims;


  /* allocate memory for extension field calculations */
  extension_fields_cur = (double*) malloc(num_extension_fields*sizeof(double));
  extension_fields_div_dist_sq = 
    (double*) malloc(num_extension_fields*sizeof(double));
  extension_fields_minus = 
    (double*) malloc(num_extension_fields*sizeof(double));
  extension_fields_plus = 
    (double*) malloc(num_extension_fields*sizeof(double));

  /*
   * loop through cells in grid to find the border of the zero level set
   */
  for (j=0; j<grid_dims[1]; j++) {
    for (i=0; i<grid_dims[0]; i++) {
      on_interface = LSM_FMM_2D_FALSE;
      borders_interface = LSM_FMM_2D_FALSE;

      /* compute index for (i,j) grid point */
      idx_ij = LSM_FMM_2D_IDX(i,j);

      /* get data values at the current grid point */
      phi_cur = phi[idx_ij];
      for (m = 0; m < num_extension_fields; m++) {
        extension_fields_cur[m] = source_fields[m][idx_ij];
      }

      /* zero out accumulation variables */
      dist_inv_sq = 0; 
      for (m = 0; m < num_extension_fields; m++) {
        extension_fields_div_dist_sq[m] = 0;
      }
  
      /* loop over neighbors */
      for (dir = 0; dir < LSM_FMM_2D_NDIM; dir++) {
        for (l = 0; l < LSM_FMM_2D_NDIM; l++) { /* reset offset */
          offset[l] = 0; 
        }

        /* reset plus and minus distances */
        dist_plus = DBL_MAX;
        dist_minus = DBL_MAX;

        /* reset plus and minus extension field values */
        for (m = 0; m < num_extension_fields; m++) {
          extension_fields_minus[m] = 0;
          extension_fields_plus[m] = 0;
        }
  
        /* calculate distance to interface in minus direction */
        offset[dir] = -1;
        neighbor[0] = i+offset[0]; neighbor[1] = j+offset[1];
        if (!LSM_FMM_2D_IDX_OUT_OF_BOUNDS(neighbor[0],neighbor[1],grid_dims)) {
          idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
          phi_minus = phi[idx_neighbor];
          if (phi_minus*phi_cur <= 0) {

            if (phi_cur == 0) {
              dist_minus = 0.0;
            } else {
              dist_minus = phi_cur/(phi_cur-phi_minus);

              for (m = 0; m < num_extension_fields; m++) {
                extension_fields_minus[m] = extension_fields_cur[m] 
                  + dist_minus*(source_fields[m][idx_neighbor]
                               -extension_fields_cur[m]);
              }
  
              /* multiply back in the units for dist_minus */
              dist_minus *= dx[dir];
            }
          }
        }
        
        /* calculate distance to interface in plus direction */
        offset[dir] = 1;
        neighbor[0] = i+offset[0]; neighbor[1] = j+offset[1];
        if (!LSM_FMM_2D_IDX_OUT_OF_BOUNDS(neighbor[0],neighbor[1],grid_dims)) {
          idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
          phi_plus = phi[idx_neighbor];
          if (phi_plus*phi_cur <= 0) {

            if (phi_cur == 0) {
              dist_plus = 0.0;
            } else {
              dist_plus = phi_cur/(phi_cur-phi_plus);
  
              for (m = 0; m < num_extension_fields; m++) {
                extension_fields_plus[m] = extension_fields_cur[m] 
                  + dist_plus*(source_fields[m][idx_neighbor]
                              -extension_fields_cur[m]);
              }
  
              /* multiply back in the units for dist_plus */
              dist_plus *= dx[dir];
            }
          }
        }

        /* check if current grid point lies on or borders the interface */
        if ( (dist_plus == 0) || (dist_minus == 0) ) {

          /* the current grid point lies directly on the interface.   */
          /* no need to check other directions, so break out of loop. */
          on_interface = LSM_FMM_2D_TRUE; 
          break;

        } else if ( (dist_plus < DBL_MAX) || (dist_minus < DBL_MAX) ) {
          borders_interface = LSM_FMM_2D_TRUE; 
  
          if (dist_plus < dist_minus) {
            dist_dir = dist_plus;
            use_plus = LSM_FMM_2D_TRUE;
          } else {
            dist_dir = dist_minus;
            use_plus = LSM_FMM_2D_FALSE;
          }
 
          /* update 1/dist^2 and ext_field/dist^2 values with */
          /* information from current coordinate direction    */
          dist_inv_sq_dir = 1/dist_dir/dist_dir;
          dist_inv_sq += dist_inv_sq_dir; 

          if (use_plus) {
            for (m = 0; m < num_extension_fields; m++) {
              extension_fields_div_dist_sq[m] += 
                extension_fields_plus[m]*dist_inv_sq_dir;
            }
          } else {
            for (m = 0; m < num_extension_fields; m++) {
              extension_fields_div_dist_sq[m] += 
                extension_fields_minus[m]*dist_inv_sq_dir;
            }
          }

        } /* end cases: current grid point on or borders interface */

      } /* end loop over neighbors */
 
      /* set distance function and extension field of grid points */
      /* on or bordering the zero level set                       */
      if (on_interface) { 

        distance_function[idx_ij] = 0.0;

        /* compute extension field value */
        for (m = 0; m < num_extension_fields; m++) {
          extension_fields[m][idx_ij] = extension_fields_cur[m];
        }

        /* set grid point as an initial front point */
        grid_idx[0] = i; grid_idx[1] = j;
        FMM_Core_setInitialFrontPoint(fmm_core_data, grid_idx,
                                      distance_function[idx_ij]);

      } else if (borders_interface) { 

        /* compute updated value for the signed distance function */
        if (phi_cur > 0)
          distance_function[idx_ij] = 1/sqrt(dist_inv_sq);
        else 
          distance_function[idx_ij] = -1/sqrt(dist_inv_sq);
  
        /* compute extension field value */
        for (m = 0; m < num_extension_fields; m++) {
          extension_fields[m][idx_ij] = 
            extension_fields_div_dist_sq[m]/dist_inv_sq;
        }

        /* set grid point as an initial front point */
        grid_idx[0] = i; grid_idx[1] = j;
        FMM_Core_setInitialFrontPoint(fmm_core_data, grid_idx,
                                      distance_function[idx_ij]);

      } /* end handling grid points on or near interface */

    }
  }  /* end loop over grid */

  /* clean up memory */
  free(extension_fields_cur); 
  free(extension_fields_div_dist_sq); 
  free(extension_fields_minus);
  free(extension_fields_plus);
}


double FMM_updateGridPoint_FieldExtension2d_Order1(
  FMM_CoreData *fmm_core_data,
  FMM_FieldData *fmm_field_data,
  int *grid_idx,
  int num_dims,
  int *grid_dims,
  double *dx)
{
  int *gridpoint_status = FMM_Core_getGridPointStatusDataArray(fmm_core_data);

  /* FMM Field Data variables */
  double *distance_function = fmm_field_data->distance_function; 
  int num_extension_fields = fmm_field_data->num_extension_fields; 
  double **extension_fields = fmm_field_data->extension_fields; 

  /* variables for extension field calculations */
  double *extension_fields_numerator;
  double *extension_fields_denominator;

  /* variables used in distance function update */
  PointStatus  neighbor_status;
  int use_plus[LSM_FMM_2D_NDIM];
  int dir_used[LSM_FMM_2D_NDIM];
  double phi_upwind[LSM_FMM_2D_NDIM];
  double phi_plus;
  double inv_dx_sq; 
  int offset[LSM_FMM_2D_NDIM]; 
  int neighbor[LSM_FMM_2D_NDIM];

  /* variables used in extension field update */
  double ext_val_neighbor;
  double dist_diff; 

  /* coefficients of quadratic equation for the updated distance function */
  double phi_A = 0;
  double phi_B = 0;
  double phi_C = 0;
  double discriminant;
  double dist_updated;

  /* auxilliary variables */
  int dir;  /* loop variable for spatial directions */
  int k;    /* loop variable for extension fields */
  int l;    /* extra loop variable */ 
  int idx_cur_gridpoint, idx_neighbor;

  /* unused function parameters */
  (void) num_dims;

  /* allocate memory extension field calculations */
  extension_fields_numerator = 
    (double*) malloc(num_extension_fields*sizeof(double));
  extension_fields_denominator = 
    (double*) malloc(num_extension_fields*sizeof(double));;
  for (k = 0; k < num_extension_fields; k++) {
    extension_fields_numerator[k] = 0;
    extension_fields_denominator[k] = 0;
  }

  /* calculate update to distance function */
  for (dir = 0; dir < LSM_FMM_2D_NDIM; dir++) { /* loop over coord directions */
    for (l = 0; l < LSM_FMM_2D_NDIM; l++) { /* reset offset */
      offset[l] = 0; 
    }

    /* changed to true if has KNOWN neighbor */
    dir_used[dir] = LSM_FMM_2D_FALSE;  

    /* find "upwind" direction and phi value */
    phi_upwind[dir] = DBL_MAX;

    /* check minus direction */
    offset[dir] = -1;
    neighbor[0] = grid_idx[0] + offset[0];
    neighbor[1] = grid_idx[1] + offset[1];
    if (!LSM_FMM_2D_IDX_OUT_OF_BOUNDS(neighbor[0],neighbor[1],grid_dims)) {
      idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
      neighbor_status = (PointStatus) gridpoint_status[idx_neighbor];
      if (KNOWN == neighbor_status) {
        phi_upwind[dir] = distance_function[idx_neighbor];
        use_plus[dir] = LSM_FMM_2D_FALSE;
        dir_used[dir] = LSM_FMM_2D_TRUE;
      }
    }

    /* check plus direction */
    offset[dir] = 1;
    neighbor[0] = grid_idx[0] + offset[0];
    neighbor[1] = grid_idx[1] + offset[1];
    if (!LSM_FMM_2D_IDX_OUT_OF_BOUNDS(neighbor[0],neighbor[1],grid_dims)) {
      idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
      neighbor_status = (PointStatus) gridpoint_status[idx_neighbor];
      if (KNOWN == neighbor_status) {
        phi_plus = distance_function[idx_neighbor];

        /* 
         * choosing the upwind direction to be the direction
         * with the smaller abs(phi) value gives a consistent 
         * solution to the "upwind" Eikonal equation.
         * NOTE: to avoid having to use the C math library,
         *       we define our own ABS macro
         */
        if (LSM_FMM_2D_ABS(phi_plus) < LSM_FMM_2D_ABS(phi_upwind[dir])) {
          phi_upwind[dir] = phi_plus;
          use_plus[dir] = LSM_FMM_2D_TRUE;
          dir_used[dir] = LSM_FMM_2D_TRUE;
        }
      }
    }

    /*
     * accumulate coefficients for updated distance function
     * if either of the neighbors are "known"
     */
    if (phi_upwind[dir] < DBL_MAX) {
      /* accumulate coefs for updated distance function */ 
      inv_dx_sq = 1/dx[dir]/dx[dir];
      phi_A += inv_dx_sq;
      phi_B += phi_upwind[dir]*inv_dx_sq;
      phi_C += phi_upwind[dir]*phi_upwind[dir]*inv_dx_sq;
    }

  } /* loop over coordinate directions */

  /* complete computation of phi_B and phi_C */
  phi_B *= -2.0;
  phi_C -= 1;  /* F_ij = 1 for a distance function calculation */

  /* compute updated distance function by solving quadratic equation */
  discriminant = phi_B*phi_B - 4*phi_A*phi_C;
  dist_updated = DBL_MAX;
  if (discriminant >= 0) {
    if (phi_B < 0) { /* distance function of neighbors is positive */
      dist_updated = (-phi_B + sqrt(discriminant))/2/phi_A;
    } else if (phi_B > 0) { /* distance function of neighbors is negative */
      dist_updated = (-phi_B - sqrt(discriminant))/2/phi_A;
    } else { 
      /* grid point is ON the interface, so keep it there */
      dist_updated = 0;
    } 
  } else {
    fprintf(stderr,"ERROR: phi update - discriminant negative!!!\n");
    if (phi_B > 0) { /* distance function of neighbors is negative */
      dist_updated *= -1;
    }
  }


  /* calculate extension field values */
  for (dir = 0; dir < LSM_FMM_2D_NDIM; dir++) { /* loop over coord directions */

    /*
     * only accumulate values from the current direction if this
     * direction was used in the update of the distance function
     */
    if (dir_used[dir]) {
      for (l = 0; l < LSM_FMM_2D_NDIM; l++) { /* reset offset */
        offset[l] = 0; 
      }
      offset[dir] = (use_plus[dir] ? 1 : -1);
      neighbor[0] = grid_idx[0] + offset[0];
      neighbor[1] = grid_idx[1] + offset[1];
      idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
  
      for (k = 0; k < num_extension_fields; k++) {
        ext_val_neighbor = extension_fields[k][idx_neighbor];
        dist_diff = dist_updated - phi_upwind[dir];
        extension_fields_numerator[k] += ext_val_neighbor*dist_diff;
        extension_fields_denominator[k] += dist_diff;
      }
    }
  } /* loop over coordinate directions */


  /* set updated quantities */
  idx_cur_gridpoint = LSM_FMM_2D_IDX(grid_idx[0],grid_idx[1]);
  distance_function[idx_cur_gridpoint] = dist_updated;
  for (k = 0; k < num_extension_fields; k++) {
    extension_fields[k][idx_cur_gridpoint] =
      extension_fields_numerator[k]/extension_fields_denominator[k];
  }

  /* free memory allocated for extension field calculations */
  free(extension_fields_numerator);
  free(extension_fields_denominator);

  return dist_updated;
}

bool isSimplePoint(int* grid_point_status, double* background, int* grid_idx, int neighbors[][3], 
                   int* grid_dims)
{
  /* grid variables */
  int offset[LSM_FMM_2D_NDIM];
  int neighbor[LSM_FMM_2D_NDIM];

  /* auxilliary variables */
  int idx_ij;
  int idx_neighbor;

  int numNeighbors = 0;
  int numBoundaryNeighbors = 0;
    
  idx_ij = LSM_FMM_2D_IDX(grid_idx[0],grid_idx[1]);

  for (offset[0] = -1; offset[0] <= 1; offset[0]++) {
    for (offset[1] = -1; offset[1] <= 1; offset[1]++) {
        
      neighbors[offset[0]+1][offset[1]+1] = 0;
/*      if (grid_idx[0] + offset[0] == 0 ||
          grid_idx[0] + offset[0] == grid_dims[0]-1 ||
          grid_idx[1] + offset[1] == 0 ||
          grid_idx[1] + offset[1] == grid_dims[1]-1)  {
        numBoundaryNeighbors++;
      }*/
        
      neighbor[0] = grid_idx[0]+offset[0]; 
      neighbor[1] = grid_idx[1]+offset[1];
          
          // Found a neigbouring point that is inside the contour
          // Insert it into the heap and update its status to known
      if (!LSM_FMM_2D_IDX_OUT_OF_BOUNDS(neighbor[0],neighbor[1],grid_dims)) {
            
        idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
            
        if (grid_point_status[idx_neighbor] != KNOWN &&
            background[idx_neighbor] >= 0 &&
            idx_neighbor != idx_ij)
        {
          neighbors[offset[0]+1][offset[1]+1] = 1;
          numNeighbors++;
        }
      }
    }
  }
    
  int numEdges = 0;
    
  for (int i=0; i < 2; i++) {
    for (int j=0; j < 3; j++) {
      if (neighbors[i][j] == 1 && neighbors[i+1][j] == 1) {
        numEdges++;
      }
    }
  }
  for (int i=0; i < 3; i++) {
    for (int j=0; j < 2; j++) {
      if (neighbors[i][j] == 1 && neighbors[i][j+1] == 1) {
        numEdges++;
      }
    }
  }
  numEdges += (neighbors[0][1] == 1 && neighbors[1][0] == 1 && neighbors[0][0] == 0)? 1 : 0;
  numEdges += (neighbors[0][1] == 1 && neighbors[1][2] == 1 && neighbors[0][2] == 0)? 1 : 0;
  numEdges += (neighbors[1][2] == 1 && neighbors[2][1] == 1 && neighbors[2][2] == 0)? 1 : 0;
  numEdges += (neighbors[1][0] == 1 && neighbors[2][1] == 1 && neighbors[2][0] == 0)? 1 : 0;
    
    //mexPrintf("Heap size: %d, Value: %g \n", FMM_Heap_getHeapSize(known_points), node.value);
//   bool bNonSimpleBoundaryPoint = false;
//   bool bBoundaryPoint = false;
//   
//   if (!bBoundaryPoint && grid_idx[0] == 0) {
//     bBoundaryPoint = true;
//     bNonSimpleBoundaryPoint = neighbors[1][0] == 0 && neighbors[1][2] == 0;
//   }  
//   if (!bBoundaryPoint && grid_idx[0] == grid_dims[0]-1) {
//     bBoundaryPoint = true;
//     bNonSimpleBoundaryPoint = neighbors[1][0] == 0 && neighbors[1][2] == 0;
//   }  
//   if (!bBoundaryPoint && grid_idx[1] == 0) {
//     bBoundaryPoint = true;
//     bNonSimpleBoundaryPoint = neighbors[0][1] == 0 && neighbors[2][1] == 0;
//   }  
//   if (!bBoundaryPoint && grid_idx[1] == grid_dims[1]-1) {
//     bBoundaryPoint = true;
//     bNonSimpleBoundaryPoint = neighbors[0][1] == 0 && neighbors[2][1] == 0;
//   }
//     
  //  Check if the point is removable
/*  return (!bBoundaryPoint && numNeighbors > numBoundaryNeighbors && (numNeighbors - numEdges == 1)) ||
      (bBoundaryPoint && !bNonSimpleBoundaryPoint);*/
  return (numNeighbors - numEdges == 1);

}

bool isJunction(double *img, int *grid_idx, int* grid_dims)
{
  int neighbors[3][3];
  int neighbor[2];
  int numEdges = 0;
  int idx_neighbor;
  int holes[8];
  int holesCount = 0;
  int numNeighbors = 0;
  
  for (int i=0; i < 3; i++) {
    for (int j=0; j < 3; j++) {
      neighbor[0] = grid_idx[0] + i - 1;
      neighbor[1] = grid_idx[1] + j - 1;
      idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
      neighbors[i][j] = (img[idx_neighbor] >= 0 ? 1 : 0);
      if (neighbors[i][j] == 1)
      {
        numNeighbors++;
      }
    }
  }
  
  holesCount += (neighbors[0][0] == 1 && neighbors[1][0] == 0? 1 : 0);
  holesCount += (neighbors[0][1] == 1 && neighbors[0][0] == 0? 1 : 0);
  holesCount += (neighbors[0][2] == 1 && neighbors[0][1] == 0? 1 : 0);
  holesCount += (neighbors[1][2] == 1 && neighbors[0][2] == 0? 1 : 0);
  holesCount += (neighbors[2][2] == 1 && neighbors[1][2] == 0? 1 : 0);
  holesCount += (neighbors[2][1] == 1 && neighbors[2][2] == 0? 1 : 0);
  holesCount += (neighbors[2][0] == 1 && neighbors[2][1] == 0? 1 : 0);
  holesCount += (neighbors[1][0] == 1 && neighbors[2][0] == 0? 1 : 0);
  
  if (holesCount >= 3)
    mexPrintf("Junction \n");
  
  return holesCount >= 3;
}

void FMM_initializeHomotopicThinning(FMM_CoreData *fmm_core_data, double *background)
{
#define DISTANCE_THINNING
  
  int *grid_dims;
  FMM_FieldData *fmm_field_data;
  FMM_Heap *known_points;
#ifndef DISTANCE_THINNING 
  deque<FMM_ListNode> borderPoints;
#endif
  
  FMM_Core_initializeHomotopicThinning(fmm_core_data, fmm_field_data, grid_dims, known_points); 
  
  /* Initialize the background boundary points
  /* FMM Field Data variables */
  double *phi = fmm_field_data->phi;
  double *thinned_img = fmm_field_data->distance_function;
   
  /* grid variables */
  int grid_idx[LSM_FMM_2D_NDIM];
  int offset[LSM_FMM_2D_NDIM];
  int neighbor[LSM_FMM_2D_NDIM];

  /* distance function variables */
  double phi_cur;

  /* auxilliary variables */
  int idx_ij;
  int idx_neighbor;

  int* grid_point_status = FMM_Core_getGridPointStatusDataArray(fmm_core_data);
  
  /*
  * loop through cells in grid to find the border of the zero level set
  */
  for (int j=0; j<grid_dims[1]; j++) {
    for (int i=0; i<grid_dims[0]; i++) {
      /* compute index for (i,j) grid point */
      
      idx_ij = LSM_FMM_2D_IDX(i,j);
      
      /* get data values at the current grid point */
      grid_idx[0] = i; grid_idx[1] = j;
      phi_cur = phi[idx_ij];
  
      if (background[idx_ij] < 0) {
        continue;
      }
        
      // The current point is on the background
      
      /* loop over neighbors */
      bool bPointInserted = false;
      for (offset[0] = -1; !bPointInserted && offset[0] <= 1; offset[0]++) {
        for (offset[1] = -1; !bPointInserted && offset[1] <= 1; offset[1]++) {
          neighbor[0] = i+offset[0]; neighbor[1] = j+offset[1];
          
          // Found a neigbouring point that is inside the contour
          // Insert it into the heap and update its status to known
          if (!LSM_FMM_2D_IDX_OUT_OF_BOUNDS(neighbor[0],neighbor[1],grid_dims)) {
            
            idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
            
            if (idx_neighbor != idx_ij &&
                background[idx_neighbor] < 0) {
#ifdef DISTANCE_THINNING 
             FMM_Core_setInitialFrontPoint(fmm_core_data, grid_idx,
                                           phi_cur);
#else              
              FMM_ListNode listNode;
              listNode.value = phi_cur;
              listNode.grid_idx[0] = grid_idx[0];
              listNode.grid_idx[1] = grid_idx[1];
              borderPoints.push_back(listNode);
#endif              
              grid_point_status[idx_ij] = TRIAL;
              thinned_img[idx_ij] =  1;
              bPointInserted = true;             
            }
          }
        }
      }
    }   
  }       
  
  int neighbors[3][3];
  int neighbors_of_neighbors[3][3];
  
  while (
#ifdef DISTANCE_THINNING 
         !FMM_Heap_isEmpty(known_points)
#else
         !borderPoints.empty()
#endif        
        ) {
      /* extract grid index of next known point */
#ifdef DISTANCE_THINNING 
    FMM_HeapNode node = FMM_Heap_extractMin(known_points,
                                            FMM_CORE_NULL,
                                            FMM_CORE_NULL);
#else
    FMM_ListNode node = borderPoints.front();
    borderPoints.pop_front();
#endif        
    
    idx_ij = LSM_FMM_2D_IDX(node.grid_idx[0],node.grid_idx[1]);
    
    grid_point_status[idx_ij] = KNOWN;
    
    if (isSimplePoint(grid_point_status, background, node.grid_idx, neighbors, grid_dims)) {
      thinned_img[idx_ij] = -1;
      for (offset[0] = -1; offset[0] <= 1; offset[0]++) {
        for (offset[1] = -1; offset[1] <= 1; offset[1]++) {
          if (neighbors[offset[0]+1][offset[1]+1] == 1) {
            neighbor[0] = node.grid_idx[0]+offset[0]; 
            neighbor[1] = node.grid_idx[1]+offset[1];
            idx_neighbor = LSM_FMM_2D_IDX(neighbor[0],neighbor[1]);
            
            if (grid_point_status[idx_neighbor] == FAR &&
                isSimplePoint(grid_point_status, background, neighbor, neighbors_of_neighbors, grid_dims))
            {
#ifdef DISTANCE_THINNING 
              FMM_Core_setNextThinnedPoint(fmm_core_data, neighbor, phi[idx_neighbor]);
#else
              FMM_ListNode listNode;
              listNode.value = phi[idx_neighbor];
              listNode.grid_idx[0] = neighbor[0];
              listNode.grid_idx[1] = neighbor[1];
              borderPoints.push_back(listNode);
              grid_point_status[idx_neighbor] = TRIAL;
#endif
              thinned_img[idx_neighbor] =  1;
            }
          }
        }
      }
    }
    else {
      grid_point_status[idx_ij] = FAR;
    }
  }   
  
  return;
  
  // Post processing to remove boudnary edges
  
  // Next look 2 pixels away from the boundary
  // Remove all pixels that are not further connected inside
  // For the remaining pixels, add a corresponding pixel to the boundary
  for (int j=1; j<grid_dims[1]-1; j++) {
    idx_ij = LSM_FMM_2D_IDX(1,j);
    if (thinned_img[idx_ij] >= 0) {
      
      bool bBoundaryPoint = false;
      int idx[] = {1,j};
      
      if (isJunction(thinned_img, idx,grid_dims))
      {
        bBoundaryPoint = true;
      }
      
      if (bBoundaryPoint) {
        idx_neighbor = LSM_FMM_2D_IDX(0,j);
        grid_point_status[idx_neighbor] = OUTSIDE_DOMAIN;
        thinned_img[idx_neighbor] = phi[idx_neighbor];
      }
      else {
        grid_point_status[idx_ij] = KNOWN;
        //thinned_img[idx_ij] = -1;
      }
    }
    
    idx_ij = LSM_FMM_2D_IDX(grid_dims[0]-2,j);
    if (thinned_img[idx_ij] >= 0) {
      bool bBoundaryPoint = false;
      int idx[] = {grid_dims[0]-2,j};
      
      if (isJunction(thinned_img, idx, grid_dims))
      {
        bBoundaryPoint = true;
      }
      
      if (bBoundaryPoint) {
        idx_neighbor = LSM_FMM_2D_IDX(grid_dims[0]-1,j);
        grid_point_status[idx_neighbor] = OUTSIDE_DOMAIN;
        thinned_img[idx_neighbor] = phi[idx_neighbor];
      }
      else {
        grid_point_status[idx_ij] = KNOWN;
        //thinned_img[idx_ij] = -1;
      }
    }
  }
  
  
  for (int i=1; i<grid_dims[0]-1; i++) {
    idx_ij = LSM_FMM_2D_IDX(i,1);
    if (thinned_img[idx_ij] >= 0) {
      bool bBoundaryPoint = false;
      int idx[] = {i,1};
      
      if (isJunction(thinned_img, idx, grid_dims))
      {
        bBoundaryPoint = true;
      }
      
      if (bBoundaryPoint) {
        idx_neighbor = LSM_FMM_2D_IDX(i,0);
        grid_point_status[idx_neighbor] = OUTSIDE_DOMAIN;
        thinned_img[idx_neighbor] = phi[idx_neighbor];
      }
      else {
        grid_point_status[idx_ij] = KNOWN;
        //thinned_img[idx_ij] = -1;
      }
    }
    
    idx_ij = LSM_FMM_2D_IDX(i,grid_dims[1]-2);
    if (thinned_img[idx_ij] >= 0) {
      bool bBoundaryPoint = false;
      int idx[] = {i,grid_dims[1]-2};
      
      if (isJunction(thinned_img, idx, grid_dims))
      {
        bBoundaryPoint = true;
      }
      
      if (bBoundaryPoint) {
        idx_neighbor = LSM_FMM_2D_IDX(i,grid_dims[1]-1);
        grid_point_status[idx_neighbor] = OUTSIDE_DOMAIN;
        thinned_img[idx_neighbor] = phi[idx_neighbor];
      }
      else {
        grid_point_status[idx_ij] = KNOWN;
        //thinned_img[idx_ij] = -1;
      }
    }
  }
  
  for (int j=1; j<grid_dims[1]-1; j++) {
    idx_ij = LSM_FMM_2D_IDX(1,j);
    if (grid_point_status[idx_ij] == KNOWN)
    {
      thinned_img[idx_ij] = -1;
    }
    idx_ij = LSM_FMM_2D_IDX(grid_dims[0]-2,j);
    if (grid_point_status[idx_ij] == KNOWN)
    {
      thinned_img[idx_ij] = -1;
    }
  }
    
  for (int i=1; i<grid_dims[0]-1; i++) {
    idx_ij = LSM_FMM_2D_IDX(i,1);
    if (grid_point_status[idx_ij] == KNOWN)
    {
      thinned_img[idx_ij] = -1;
    }
    idx_ij = LSM_FMM_2D_IDX(i,grid_dims[1]-2);
    if (grid_point_status[idx_ij] == KNOWN)
    {
      thinned_img[idx_ij] = -1;
    }
  } 
   
  //Remove all the pixels 1 away from the boundary
  for (int j=0; j<grid_dims[1]; j++) {
    idx_ij = LSM_FMM_2D_IDX(0,j);
    if (grid_point_status[idx_ij] != OUTSIDE_DOMAIN)
    {
      thinned_img[idx_ij] = -1;
    }
    idx_ij = LSM_FMM_2D_IDX(grid_dims[0]-1,j);
    if (grid_point_status[idx_ij] != OUTSIDE_DOMAIN)
    {
      thinned_img[idx_ij] = -1;
    }
  }
  for (int i=0; i<grid_dims[0]; i++) {
    idx_ij = LSM_FMM_2D_IDX(i,0);
    if (grid_point_status[idx_ij] != OUTSIDE_DOMAIN)
    {
      thinned_img[idx_ij] = -1;
    }
    idx_ij = LSM_FMM_2D_IDX(i,grid_dims[1]-1);
    if (grid_point_status[idx_ij] != OUTSIDE_DOMAIN)
    {
      thinned_img[idx_ij] = -1;
    }
  }
  
  
}

int doHomotopicThinning(
  double *thinned_img,
  double *phi,
  double *background,
  int *grid_dims)
{
  /* fast marching method data */
  FMM_CoreData *fmm_core_data;

  /* pointers to callback functions */
  updateGridPointFuncPtr updateGridPoint = NULL;
  initializeFrontFuncPtr initializeFront = NULL;

  /* auxiliary variables */
  int num_gridpoints;       /* number of grid points */
  int i, j;                 /* loop variables */
  double *ptr_thinned, *ptr_back;              /* pointer to field data */
  double dx[2];
  int idx_ij;

  /********************************************
  * set up FMM Field Data
  ********************************************/
  FMM_FieldData *fmm_field_data = 
      (FMM_FieldData*) malloc(sizeof(FMM_FieldData));
  if (!fmm_field_data) return LSM_FMM_2D_ERR_FMM_DATA_CREATION_ERROR;
  fmm_field_data->phi = phi;
  fmm_field_data->distance_function = thinned_img;
  fmm_field_data->num_extension_fields = 0;
  fmm_field_data->source_fields = NULL;
  fmm_field_data->extension_fields = NULL;
   
  /********************************************
  * initialize phi and extension fields
  ********************************************/
  num_gridpoints = 1;
  for (i = 0; i < LSM_FMM_2D_NDIM; i++) {
    num_gridpoints *= grid_dims[i];
  }
  for (i = 0, ptr_thinned = thinned_img, ptr_back = background;
       i < num_gridpoints; i++, ptr_thinned++, ptr_back++) {
    if (*ptr_back > 0) {
      *ptr_thinned = 1;
    }
    else {
      *ptr_thinned = -1;
    }
  }

  for (i = 0; i < grid_dims[0]; i++) {
    idx_ij = LSM_FMM_2D_IDX(i,0);
    phi[idx_ij] = grid_dims[0] + grid_dims[1];
    background[idx_ij] = 1;
    idx_ij = LSM_FMM_2D_IDX(i,grid_dims[1]-1);
    phi[idx_ij] = grid_dims[0] + grid_dims[1];    
    background[idx_ij] = 1;
  }
  
  for (i = 0; i < grid_dims[1]; i++) {
    idx_ij = LSM_FMM_2D_IDX(0,i);
    phi[idx_ij] = grid_dims[0] + grid_dims[1];
    background[idx_ij] = 1;
    idx_ij = LSM_FMM_2D_IDX(grid_dims[0]-1,i);
    phi[idx_ij] = grid_dims[0] + grid_dims[1];    
    background[idx_ij] = 1;
  }
  
  /********************************************
  * initialize FMM Core Data
  ********************************************/
  fmm_core_data = FMM_Core_createFMM_CoreData(
      fmm_field_data,
      LSM_FMM_2D_NDIM,
      grid_dims,
      dx,
      initializeFront,
      updateGridPoint);
  
  if (!fmm_core_data) return LSM_FMM_2D_ERR_FMM_DATA_CREATION_ERROR;

  /* initialize grid points around the front */ 
  FMM_initializeHomotopicThinning(fmm_core_data, background); 

  FMM_Core_destroyFMM_CoreData(fmm_core_data);
  free(fmm_field_data);

  return 0;
}
