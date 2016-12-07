/*
 * File:        FMM_Core.h
 * Copyright:   (c) 2005-2006 Kevin T. Chu
 * Revision:    $Revision: 1.16 $
 * Modified:    $Date: 2006/08/13 13:32:08 $
 * Description: Header file for core Fast Marching Method algorithm functions
 */

#ifndef included_FMM_Core_h
#define included_FMM_Core_h

#ifdef __cplusplus
extern "C" {
#endif

/*! \file FMM_Core.h
 *
 * \brief
 * @ref FMM_Core.h provides functionality for the numerics independent 
 * aspects of the Fast Marching Method algorithm in two- and 
 * three-dimensions.  
 *
 * Specific numerical calculations must be supplied by the user through 
 * callback functions for detecting/initializing the front and updating 
 * individual grid points.
 *
 * Dependencies:  @ref FMM_Heap.h and user-supplied callback routines
 *                                 
 * <h3> Usage: </h3>
 * 
 * -# Provide implementations for the callback functions defined in 
 *    @ref FMM_Callback_API.h.  
 * -# Create an FMM_CoreData structure using FMM_Core_createFMM_CoreData().
 * -# Initialize the front using FMM_Core_initializeFront().  
 * -# Mark grid points that are outside of the mathematical domain for 
 *    the problem using the FMM_Core_markPointOutsideDomain() function.
 * -# Advance the front as far as desired using FMM_Core_advanceFront().
 *    Typically, the front is advanced until there are no more grid 
 *    points to update.
 * -# Clean up the memory allocated for the FMM_CoreData using
 *    FMM_Core_destroyFMM_CoreData().
 *
 */

/*======================= FMM_Core Constants =========================*/
#define FMM_CORE_TRUE                   (1)
#define FMM_CORE_FALSE                  (0)
#define FMM_CORE_NULL                   (0)
#define FMM_CORE_MAX_NDIM               (FMM_HEAP_MAX_NDIM)

/*================== FMM_Core Type Declarations ======================*/

/*!
 * FMM_CoreData is a data structure that contains information about the 
 * current state of a fast marching method calculation.
 */
typedef struct FMM_CoreData FMM_CoreData;

/*!
 * FMM_FieldData is a user-defined data structure that contains the field 
 * data for the fast marching method calculation (e.g. phi, distance 
 * function, source fields, extension fields).
 */
typedef struct FMM_FieldData FMM_FieldData;

/*!
 * PointStatus is an enumerated type that represents the status of a
 * grid point during the Fast Marching Method computation.
 */
typedef enum { KNOWN, TRIAL, FAR, OUTSIDE_DOMAIN } PointStatus;

typedef struct FMM_Heap FMM_Heap;

/*!
 * initializeFrontFuncPtr is a function pointer to one of the
 * callback functions defined in @ref FMM_Callback_API.h, which must be
 * defined in order to use the functions in @ref FMM_Core.h.
 */
typedef void (*initializeFrontFuncPtr)(
  FMM_CoreData *fmm_core_data,
  FMM_FieldData *fmm_field_data,
  int num_dims,
  int *grid_dims,
  double *dx);

/*!
 * updateGridPointFuncPtr is a function pointer to one of the callback 
 * functions defined in @ref FMM_Callback_API.h, which must be defined 
 * in order to use the functions in @ref FMM_Core.h.
 */
typedef double (*updateGridPointFuncPtr)(  
  FMM_CoreData *fmm_core_data,
  FMM_FieldData *fmm_field_data,
  int *grid_idx,
  int num_dims,
  int *grid_dims,
  double *dx);


/*================== FMM_Core Function Declarations ==================*/

/*!
 * FMM_Core_createFMM_CoreData() allocates memory for an FMM_CoreData 
 * structure and initializes it using the specified configuration information.
 *
 * Arguments:
 *  - fmm_field_data (in):          pointer to FMM_FieldData data structure
 *  - num_dims (in):                number of dimensions for FMM computation
 *  - grid_dims (in):               integer array of dimensions of computational
 *                                  grid
 *  - dx (in):                      double array containing grid cell sizes 
 *                                  in each of the coordinate directions
 *  - initializeFront (in):         callback function pointer that is 
 *                                  used to find and initialize the front
 *                                  at the beginning of the FMM algorithm
 *                                  (see @ref FMM_Callback_API.h for more 
 *                                  details)
 *  - updateGridPoint (in):         callback function pointer that is 
 *                                  used to update individual grid points
 *                                  during the FMM algorithm
 *                                  (see @ref FMM_Callback_API.h for more 
 *                                  details)
 *
 * Return value:                    pointer to new FMM_CoreData structure
 *                                  containing the relevant information
 *                                  for an FMM calculation
 *
 * NOTES:
 *  - It is assumed that the user has allocated the memory and properly
 *    set up the FMM_FieldData.
 *
 *  - It is assumed that the grid_dims and dx arrays are at least 
 *    num_dims in length.
 *
 *  - The updateGridPoint() and initializeFront() callback functions
 *    MUST follow the protocol described in @ref FMM_Callback_API.h.
 *
 */
FMM_CoreData* FMM_Core_createFMM_CoreData(
  FMM_FieldData *fmm_field_data,
  int num_dims,
  int *grid_dims,
  double *dx,
  initializeFrontFuncPtr initializeFront,
  updateGridPointFuncPtr updateGridPoint);

/*!
 * FMM_Core_destroyFMM_CoreData() frees the memory associated with an 
 * FMM_CoreData structure.
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" to be destroyed
 *
 * Return value:           none
 *
 */
void FMM_Core_destroyFMM_CoreData(FMM_CoreData *fmm_core_data);

/*!
 * FMM_Core_initializeFront() sets the initial set of "known" and "trial"
 * points.  It first initializes the list of "known" points by 
 * calling the user-provided initializeFront() function to locate
 * the points that border the interface.  It then initializes the list 
 * of "trial" points by updating and adding all of the neighbors of 
 * "known" points.
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 *
 * Return value:           none
 *
 */
void FMM_Core_initializeFront(FMM_CoreData *fmm_core_data);

/*!
 * FMM_Core_setInitialFrontPoint() sets a grid point as being on the initial
 * front.
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 *  - grid_idx (in):       integer array containing the grid index of the 
 *                         grid point to set as an initial front point
 *  - value (in):          value of initial front point from zero level 
 *                         set (e.g. distance or arrival time)
 *
 * Return value:           none
 *
 * NOTES:
 *  - This function MUST be called during the user-defined 
 *    initializeFrontFuncPtr() callback function to add grid points
 *    to the initial front.  Otherwise, the initial front will remain 
 *    empty and the FMM calculation wll yield incorrect results.
 *
 *  - If the value of a grid point is set to DBL_MAX, then it will
 *    be treated as being outside of the domain of the problem.
 *
 *  - It is assumed that the size of the grid_idx array is at least 
 *    equal to the number of spatial dimensions of the problem. 
 *
 */
void FMM_Core_setInitialFrontPoint(
  FMM_CoreData *fmm_core_data, 
  int *grid_idx, 
  double value);

/*!
 * FMM_Core_markPointOutsideDomain() sets a grid point as being outside of 
 * the mathematical domain for the problem.
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 *  - grid_idx (in):       integer array containing the grid index of the 
 *                         grid point to set outside of the domain
 *
 * Return value:           none
 *
 * NOTES:
 *  - This function MUST be called before FMM_Core_advanceFront() 
 *    to set grid points as being outside of the mathematical domain 
 *    for the problem.  Otherwise, all grid points will be treated as
 *    being in the interior of the domain.
 *
 *  - It is assumed that the size of the grid_idx array is at least 
 *    equal to the number of spatial dimensions of the problem. 
 *
 */
void FMM_Core_markPointOutsideDomain(
  FMM_CoreData *fmm_core_data, 
  int *grid_idx);

/*!
 * FMM_Core_advanceFront() advances the front of "known" grid points by
 * a single grid point.  It basically carries out the main update
 * loop of the Fast Marching Method.
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 *
 * Return value:           The value of the last updated point
 *
 */
double FMM_Core_advanceFront(FMM_CoreData *fmm_core_data);

/*!
 * FMM_Core_moreGridPointsToUpdate() determines whether there are more grid 
 * points to update.
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 * 
 * Return value:           true (1) if there are more grid points to update;
 *                         false (0) otherwise
 *
 */
int FMM_Core_moreGridPointsToUpdate(FMM_CoreData *fmm_core_data);

/*!
 * FMM_Core_getGridPointStatusData() is an accessor function for 
 * the gridpoint_status data array managed by the FMM_CoreData structure.
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 * 
 * Return value:           pointer to gridpoint_status data array
 *
 */
int* FMM_Core_getGridPointStatusDataArray(FMM_CoreData *fmm_core_data);

/*!
 * FMM_Core_initializeHomotopicThinning() initializes the heap to contain
 * the boundary points of phi (which stores the distance transform)
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 * 
 * Return value:           none
 *
 */
void FMM_Core_initializeHomotopicThinning(FMM_CoreData *fmm_core_data,FMM_FieldData* &fmm_field_data,
                                          int* &grid_dims, FMM_Heap* &known_points);

void FMM_Core_setNextThinnedPoint(
    FMM_CoreData *fmm_core_data, 
    int *grid_idx, 
    double value);
    
#ifdef __cplusplus
}
#endif

#endif
