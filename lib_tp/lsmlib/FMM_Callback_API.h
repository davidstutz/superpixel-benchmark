/*
 * File:        FMM_Callback_API.h
 * Copyright:   (c) 2005-2006 Kevin T. Chu
 * Revision:    $Revision: 1.13 $
 * Modified:    $Date: 2006/08/13 13:32:08 $
 * Description: Definitions of callback function signatures for FMM algorithm
 */

#ifndef included_FMM_Callback_API_h
#define included_FMM_Callback_API_h

#include "FMM_Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \file FMM_Callback_API.h
 * 
 * \brief 
 * @ref FMM_Callback_API.h defines the callback interfaces required by 
 * the @ref FMM_Core.h functions to carry out the Fast Marching Method 
 * algorithm.
 * 
 */

/*!
 * FMM_FieldData is a data structure that MUST be defined by the user
 * in order for FMM_Core to function properly.  It should contain
 * the field  data for the fast marching method calculation (e.g. phi, 
 * distance function, source fields, extension fields).
 */
typedef struct FMM_FieldData FMM_FieldData;


 
/*==================== FMM_Callback_API Declarations =======================*/

/*!
 * initializeFront_CallbackFunc() defines the signature of 
 * the callback function required by FMM_Core_initializeFront() 
 * to find and initialize the front.
 *
 * IMPORTANT NOTE:  This callback function MUST call 
 *   FMM_Core_setInitialFrontPoint() to add grid points to 
 *   the initial front.  Otherwise, the initial front will
 *   be empty and the FMM calculation will yield incorrect
 *   results.
 *
 * Arguments:
 *  - fmm_core_data (in/out):       FMM_CoreData "object" actively managing 
 *                                  the FMM computation
 *  - fmm_field_data (in/out):      pointer to FMM_FieldData containing
 *                                  application specific field data
 *  - num_dims (in):                number of dimensions for FMM computation
 *  - grid_dims (in):               integer array of dimensions of 
 *                                  computational grid
 *  - dx (in):                      double array containing grid cell sizes
 *                                  in each of the coordinate directions
 *
 * Return value:                    none
 *
 */
void initializeFront_CallbackFunc(
  FMM_CoreData *fmm_core_data,
  FMM_FieldData *fmm_field_data,
  int num_dims,
  int *grid_dims,
  double *dx);

/*!
 * updateGridPoint_CallbackFunc() defines the signature of the callback
 * function required by FMM_Core_updateNeighbors() to compute
 * the updated distance of the specified grid point using values of 
 * neighbors that have status "KNOWN". 
 *
 * Arguments:
 *  - fmm_core_data (in/out):       FMM_CoreData "object" actively managing 
 *                                  the FMM computation
 *  - fmm_field_data (in/out):      pointer to FMM_FieldData containing
 *                                  application specific field data
 *  - grid_idx (in):                integer array containing the grid index 
 *                                  of the grid point to update
 *  - num_dims (in):                number of dimensions for FMM computation
 *  - grid_dims (in):               integer array of dimensions of computational
 *                                  grid
 *  - dx (in):                      double array containing grid cell sizes
 *                                  in each of the coordinate directions
 *
 * Return value:                    updated distance function value at
 *                                  specified grid point
 *
 */
double updateGridPoint_CallbackFunc(
  FMM_CoreData *fmm_core_data, 
  FMM_FieldData *fmm_field_data, 
  int *grid_idx,
  int num_dims,
  int *grid_dims,
  double *dx);

#ifdef __cplusplus
}
#endif

#endif
