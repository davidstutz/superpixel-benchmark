cmake_minimum_required (VERSION 2.8)
project (superpixel_benchmark)
find_package(Threads)

add_library(vlslic
    generic.c
    host.c
    random.c
    mathop.c
    slic.c
)

target_link_libraries(vlslic Threads::Threads)
