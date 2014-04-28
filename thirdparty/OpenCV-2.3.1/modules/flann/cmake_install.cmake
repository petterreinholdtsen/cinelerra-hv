# Install script for directory: /root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
      FILE(RPATH_CHECK
           FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1"
           RPATH "/usr/local/lib")
    ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
      "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib/libopencv_flann.so.2.3.1"
      "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib/libopencv_flann.so.2.3"
      "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib/libopencv_flann.so"
      )
    IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
      FILE(RPATH_CHANGE
           FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1"
           OLD_RPATH "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib:"
           NEW_RPATH "/usr/local/lib")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
      FILE(RPATH_CHECK
           FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1"
           RPATH "/usr/local/lib")
    ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
      "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib/libopencv_flann.so.2.3.1"
      "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib/libopencv_flann.so.2.3"
      "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib/libopencv_flann.so"
      )
    IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
      FILE(RPATH_CHANGE
           FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1"
           OLD_RPATH "/root/hvirtual/thirdparty/OpenCV-2.3.1/lib:"
           NEW_RPATH "/usr/local/lib")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libopencv_flann.so.2.3.1")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/flann" TYPE FILE FILES
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/general.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/dist.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/kdtree_single_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/ground_truth.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/all_indices.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/hdf5.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/flann_base.hpp"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/timer.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/matrix.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/dynamic_bitset.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/lsh_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/hierarchical_clustering_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/autotuned_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/index_testing.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/simplex_downhill.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/saving.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/any.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/kdtree_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/composite_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/heap.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/allocator.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/lsh_table.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/miniflann.hpp"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/config.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/nn_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/dummy.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/params.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/random.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/flann.hpp"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/kmeans_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/defines.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/result_set.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/logger.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/object_factory.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/linear_index.h"
    "/root/hvirtual/thirdparty/OpenCV-2.3.1/modules/flann/include/opencv2/flann/sampling.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

