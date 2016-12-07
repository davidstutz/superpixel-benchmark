/*
 * File:        lsm_fast_marching_method.h
 * Copyright:   (c) 2005-2006 Kevin T. Chu
 * Revision:    $Revision: 1.10 $
 * Modified:    $Date: 2006/08/14 20:18:55 $
 * Description: Header file for 2D and 3D Fast Marching Method Algorithms
 */
 
#ifndef included_fast_marching_method_h
#define included_fast_marching_method_h

#ifdef __cplusplus
extern "C" {
#endif

/*! \file lsm_fast_marching_method.h
 * 
 * \brief 
 * @ref lsm_fast_marching_method.h provides support for basic fast 
 * marching method calculations:  computing distance functions, 
 * extensions of field variables (e.g. extension velocities, etc.) 
 * off of the zero contour of a level set function, and solving the
 * Eikonal equation.
 *
 * The algorithm (and naming) closely follows the description in 
 * "Level Set Methods and Fast Marching Methods" by J.A. Sethian
 * and "The Fast Construction of Extension Velocities in Level Set
 * Methods" by D. Adalsteinsson and J.A. Sethian (J. Comp. Phys, 
 * vol 148, p 2-22, 1999).
 * 
 * 
 * <h3> NOTES </h3>
 * - The fast marching method library assumes that the field data
 *   are stored in Fortran order (i.e. column-major order).
 *
 * - Error Codes:  0 - successful computation,
 *                 1 - FMM_Data creation error,
 *                 2 - invalid spatial derivative order
 *
 * - While @ref lsm_fast_marching_method.h only provides functions 
 *   for 2D and 3D FMM calculations, LSMLIB is capable of supporting higher 
 *   dimensional calculations (currently as high as 8, set by 
 *   FMM_HEAP_MAX_NDIM in @ref FMM_Heap.h).  To use LSMLIB to do 
 *   higher dimensional fast marching method calculations, just modify 
 *   lsm_FMM_field_extension*d.c and/or lsm_FMM_eikonal*d.c so that
 *   the data array sizes and index calculations are appropriate
 *   for the dimensionality of the problem of interest.
 *
 */


/*!
 * computeExtensionFields2d uses the FMM algorithm to compute the 
 * distance function and extension fields from the original level set
 * function, phi, and the specified source fields.  
 *
 * Arguments:
 *  - distance_function (out):        updated distance function
 *  - extension_fields (out):         extension fields
 *  - phi (in):                       original level set function
 *  - mask (in):                      mask for domain of problem;
 *                                    grid points outside of the domain
 *                                    of the problem should be set to a 
 *                                    negative value.  
 *  - source_fields(in):              source fields used to compute extension 
 *                                    fields
 *  - num_extension_fields (in):      number of extension fields to compute
 *  - spatial_derivative_order (in):  order of finite differences used 
 *                                    to compute spatial derivatives
 *  - grid_dims (in):                 array of index space extents for all 
 *                                    fields 
 *  - dx (in):                        array of grid cell sizes in each 
 *                                    coordinate direction
 *
 * Return value:                      error code (see NOTES for translation)
 *
 *
 * NOTES:
 *  - For grid points that are masked out, the distance function and
 *    extension fields are set to 0.
 *
 *  - It is assumed that the user has allocated the memory for the
 *    distance function, extension fields, phi, and source fields.
 *
 *  - All data fields are assumed to have the same index space extents.
 *
 *  - If mask is set to a NULL pointer, then all grid points are treated
 *    as being in the interior of the domain.
 *
 *  - The number of extension fields is assumed to be equal to the
 *    number of source fields.
 *
 */
int computeExtensionFields2d(
  double *distance_function,
  double **extension_fields,
  double *phi,
  double *mark,
  double **source_fields,
  int num_extension_fields,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx);

int computeExtensionFields2d_WithMaxVal(
  double *distance_function,
  double **extension_fields,
  double *phi,
  double *mark,
  double **source_fields,
  int num_extension_fields,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx,
  double max_value);

/*!
 * computeDistanceFunction2d uses the FMM algorithm to compute the 
 * a distance function from the original level set function, phi.
 *
 * Arguments:
 *  - distance_function (out):        updated distance function
 *  - phi (in):                       original level set function
 *  - mask (in):                      mask for domain of problem;
 *                                    grid points outside of the domain
 *                                    of the problem should be set to a 
 *                                    negative value.
 *  - spatial_derivative_order (in):  order of finite differences used 
 *                                    to compute spatial derivatives
 *  - grid_dims (in):                 array of index space extents for all 
 *                                    fields 
 *  - dx (in):                        array of grid cell sizes in each 
 *                                    coordinate direction
 *
 * Return value:                      error code (see NOTES for translation)
 *
 *
 * NOTES:
 *  - For grid points that are masked out, the distance function is
 *    set to 0.
 *
 *  - It is assumed that the user has allocated the memory for the
 *    distance function and phi fields.
 *
 *  - If mask is set to a NULL pointer, then all grid points are treated
 *    as being in the interior of the domain.
 *
 *  - All data fields are assumed to have the same index space extents.
 *
 */
int computeDistanceFunction2d(
  double *distance_function,
  double *phi,
  double *mark,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx);

/*!
 * solveEikonalEquation2d uses the FMM algorithm to solve the Eikonal
 * equation 
 *
 *   |grad(phi)| = 1/speed(x,y)
 *
 * in two space dimensions with the specified boundary data 
 * and speed function. 
 *
 * Arguments:
 *  - phi (in):                       pointer to solution to Eikonal equation;
 *                                    phi must be initialized as specified in
 *                                    the NOTES below. 
 *  - speed (in):                     pointer to speed field
 *  - mask (in):                      mask for domain of problem;
 *                                    grid points outside of the domain
 *                                    of the problem should be set to a 
 *                                    negative value.
 *  - spatial_derivative_order (in):  order of finite differences used 
 *                                    to compute spatial derivatives
 *  - grid_dims (in):                 array of index space extents for all 
 *                                    fields 
 *  - dx (in):                        array of grid cell sizes in each 
 *                                    coordinate direction
 *
 * Return value:                      error code (see NOTES for translation)
 *
 * 
 * NOTES:
 *  - For grid points that are masked out or have speed equal to zero, phi 
 *    is set to DBL_MAX.
 *
 *  - Both phi and the speed function is assumed to be strictly non-negative.
 *
 *  - phi MUST be initialized so that the values for phi at grid points on 
 *    or adjacent to the boundary of the domain for the Eikonal equation 
 *    are correctly set.  All other grid points should be set to have
 *    negative values for phi.
 *
 *  - It is the user's responsibility to set the speed function.
 *
 *  - If mask is set to a NULL pointer, then all grid points are treated
 *    as being in the interior of the domain.
 *
 *  - All data fields are assumed to have the same index space extents.
 *
 */
int solveEikonalEquation2d(
  double *phi,
  double *speed,
  double *mask,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx);

/*!
 * computeExtensionFields3d uses the FMM algorithm to compute the 
 * distance function and extension fields from the original level set
 * function, phi, and the specified source fields.  
 *
 * Arguments:
 *  - distance_function (out):        updated distance function
 *  - extension_fields (out):         extension fields
 *  - phi (in):                       original level set function
 *  - mask (in):                      mask for domain of problem;
 *                                    grid points outside of the domain
 *                                    of the problem should be set to a 
 *                                    negative value.
 *  - source_fields(in):              source fields used to compute extension 
 *                                    fields
 *  - num_extension_fields (in):      number of extension fields to compute
 *  - spatial_derivative_order (in):  order of finite differences used 
 *                                    to compute spatial derivatives
 *  - grid_dims (in):                 array of index space extents for all 
 *                                    fields 
 *  - dx (in):                        array of grid cell sizes in each 
 *                                    coordinate direction
 *
 * Return value:                      error code (see NOTES for translation)
 *
 *
 * NOTES:
 *  - For grid points that are masked out, the distance function and
 *    extension fields are set to 0.
 *
 *  - It is assumed that the user has allocated the memory for the
 *    distance function, extension fields, phi, and source fields.
 *
 *  - All data fields are assumed to have the same index space extents.
 *
 *  - If mask is set to a NULL pointer, then all grid points are treated
 *    as being in the interior of the domain.
 *
 *  - The number of extension fields is assumed to be equal to the
 *    number of source fields.
 *
 */
int computeExtensionFields3d(
  double *distance_function,
  double **extension_fields,
  double *phi,
  double *mask,
  double **source_fields,
  int num_extension_fields,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx);

/*!
 * computeDistanceFunction3d uses the FMM algorithm to compute the 
 * a distance function from the original level set function, phi.
 *
 * Arguments:
 *  - distance_function (out):        updated distance function
 *  - phi (in):                       original level set function
 *  - mask (in):                      mask for domain of problem;
 *                                    grid points outside of the domain
 *                                    of the problem should be set to a 
 *                                    negative value.
 *  - spatial_derivative_order (in):  order of finite differences used 
 *                                    to compute spatial derivatives
 *  - grid_dims (in):                 array of index space extents for all 
 *                                    fields 
 *  - dx (in):                        array of grid cell sizes in each 
 *                                    coordinate direction
 *
 * Return value:                      error code (see NOTES for translation)
 *
 *
 * NOTES:
 *  - For grid points that are masked out, the distance function is
 *    set to 0.
 *
 *  - It is assumed that the user has allocated the memory for the
 *    distance function and phi fields.
 *
 *  - If mask is set to a NULL pointer, then all grid points are treated
 *    as being in the interior of the domain.
 *
 *  - All data fields are assumed to have the same index space extents.
 *
 */
int computeDistanceFunction3d(
  double *distance_function,
  double *phi,
  double *mask,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx);

/*!
 * solveEikonalEquation3d uses the FMM algorithm to solve the Eikonal
 * equation 
 *
 *   |grad(phi)| = 1/speed(x,y,z)
 *
 * in three space dimensions with the specified boundary data 
 * and speed function. 
 *
 * Arguments:
 *  - phi (in/out):                   pointer to solution to Eikonal equation;
 *                                    phi must be initialized as specified in
 *                                    the NOTES below. 
 *  - speed (in):                     pointer to speed field
 *  - mask (in):                      mask for domain of problem;
 *                                    grid points outside of the domain
 *                                    of the problem should be set to a 
 *                                    negative value.
 *  - spatial_derivative_order (in):  order of finite differences used 
 *                                    to compute spatial derivatives
 *  - grid_dims (in):                 array of index space extents for all 
 *                                    fields 
 *  - dx (in):                        array of grid cell sizes in each 
 *                                    coordinate direction
 *
 * Return value:                      error code (see NOTES for translation)
 *
 *
 * NOTES:
 *  - For grid points that are masked out or have speed equal to zero, phi 
 *    is set to DBL_MAX.
 *
 *  - Both phi and the speed function is assumed to be strictly non-negative.
 *
 *  - phi MUST be initialized so that the values for phi at grid points on 
 *    or adjacent to the boundary of the domain for the Eikonal equation 
 *    are correctly set.  All other grid points should be set to have
 *    negative values for phi.
 *
 *  - It is the user's responsibility to set the speed function.
 *
 *  - If mask is set to a NULL pointer, then all grid points are treated
 *    as being in the interior of the domain.
 *
 *  - All data fields are assumed to have the same index space extents.
 *
 */
int solveEikonalEquation3d(
  double *phi,
  double *speed,
  double *mask,
  int spatial_derivative_order,
  int *grid_dims,
  double *dx);

/*!
 * FMM_initializeHomotopicThinning() initializes the heap to contain
 * the boundary points of phi (which stores the distance transform)
 *
 * Arguments:
 *  - fmm_core_data (in):  FMM_CoreData "object" actively managing the 
 *                         FMM computation
 * 
 * Return value:           none
 *
 */
// void FMM_initializeHomotopicThinning(FMM_CoreData *fmm_core_data);

int doHomotopicThinning(
  double *thinned_img,
  double *phi,
  double *background,
  int *grid_dims);

bool isSimplePoint(int* grid_point_status, double* background, int* grid_idx, int neighbors[][3], 
                   int* grid_dims);

#ifdef __cplusplus
}
#endif

#endif
