# - Try to find CIMG
# Once done, this will define
#
#  CIMG_FOUND - system has CIMG
#  CIMG_INCLUDE_DIRS - the CIMG include directories
#

find_path(CImg_INCLUDE_DIR
  NAMES CImg.h
  PATHS ${CIMG_PKGCONF_INCLUDE_DIRS}
)

set(CIMG_PROCESS_INCLUDES CImg_INCLUDE_DIR)

find_library(x11_LIBRARY
    NAMES X11
)
find_library(pthread_LIBRARY
    NAMES pthread
)
set(CIMG_PROCESS_LIBS ${x11_LIBRARY} ${pthread_LIBRARY})

set(CIMG_LIBRARIES ${CIMG_PROCESS_LIBS})
set(CIMG_INCLUDE_DIRS ${CImg_INCLUDE_DIR})
set(CImg_LIBRARIES ${CIMG_PROCESS_LIBS})
set(CImg_INCLUDE_DIRS ${CImg_INCLUDE_DIR})