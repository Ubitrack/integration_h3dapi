IF(WIN32)
cmake_minimum_required(VERSION 2.6.0)
ENDIF(WIN32)
PROJECT(H3DUbitrack)

SET( H3DUbitrack_FULL_VERSION 0.0 ) 


# Set here already in case a special NSIS template needs to be used by cpack.
SET( CONVERTED_H3D_ROOT "" )
IF( EXISTS $ENV{H3D_ROOT} )
  FILE( TO_CMAKE_PATH $ENV{H3D_ROOT} CONVERTED_H3D_ROOT )
ENDIF( EXISTS $ENV{H3D_ROOT} )
SET(CMAKE_MODULE_PATH  ${H3DUbitrack_SOURCE_DIR}/modules ${CONVERTED_H3D_ROOT}/build/modules ${H3DUbitrack_SOURCE_DIR}/../../H3DAPI/build/modules )
INCLUDE( StripAndAddLibraryDirectories )


IF(WIN32) 
    SET(CMAKE_C_FLAGS "ws2_32") 
ENDIF(WIN32) 


#add all sources
AUX_SOURCE_DIRECTORY(../src H3DUbitrack_SRCS)

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

# H3DUbitrack shared library definition
ADD_LIBRARY(H3DUbitrack SHARED ${H3DUbitrack_SRCS})


# set the install directory to the MRP directory on Windows
IF(WIN32) 
 SET( CMAKE_INSTALL_PREFIX ${H3DUbitrack_SOURCE_DIR}/../..)
ENDIF(WIN32)

# make sure symbols are exported.
SET( H3DUbitrack_COMPILE_FLAGS "-DH3DUbitrack_EXPORTS" )

# add the libraries needed for linking
TARGET_LINK_LIBRARIES( H3DUbitrack ${requiredLibs} ${optionalLibs} )

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES DEBUG_POSTFIX "_d" )

# set the version of the library
SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES VERSION ${H3DUbitrack_FULL_VERSION} )

# change the name depending on compiler to be able to tell them apart
# since they are not compatible with each other. 
SET( TEMP_OUTPUT_POSTFIX "" )
SET( H3DUbitrack_LINK_FLAGS_DEBUG "" )
IF(MSVC70 OR MSVC71)
  SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES OUTPUT_NAME H3DUbitrack_vc7 )
  SET( TEMP_OUTPUT_POSTFIX _vc7 )
ELSEIF(MSVC80)
  SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES OUTPUT_NAME H3DUbitrack_vc8 )
  SET( TEMP_OUTPUT_POSTFIX _vc8 )
  # remove some warnings about deprecated functions.
  SET( H3DUbitrack_COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
ELSEIF(MSVC90)
  SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES OUTPUT_NAME H3DUbitrack_vc9 )
  SET( TEMP_OUTPUT_POSTFIX _vc9 )
  # remove some warnings about deprecated functions.
  SET( H3DUbitrack_COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /MP" )
ENDIF(MSVC70 OR MSVC71)

SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS}" )

# autogenerate H3DUbitrack.hh depending on the libraries available.
CONFIGURE_FILE( ${H3DUbitrack_SOURCE_DIR}/../include/H3DUbitrack/H3DUbitrack_cmake.hh ${H3DUbitrack_SOURCE_DIR}/../include/H3DUbitrack/H3DUbitrack.hh )

INSTALL( TARGETS H3DUbitrack
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION Plugins
         ARCHIVE DESTINATION lib )

IF( NOT WIN32 )
  INSTALL( DIRECTORY ../include/H3DUbitrack
           DESTINATION include 
           PATTERN .svn EXCLUDE )
ENDIF( NOT WIN32 )
