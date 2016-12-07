include_directories(${EIGEN_INCLUDE_DIRS} ../../lib_dasp_density/ ../../lib_dasp_slimage/ ../../lib_dasp_danvil/)

add_library(dasp_pds
	DeltaDensitySampling.cpp
	DeltaDensitySamplingOld.cpp
	Fattal.cpp
	FloydSteinberg.cpp
	FloydSteinbergMultiLayer.cpp
	Grid.cpp
	SimplifiedPDS.cpp
	SimplifiedPDSOld.cpp
)

target_link_libraries(dasp_pds dasp_density)

