include_directories(${EIGEN_INCLUDE_DIRS} ../../lib_dasp_danvil/ ../../lib_dasp_slimage ../../lib_dasp_common/)

add_library(dasp_density
	PointDensity.cpp
	ScalePyramid.cpp
	Smooth.cpp
	Visualization.cpp
)

target_link_libraries(dasp_density ${OpenCV_LIBS})
