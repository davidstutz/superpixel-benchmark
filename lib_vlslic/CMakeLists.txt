cmake_minimum_required (VERSION 2.8)
project (superpixel_benchmark)

add_library(vlslic
    generic.c
    host.c
    random.c
    mathop.c
    slic.c
)