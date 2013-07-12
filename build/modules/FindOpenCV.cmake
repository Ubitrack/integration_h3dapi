# Module for locating OpenCV.
#
# Customizable variables:
#   OPENCV_ROOT_DIR
#     Specifies the OpenCV root directory.
#
#   OPENCV_USE_GPU_LIBS
#     Indicates whether GPU enabled libraries should be located.
#
#   OPENCV_USE_STATIC_LIBS
#     Indicates whether static instead of shared libraries should be located.
#
#   OPENCV_COMPILER
#     The compiler name used to locate the libraries. Setting this variable is
#     useful only if the OpenCV superpack is being used with its default
#     directory layout.
#
# Read-only variables:
#   OPENCV_FOUND
#     Indicates whether the library has been found.
#
#   OPENCV_INCLUDE_DIRS
#     Specifies to the OpenCV include directory.
#
#   OPENCV_<COMPONENT>_FOUND
#     Indicates whether the specified <COMPONENT> was found.
#
#   OPENCV_LIBRARIES
#     Specifies the OpenCV libraries that should be passed to
#     target_link_libararies.
#
#
# Copyright (c) 2013 Sergiu Dotenco
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTOPENCVLAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

INCLUDE (FindPackageHandleStandardArgs)

IF (CMAKE_VERSION VERSION_GREATER 2.8.7)
  SET (_OPENCV_CHECK_COMPONENTS FALSE)
ELSE (CMAKE_VERSION VERSION_GREATER 2.8.7)
  SET (_OPENCV_CHECK_COMPONENTS TRUE)
ENDIF (CMAKE_VERSION VERSION_GREATER 2.8.7)

IF(NOT DEFINED OPENCV_ROOT_DIR)
	FIND_PATH (OPENCV_ROOT_DIR
	  NAMES OpenCVConfig.cmake include/opencv/cv.h include/cv/cv.h
			include/opencv2/opencv.hpp
	  PATHS ENV OPENCVROOT
	  DOC "OpenCV root directory")
ENDIF(NOT DEFINED OPENCV_ROOT_DIR)

IF (OPENCV_USE_STATIC_LIBS)
  SET (_OPENCV_LIB_DIR staticlib)
ELSE (OPENCV_USE_STATIC_LIBS)
  SET (_OPENCV_LIB_DIR lib)
ENDIF (OPENCV_USE_STATIC_LIBS)

SET (_OPENCV_POSSIBLE_LIB_SUFFIXES ${_OPENCV_LIB_DIR})

IF (DEFINED OPENCV_COMPILER)
  SET (_OPENCV_COMPILER ${OPENCV_COMPILER})
ELSEIF (MSVC11)
  SET (_OPENCV_COMPILER vc11)
ELSEIF (MSVC10)
  SET (_OPENCV_COMPILER vc10)
ELSEIF (MSVC90)
  SET (_OPENCV_COMPILER vc9)
ELSEIF (MSVC80)
  SET (_OPENCV_COMPILER vc8)
ELSEIF (MSVC71)
  SET (_OPENCV_COMPILER vc71)
ELSEIF (MSVC70)
  SET (_OPENCV_COMPILER vc7)
ELSEIF (MSVC60)
  SET (_OPENCV_COMPILER vc6)
ELSEIF (MINGW)
  SET (_OPENCV_COMPILER mingw)
ENDIF (DEFINED OPENCV_COMPILER)

IF (OPENCV_USE_GPU_LIBS)
  SET (_OPENCV_BUILD_DIR build/gpu)
ELSE (OPENCV_USE_GPU_LIBS)
  SET (_OPENCV_BUILD_DIR build)
ENDIF (OPENCV_USE_GPU_LIBS)

IF (DEFINED _OPENCV_COMPILER)
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET (_OPENCV_ARCHITECTURE x64)
  ELSEIF (CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET (_OPENCV_ARCHITECTURE x86)
  ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 8)

  IF (OPENCV_USE_GPU_LIBS)
    LIST (APPEND _OPENCV_LIB_HINTS
      ${OPENCV_ROOT_DIR}/${_OPENCV_BUILD_DIR}/${_OPENCV_ARCHITECTURE}/${_OPENCV_LIB_DIR})
  ELSE (OPENCV_USE_GPU_LIBS)
    LIST (APPEND _OPENCV_LIB_HINTS
      ${OPENCV_ROOT_DIR}/${_OPENCV_BUILD_DIR}/${_OPENCV_ARCHITECTURE}/${_OPENCV_COMPILER}/${_OPENCV_LIB_DIR})
  ENDIF (OPENCV_USE_GPU_LIBS)
