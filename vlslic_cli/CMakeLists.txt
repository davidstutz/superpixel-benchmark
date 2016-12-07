cmake_minimum_required (VERSION 2.8)
project (superpixel_benchmark)

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

include_directories(../lib_eval/ ../lib_vlslic/ ${OpenCV_INCLUDE_DIRS} 
        ${Boost_INCLUDE_DIRS})
add_executable(vlslic_cli main.cpp)
target_link_libraries(vlslic_cli vlslic eval ${Boost_LIBRARIES} 
        ${OpenCV_LIBS})
