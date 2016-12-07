cmake_minimum_required (VERSION 2.8)
project (superpixel_benchmark)

set(graphseg_DEFINES
#       -DSPECTRAL_VERBOSE
#       -DSEGS_DBG_PRINT
        -DUSE_USE_SOLVER_IETL)

add_definitions(${graphseg_DEFINES})

include_directories(${EIGEN_INCLUDE_DIRS} ../lib_dasp_ietl/)
ADD_LIBRARY(dasp_graphseg 
        graphseg.cpp
        Labeling.cpp
        # spectral/arpackpp.cpp
        # spectral/eigen.cpp
        # spectral/lapack.cpp
        spectral/ietl.cpp)

target_link_libraries(dasp_graphseg 
        # parpack
	# arpack
	# superlu
	lapack
	blas)