cmake_minimum_required (VERSION 2.8)
project (superpixel_benchmark)

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

include_directories(../lib_eval/
    ../lib_ers/
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)
add_executable(ers_cli main.cpp)
target_link_libraries(ers_cli
    eval
    ers
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
)