ENDIF (DEFINED _OPENCV_COMPILER)

LIST (APPEND _OPENCV_LIB_HINTS ${OPENCV_ROOT_DIR})

FIND_PATH (OPENCV_INCLUDE_DIR
  NAMES opencv/cv.h opencv2/opencv.hpp
  HINTS ${OPENCV_ROOT_DIR}/build ${OPENCV_ROOT_DIR}
  PATH_SUFFIXES include
  DOC "OpenCV include directory")

IF (OPENCV_ROOT_DIR)
  FIND_PACKAGE (OpenCV NO_MODULE QUIET
    HINTS ${OPENCV_ROOT_DIR}/build ${OPENCV_ROOT_DIR}
    NO_CMAKE_BUILDS_PATH)

  MARK_AS_ADVANCED (OpenCV_DIR)

  IF (OpenCV_FOUND)
    SET (OPENCV_VERSION_MAJOR ${OpenCV_VERSION_MAJOR})
    SET (OPENCV_VERSION_MINOR ${OpenCV_VERSION_MINOR})
    SET (OPENCV_VERSION_PATCH ${OpenCV_VERSION_PATCH})
    SET (OPENCV_VERSION ${OpenCV_VERSION_MAJOR}.${OpenCV_VERSION_MINOR})

    IF (OpenCV_VERSION_PATCH GREATER 0)
      SET (OPENCV_VERSION ${OPENCV_VERSION}.${OpenCV_VERSION_PATCH})
    ENDIF (OpenCV_VERSION_PATCH GREATER 0)

    SET (OPENCV_VERSION_COUNT 3)

    # Search for a specific version
    SET (_OPENCV_PREFERRED_SUFFIX
      ${OPENCV_VERSION_MAJOR}${OPENCV_VERSION_MINOR}${OPENCV_VERSION_PATCH})
    # Otherwise it try to guess it.
  ELSE (OpenCV_FOUND)
    FIND_FILE (OPENCV_VERSION_HEADER NAMES cvver.h version.hpp HINTS
      ${OPENCV_INCLUDE_DIR} PATH_SUFFIXES opencv opencv2/core)

    IF (OPENCV_VERSION_HEADER)
      SET (OPENCV_VERSION_HEADER ${OPENCV_VERSION_HEADER} CACHE INTERNAL
        "OpenCV version header" FORCE)

      # Find OpenCV version by looking at cvver.h or version.hpp
      FILE (STRINGS ${OPENCV_VERSION_HEADER}
        _OPENCV_VERSION_STRING REGEX "^#define[ \t]+CV_(([A-Z]+_VERSION)|VERSION_[A-Z]+)[ \t]+[0-9]+$")

      STRING (REGEX REPLACE ".*#define[ \t]+(CV_VERSION_EPOCH|CV_MAJOR_VERSION)[ \t]+([0-9]+).*" "\\2"
        OPENCV_VERSION_MAJOR ${_OPENCV_VERSION_STRING})
      STRING (REGEX REPLACE ".*#define[ \t]+(CV_VERSION_MAJOR|CV_MINOR_VERSION)[ \t]+([0-9]+).*" "\\2"
        OPENCV_VERSION_MINOR ${_OPENCV_VERSION_STRING})
      STRING (REGEX REPLACE ".*#define[ \t]+(CV_VERSION_MINOR|CV_SUBMINOR_VERSION)[ \t]+([0-9]+).*"
        "\\2" OPENCV_VERSION_PATCH ${_OPENCV_VERSION_STRING})
      STRING (REGEX REPLACE ".*#define[ \t]+(CV_VERSION_REVISION)[ \t]+([0-9]+).*"
        "\\2" OPENCV_VERSION_TWEAK ${_OPENCV_VERSION_STRING})

      SET (_OPENCV_PREFERRED_SUFFIX
        ${OPENCV_VERSION_MAJOR}${OPENCV_VERSION_MINOR}${OPENCV_VERSION_PATCH})

      SET (OPENCV_VERSION
        ${OPENCV_VERSION_MAJOR}.${OPENCV_VERSION_MINOR})

      IF (OPENCV_VERSION_TWEAK STREQUAL "")
        SET (OPENCV_VERSION_TWEAK 0)
      ENDIF (OPENCV_VERSION_TWEAK STREQUAL "")

      IF (OPENCV_VERSION_PATCH GREATER 0 OR OPENCV_VERSION_TWEAK GREATER 0)
        SET (OPENCV_VERSION ${OPENCV_VERSION}.${OPENCV_VERSION_PATCH})
      ENDIF (OPENCV_VERSION_PATCH GREATER 0 OR OPENCV_VERSION_TWEAK GREATER 0)

      IF (OPENCV_VERSION_TWEAK GREATER 0)
        SET (OPENCV_VERSION ${OPENCV_VERSION}.${OPENCV_VERSION_TWEAK})
      ENDIF (OPENCV_VERSION_TWEAK GREATER 0)

      SET (OPENCV_VERSION_COUNT 4)
    ENDIF (OPENCV_VERSION_HEADER)
  ENDIF (OpenCV_FOUND)

  SET (_OPENCV_DETECTED_SUFFIX
    ${OPENCV_VERSION_MAJOR}${OPENCV_VERSION_MINOR}${OPENCV_VERSION_PATCH})

  IF (NOT OpenCV_FIND_COMPONENTS)
    IF (OPENCV_VERSION VERSION_GREATER 2.1)
      SET (OpenCV_FIND_COMPONENTS calib3d contrib core features2d flann highgui
        imgproc legacy ml objdetect video)
    ELSE (OPENCV_VERSION VERSION_GREATER 2.1)
      SET (OpenCV_FIND_COMPONENTS cxcore cv ml highgui cvaux)
    ENDIF (OPENCV_VERSION VERSION_GREATER 2.1)
  ENDIF (NOT OpenCV_FIND_COMPONENTS)

  # Loop over each components
  FOREACH (_OPENCV_COMPONENT ${OpenCV_FIND_COMPONENTS})
    SET (_OPENCV_COMPONENT_BASE opencv_${_OPENCV_COMPONENT})

    SET (_OPENCV_COMPONENT_POSSIBLE_DEBUG_NAMES
         ${_OPENCV_COMPONENT_BASE}d
         ${_OPENCV_COMPONENT_BASE}${_OPENCV_DETECTED_SUFFIX}d
         ${_OPENCV_COMPONENT_BASE}d
         ${_OPENCV_COMPONENT_BASE}${_OPENCV_DETECTED_SUFFIX}d)
    SET (_OPENCV_COMPONENT_POSSIBLE_RELEASE_NAMES
         ${_OPENCV_COMPONENT_BASE}
         ${_OPENCV_COMPONENT_BASE}${_OPENCV_DETECTED_SUFFIX}
         ${_OPENCV_COMPONENT_BASE}
         ${_OPENCV_COMPONENT_BASE}${_OPENCV_DETECTED_SUFFIX})

    STRING (TOUPPER ${_OPENCV_COMPONENT} _OPENCV_COMPONENT_UPPER)
    SET (_OPENCV_LIBRARY_BASE OPENCV_${_OPENCV_COMPONENT_UPPER}_LIBRARY)

    FIND_LIBRARY (${_OPENCV_LIBRARY_BASE}_DEBUG
      NAMES ${_OPENCV_COMPONENT_POSSIBLE_DEBUG_NAMES}
      HINTS ${_OPENCV_LIB_HINTS} "${OPENCV_ROOT_DIR}/lib_debug"
      PATH_SUFFIXES ${_OPENCV_POSSIBLE_LIB_SUFFIXES}
      DOC "OpenCV ${_OPENCV_COMPONENT} debug library")

    FIND_LIBRARY (${_OPENCV_LIBRARY_BASE}_RELEASE
      NAMES ${_OPENCV_COMPONENT_POSSIBLE_RELEASE_NAMES}
      HINTS ${_OPENCV_LIB_HINTS} "${OPENCV_ROOT_DIR}/lib"
      PATH_SUFFIXES ${_OPENCV_POSSIBLE_LIB_SUFFIXES}
      DOC "OpenCV ${_OPENCV_COMPONENT} release library")

    IF (${_OPENCV_LIBRARY_BASE}_DEBUG)
      LIST (APPEND _OPENCV_ALL_LIBS ${${_OPENCV_LIBRARY_BASE}_DEBUG})
    ENDIF (${_OPENCV_LIBRARY_BASE}_DEBUG)

    IF (${_OPENCV_LIBRARY_BASE}_RELEASE)
      LIST (APPEND _OPENCV_ALL_LIBS ${${_OPENCV_LIBRARY_BASE}_RELEASE})
    ENDIF (${_OPENCV_LIBRARY_BASE}_RELEASE)

    SET (OPENCV_${_OPENCV_COMPONENT_UPPER}_FOUND TRUE)

    # Debug and release
    IF (${_OPENCV_LIBRARY_BASE}_DEBUG AND ${_OPENCV_LIBRARY_BASE}_RELEASE)
      SET (${_OPENCV_LIBRARY_BASE}
        #optimized ${${_OPENCV_LIBRARY_BASE}_RELEASE}
		${${_OPENCV_LIBRARY_BASE}_RELEASE}
		debug ${${_OPENCV_LIBRARY_BASE}_DEBUG}
        CACHE DOC
        "OpenCV ${_OPENCV_COMPONENT} library")
      # Debug only
    ELSEIF (${_OPENCV_LIBRARY_BASE}_DEBUG)
      SET (${_OPENCV_LIBRARY_BASE} ${${_OPENCV_LIBRARY_BASE}_DEBUG} CACHE DOC
        "OpenCV ${_OPENCV_COMPONENT} library")
      # Release only
    ELSEIF (${_OPENCV_LIBRARY_BASE}_RELEASE)
      SET (${_OPENCV_LIBRARY_BASE} ${${_OPENCV_LIBRARY_BASE}_RELEASE} CACHE DOC
        "OpenCV ${_OPENCV_COMPONENT} library")
    ELSE (${_OPENCV_LIBRARY_BASE}_DEBUG AND ${_OPENCV_LIBRARY_BASE}_RELEASE)
      # Component missing: record it for a later report
      LIST (APPEND _OPENCV_MISSING_COMPONENTS ${_OPENCV_COMPONENT})
      LIST (APPEND _OPENCV_MISSING_LIBRARIES ${_OPENCV_LIBRARY_BASE})
      SET (OPENCV_${_OPENCV_COMPONENT_UPPER}_FOUND FALSE)
    ENDIF (${_OPENCV_LIBRARY_BASE}_DEBUG AND ${_OPENCV_LIBRARY_BASE}_RELEASE)

    MARK_AS_ADVANCED (${_OPENCV_LIBRARY_BASE} ${_OPENCV_LIBRARY_BASE}_DEBUG
      ${_OPENCV_LIBRARY_BASE}_RELEASE)

    SET (OpenCV_${_OPENCV_COMPONENT}_FOUND
      ${OPENCV_${_OPENCV_COMPONENT_UPPER}_FOUND})

    # Make sure only libraries that have been actually found are registered
    IF (${_OPENCV_LIBRARY_BASE})
      LIST (APPEND _OPENCV_LIBRARIES ${${_OPENCV_LIBRARY_BASE}})
    ENDIF (${_OPENCV_LIBRARY_BASE})
  ENDFOREACH (_OPENCV_COMPONENT)

  IF (NOT DEFINED _OPENCV_MISSING_COMPONENTS)
    # Success: all components were found
    LIST (APPEND OPENCV_LIBRARIES ${_OPENCV_LIBRARIES})
  ELSE (NOT DEFINED _OPENCV_MISSING_COMPONENTS AND _OPENCV_CHECK_COMPONENTS)
    IF (NOT OpenCV_FIND_QUIETLY)
      MESSAGE (STATUS "One or more OpenCV components were not found:")
      # Display missing components indented, each on a separate line
      FOREACH (_OPENCV_MISSING_COMPONENT ${_OPENCV_MISSING_COMPONENTS})
        MESSAGE (STATUS "  " ${_OPENCV_MISSING_COMPONENT})
      ENDFOREACH (_OPENCV_MISSING_COMPONENT ${_OPENCV_MISSING_COMPONENTS})
    ENDIF (NOT OpenCV_FIND_QUIETLY)
  ENDIF (NOT DEFINED _OPENCV_MISSING_COMPONENTS)

  SET (OPENCV_INCLUDE_DIRS ${OPENCV_INCLUDE_DIR})
