# - Try to find Glog
#
# The following variables are optionally searched for defaults
#  GLOG_ROOT_DIR:            Base directory where all GLOG components are found
#
# The following are set after configuration is done: 
#  GLOG_FOUND
#  GLOG_INCLUDE_DIRS
#  GLOG_LIBRARIES

include(FindPackageHandleStandardArgs)

set(GLOG_ROOT_DIR "" CACHE PATH "Folder contains Google glog")

find_path(GLOG_INCLUDE_DIR glog/logging.h
    PATHS
        ${GLOG_ROOT_DIR}
        ~/glog-0.3.3
    PATH_SUFFIXES
        src)

find_library(GLOG_LIBRARY glog libglog
    PATHS
        ${GLOG_ROOT_DIR}
        ~/glog-0.3.3
    PATH_SUFFIXES
        .libs
        lib
        lib64)

find_package_handle_standard_args(GLOG DEFAULT_MSG
    GLOG_INCLUDE_DIR GLOG_LIBRARY)

if(GLOG_FOUND)
    set(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
    set(GLOG_LIBRARIES ${GLOG_LIBRARY})
endif()
