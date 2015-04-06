# - Find REMOTEPHANTOM
# Find the native REMOTEPHANTOM headers and libraries.
#
#  REMOTEPHANTOM_INCLUDE_DIR -  where to find RemotePhantom/RemotePhantom.hh, etc.
#  REMOTEPHANTOM_LIBRARIES    - List of libraries when using REMOTEPHANTOM.
#  REMOTEPHANTOM_FOUND        - True if REMOTEPHANTOM found.

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header file.
FIND_PATH( REMOTEPHANTOM_INCLUDE_DIR NAMES RemotePhantom/RemotePhantom.h
           PATHS $ENV{H3D_ROOT}/include
                 ../include
                 ${module_file_path}/../../include
                 ${module_file_path}/../../../RemotePhantomDevice/include
           DOC "Path in which the file H3D/RemotePhantom.h is located." )
MARK_AS_ADVANCED(REMOTEPHANTOM_INCLUDE_DIR)

IF( MSVC )
  SET( H3D_MSVC_VERSION 6 )
  SET( TEMP_MSVC_VERSION 1299 )
  WHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
    MATH( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
    MATH( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
  ENDWHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
  SET( REMOTEPHANTOM_NAME "RemotePhantom_vc${H3D_MSVC_VERSION}" )
ELSE(MSVC)
  SET( REMOTEPHANTOM_NAME remotephantom )
ENDIF( MSVC )

SET( DEFAULT_LIB_INSTALL "lib" )
IF( WIN32 )
  SET( DEFAULT_LIB_INSTALL "lib32" )
  IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( DEFAULT_LIB_INSTALL "lib64" )
  ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
ENDIF( WIN32 )

# Look for the library.
FIND_LIBRARY( REMOTEPHANTOM_LIBRARY NAMES ${REMOTEPHANTOM_NAME}
              PATHS $ENV{H3D_ROOT}/../${DEFAULT_LIB_INSTALL}
                    ../../${DEFAULT_LIB_INSTALL}
                    ${module_file_path}/../../../${DEFAULT_LIB_INSTALL}
              DOC "Path to ${REMOTEPHANTOM_NAME} library." )

FIND_LIBRARY( REMOTEPHANTOM_DEBUG_LIBRARY NAMES ${REMOTEPHANTOM_NAME}_d
              PATHS $ENV{H3D_ROOT}/../${DEFAULT_LIB_INSTALL}
                    ../../${DEFAULT_LIB_INSTALL}
                    ${module_file_path}/../../../${DEFAULT_LIB_INSTALL}
              DOC "Path to ${REMOTEPHANTOM_NAME}_d library." )
MARK_AS_ADVANCED(REMOTEPHANTOM_LIBRARY)
MARK_AS_ADVANCED(REMOTEPHANTOM_DEBUG_LIBRARY)

IF(REMOTEPHANTOM_LIBRARY OR REMOTEPHANTOM_DEBUG_LIBRARY)
  SET( HAVE_REMOTEPHANTOM_LIBRARY 1 )
ELSE(REMOTEPHANTOM_LIBRARY OR REMOTEPHANTOM_DEBUG_LIBRARY)
  SET( HAVE_REMOTEPHANTOM_LIBRARY 0 )
ENDIF(REMOTEPHANTOM_LIBRARY OR REMOTEPHANTOM_DEBUG_LIBRARY)

# Copy the results to the output variables.
IF(REMOTEPHANTOM_INCLUDE_DIR AND HAVE_REMOTEPHANTOM_LIBRARY)

  SET(REMOTEPHANTOM_FOUND 1)
  IF(REMOTEPHANTOM_LIBRARY)
    SET(REMOTEPHANTOM_LIBRARIES ${REMOTEPHANTOM_LIBRARIES} optimized ${REMOTEPHANTOM_LIBRARY} )
  ELSE(REMOTEPHANTOM_LIBRARY)
    SET(REMOTEPHANTOM_LIBRARIES ${REMOTEPHANTOM_LIBRARIES} optimized ${REMOTEPHANTOM_NAME} )
    MESSAGE( STATUS "REMOTEPHANTOM release libraries not found. Release build might not work." )
  ENDIF(REMOTEPHANTOM_LIBRARY)

  IF(REMOTEPHANTOM_DEBUG_LIBRARY)
    SET(REMOTEPHANTOM_LIBRARIES ${REMOTEPHANTOM_LIBRARIES} debug ${REMOTEPHANTOM_DEBUG_LIBRARY} )
  ELSE(REMOTEPHANTOM_DEBUG_LIBRARY)
    SET(REMOTEPHANTOM_LIBRARIES ${REMOTEPHANTOM_LIBRARIES} debug ${REMOTEPHANTOM_NAME}_d )
    MESSAGE( STATUS "REMOTEPHANTOM debug libraries not found. Debug build might not work." )
  ENDIF(REMOTEPHANTOM_DEBUG_LIBRARY)

  SET(REMOTEPHANTOM_INCLUDE_DIR ${REMOTEPHANTOM_INCLUDE_DIR} )
  SET(REMOTEPHANTOM_LIBRARIES ${REMOTEPHANTOM_LIBRARIES} )
ELSE(REMOTEPHANTOM_INCLUDE_DIR AND HAVE_REMOTEPHANTOM_LIBRARY)
  SET(REMOTEPHANTOM_FOUND 0)
  SET(REMOTEPHANTOM_LIBRARIES)
  SET(REMOTEPHANTOM_INCLUDE_DIR)
ENDIF(REMOTEPHANTOM_INCLUDE_DIR AND HAVE_REMOTEPHANTOM_LIBRARY)

# Report the results.
IF(NOT REMOTEPHANTOM_FOUND)
  SET(REMOTEPHANTOM_DIR_MESSAGE
    "REMOTEPHANTOM was not found. Make sure REMOTEPHANTOM_LIBRARY ( and/or REMOTEPHANTOM_DEBUG_LIBRARY ) and REMOTEPHANTOM_INCLUDE_DIR are set.")
  IF(REMOTEPHANTOM_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${REMOTEPHANTOM_DIR_MESSAGE}")
  ELSEIF(NOT REMOTEPHANTOM_FIND_QUIETLY)
    MESSAGE(STATUS "${REMOTEPHANTOM_DIR_MESSAGE}")
  ENDIF(REMOTEPHANTOM_FIND_REQUIRED)
ENDIF(NOT REMOTEPHANTOM_FOUND)