ENDIF (OPENCV_ROOT_DIR)

IF (WIN32)
  FIND_PROGRAM (LIB_EXECUTABLE NAMES lib
    HINTS "$ENV{VS110COMNTOOLS}/../../VC/bin"
          "$ENV{VS100COMNTOOLS}/../../VC/bin"
          "$ENV{VS90COMNTOOLS}/../../VC/bin"
          "$ENV{VS71COMNTOOLS}/../../VC/bin"
          "$ENV{VS80COMNTOOLS}/../../VC/bin"
    DOC "Library manager")

  MARK_AS_ADVANCED (LIB_EXECUTABLE)
ENDIF (WIN32)

MACRO (GET_LIB_REQUISITES LIB REQUISITES)
  IF (LIB_EXECUTABLE)
    GET_FILENAME_COMPONENT (_LIB_PATH ${LIB_EXECUTABLE} PATH)

    IF (MSVC)
      # Do not redirect the output
      UNSET (ENV{VS_UNICODE_OUTPUT})
    ENDIF (MSVC)

    EXECUTE_PROCESS (COMMAND ${LIB_EXECUTABLE} /nologo /list ${LIB}
      WORKING_DIRECTORY ${_LIB_PATH}/../../Common7/IDE
      OUTPUT_VARIABLE _LIB_OUTPUT ERROR_QUIET)

    STRING (REPLACE "\n" ";" "${REQUISITES}" "${_LIB_OUTPUT}")
    LIST (REMOVE_DUPLICATES ${REQUISITES})
  ENDIF (LIB_EXECUTABLE)
