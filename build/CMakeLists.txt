IF(WIN32)
cmake_minimum_required(VERSION 2.6.0)
ENDIF(WIN32)
PROJECT(H3DCV)

SET( H3DCV_FULL_VERSION 0.0 ) 


# Set here already in case a special NSIS template needs to be used by cpack.
SET( CONVERTED_H3D_ROOT "" )
IF( EXISTS $ENV{H3D_ROOT} )
  FILE( TO_CMAKE_PATH $ENV{H3D_ROOT} CONVERTED_H3D_ROOT )
ENDIF( EXISTS $ENV{H3D_ROOT} )
SET(CMAKE_MODULE_PATH  ${H3DCV_SOURCE_DIR}/modules ${CONVERTED_H3D_ROOT}/build/modules ${H3DCV_SOURCE_DIR}/../../H3DAPI/build/modules )
INCLUDE( StripAndAddLibraryDirectories )


IF(WIN32) 
    SET(CMAKE_C_FLAGS "ws2_32") 
ENDIF(WIN32) 


#add all sources
AUX_SOURCE_DIRECTORY(../src H3DCV_SRCS)

# add all optional libraries to this variable
SET(optionalLibs)

# add all required libraries to this variable
SET(requiredLibs)

INCLUDE_DIRECTORIES( ../include )

IF(WIN32)
  SET(requiredLibs ${requiredLibs} )
ENDIF(WIN32)

#H3DUtil
FIND_PACKAGE(H3DUtil REQUIRED)

IF(H3DUTIL_FOUND)
  INCLUDE_DIRECTORIES( ${H3DUTIL_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${H3DUTIL_LIBRARIES} )
ENDIF(H3DUTIL_FOUND)

#HAPI
FIND_PACKAGE(HAPI REQUIRED)

IF(HAPI_FOUND)
  INCLUDE_DIRECTORIES( ${HAPI_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${HAPI_LIBRARIES} )
ENDIF(HAPI_FOUND)

#H3DAPI
FIND_PACKAGE(H3DAPI REQUIRED)

IF(H3DAPI_FOUND)
  INCLUDE_DIRECTORIES( ${H3DAPI_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${H3DAPI_LIBRARIES} )
ENDIF(H3DAPI_FOUND)

#OpenCV 
FIND_PACKAGE(OpenCV REQUIRED COMPONENTS CORE IMGPROC HIGHGUI OBJDETECT)

IF(OpenCV_FOUND)
  INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${OpenCV_LIBRARIES} )
  MESSAGE(STATUS "Found: opencv, includes: ${OpenCV_INCLUDE_DIR}, libraries: ${OpenCV_LIBRARIES}")
  SET(HAVE_OPENCV 1)
ENDIF(OpenCV_FOUND)

#libfreenect
FIND_PACKAGE(libfreenect)

IF(LIBFREENECT_FOUND)
  INCLUDE_DIRECTORIES( ${FREENECT_INCLUDE_DIR} ) 
  SET(requiredLibs ${requiredLibs} ${FREENECT_LIBRARY} )
  SET(HAVE_FREENECT 1)
ENDIF(LIBFREENECT_FOUND)

#DC1394
FIND_PACKAGE(DC1394)
IF(DC1394_2_FOUND)
  SET(HAVE_DC1394 1)
  OPTION(USE_DC1394_2    "Compile H3D_EXT with the libdc1394.2 library" ON)
  INCLUDE_DIRECTORIES( ${DC1394_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${DC1394_LIBRARIES} )
ELSE(DC1394_2_FOUND)
  SET(USE_DC1394_2 OFF)
ENDIF(DC1394_2_FOUND)

#Flycapture
FIND_PACKAGE(Flycapture)
IF(FLYCAPTURE_FOUND)
  SET(HAVE_FLYCAPTURE 1)
  INCLUDE_DIRECTORIES( ${FLYCAPTURE_INCLUDE_DIRS} )
  SET(requiredLibs ${requiredLibs} ${FLYCAPTURE_LIBRARIES} )
  MESSAGE(STATUS "Found: flycapture, includes ${FLYCAPTURE_INCLUDE_DIRS}, libraries: ${FLYCAPTURE_LIBRARIES}")
ENDIF(FLYCAPTURE_FOUND)



#ARToolkitPlus
FIND_PACKAGE(ARTKP)

IF(ARTKP_FOUND)
  INCLUDE_DIRECTORIES( ${ARTKP_INCLUDE_DIRS} ) 
  SET(requiredLibs ${requiredLibs} ${ARTKP_LIBRARIES} )
  MESSAGE(STATUS "Found: ARToolKitPlus, includes: ${ARTKP_INCLUDE_DIRS}, libraries: ${ARTKP_LIBRARIES}")
  SET(HAVE_ARTKP 1)
ENDIF(ARTKP_FOUND)



# H3DCV shared library definition
ADD_LIBRARY(H3DCV SHARED ${H3DCV_SRCS})


# set the install directory to the MRP directory on Windows
IF(WIN32) 
 SET( CMAKE_INSTALL_PREFIX ${H3DCV_SOURCE_DIR}/../..)
ENDIF(WIN32)

# make sure symbols are exported.
SET( H3DCV_COMPILE_FLAGS "-DH3DCV_EXPORTS" )

# add the libraries needed for linking
TARGET_LINK_LIBRARIES( H3DCV ${requiredLibs} ${optionalLibs} )

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES( H3DCV PROPERTIES DEBUG_POSTFIX "_d" )

# set the version of the library
SET_TARGET_PROPERTIES( H3DCV PROPERTIES VERSION ${H3DCV_FULL_VERSION} )

# change the name depending on compiler to be able to tell them apart
# since they are not compatible with each other. 
SET( TEMP_OUTPUT_POSTFIX "" )
SET( H3DCV_LINK_FLAGS_DEBUG "" )
IF(MSVC70 OR MSVC71)
  SET_TARGET_PROPERTIES( H3DCV PROPERTIES OUTPUT_NAME H3DCV_vc7 )
  SET( TEMP_OUTPUT_POSTFIX _vc7 )
ELSEIF(MSVC80)
  SET_TARGET_PROPERTIES( H3DCV PROPERTIES OUTPUT_NAME H3DCV_vc8 )
  SET( TEMP_OUTPUT_POSTFIX _vc8 )
  # remove some warnings about deprecated functions.
  SET( H3DCV_COMPILE_FLAGS "${H3DCV_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
ELSEIF(MSVC90)
  SET_TARGET_PROPERTIES( H3DCV PROPERTIES OUTPUT_NAME H3DCV_vc9 )
  SET( TEMP_OUTPUT_POSTFIX _vc9 )
  # remove some warnings about deprecated functions.
  SET( H3DCV_COMPILE_FLAGS "${H3DCV_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /MP" )
ENDIF(MSVC70 OR MSVC71)

SET_TARGET_PROPERTIES( H3DCV PROPERTIES COMPILE_FLAGS "${H3DCV_COMPILE_FLAGS}" )

# autogenerate H3DCV.hh depending on the libraries available.
CONFIGURE_FILE( ${H3DCV_SOURCE_DIR}/../include/H3DCV/H3DCV_cmake.hh ${H3DCV_SOURCE_DIR}/../include/H3DCV/H3DCV.hh )

INSTALL( TARGETS H3DCV
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION Plugins
         ARCHIVE DESTINATION lib )

IF( NOT WIN32 )
  INSTALL( DIRECTORY ../include/H3DCV
           DESTINATION include 
           PATTERN .svn EXCLUDE )
ENDIF( NOT WIN32 )