ENDMACRO (GET_LIB_REQUISITES)

IF (_OPENCV_ALL_LIBS)
  # collect lib requisites using the lib tool
  FOREACH (_OPENCV_COMPONENT ${_OPENCV_ALL_LIBS})
    GET_LIB_REQUISITES (${_OPENCV_COMPONENT} _OPENCV_REQUISITES)
  ENDFOREACH (_OPENCV_COMPONENT)
ENDIF (_OPENCV_ALL_LIBS)

IF (NOT OPENCV_BINARY_DIR)
  SET (_OPENCV_UPDATE_BINARY_DIR TRUE)
ELSE (NOT OPENCV_BINARY_DIR)
  SET (_OPENCV_UPDATE_BINARY_DIR FALSE)
ENDIF (NOT OPENCV_BINARY_DIR)

SET (_OPENCV_BINARY_DIR_HINTS ${OPENCV_ROOT_DIR}/bin)

IF (_OPENCV_ALL_LIBS)
  LIST (GET _OPENCV_ALL_LIBS 0 _OPENCV_LIB)
  # extract the path to the lib directory
  GET_FILENAME_COMPONENT (_OPENCV_LIB ${_OPENCV_LIB} PATH)
  # extract the path to lib's parent directory
  GET_FILENAME_COMPONENT (_OPENCV_COMMON_DIR ${_OPENCV_LIB}/.. ABSOLUTE)

  LIST (APPEND _OPENCV_BINARY_DIR_HINTS ${_OPENCV_COMMON_DIR}/bin)
ENDIF (_OPENCV_ALL_LIBS)

FIND_PROGRAM (OPENCV_BINARY_DIR
  NAMES opencv_haartraining opencv_performance opencv_stitching
        opencv_createsamples
  HINTS ${_OPENCV_BINARY_DIR_HINTS} NO_DEFAULT_PATH)

IF (_OPENCV_REQUISITES)
  FIND_FILE (OPENCV_BINARY_DIR NAMES ${_OPENCV_REQUISITES}
    HINTS ${_OPENCV_BINARY_DIR_HINTS} NO_DEFAULT_PATH)
ENDIF (_OPENCV_REQUISITES)

IF (OPENCV_BINARY_DIR AND _OPENCV_UPDATE_BINARY_DIR)
  SET (_OPENCV_BINARY_DIR ${OPENCV_BINARY_DIR})
  UNSET (OPENCV_BINARY_DIR CACHE)

  GET_FILENAME_COMPONENT (OPENCV_BINARY_DIR ${_OPENCV_BINARY_DIR} PATH)
ENDIF (OPENCV_BINARY_DIR AND _OPENCV_UPDATE_BINARY_DIR)

SET (OPENCV_BINARY_DIR ${OPENCV_BINARY_DIR} CACHE PATH
  "OpenCV binary directory")

MARK_AS_ADVANCED (OPENCV_BINARY_DIR OPENCV_INCLUDE_DIR)

IF (NOT _OPENCV_CHECK_COMPONENTS)
  SET (_OPENCV_FPHSA_ADDITIONAL_ARGS HANDLE_COMPONENTS)
ENDIF (NOT _OPENCV_CHECK_COMPONENTS)

FIND_PACKAGE_HANDLE_STANDARD_ARGS (OpenCV REQUIRED_VARS OPENCV_ROOT_DIR
  OPENCV_INCLUDE_DIR ${_OPENCV_MISSING_LIBRARIES} VERSION_VAR OPENCV_VERSION
  ${_OPENCV_FPHSA_ADDITIONAL_ARGS})