IF(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
ELSE(WIN32)
  cmake_minimum_required(VERSION 2.4)
ENDIF(WIN32)

PROJECT(H3DUbitrack)

# optionally enable ETW Tracing manually
IF(WIN32)
OPTION(UBITRACK_USE_ETW "Use EventTracing for Windows Providers" OFF)

IF(UBITRACK_USE_ETW)
ADD_DEFINITIONS(-DHAVE_ETW)
ENDIF(UBITRACK_USE_ETW)
ENDIF(WIN32)

# set the install directory to the H3DUbitrack directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET ) 
  SET( CMAKE_INSTALL_PREFIX ${H3DUbitrack_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )

# Add a cache variable GENERATE_CPACK_PROJECT to have the choice of generating a project
# for packaging H3DUbitrack. Default is NO since most people will not use this.
IF( NOT DEFINED GENERATE_CPACK_PROJECT )
  SET( GENERATE_CPACK_PROJECT "NO" CACHE BOOL "Decides if a cpack project should be generated. The project in the first loaded CMakeLists will configure CPack." )
  MARK_AS_ADVANCED(GENERATE_CPACK_PROJECT)
ENDIF( NOT DEFINED GENERATE_CPACK_PROJECT )

# Add a cache variable PREFER_STATIC_LIBRARIES to have the choice of generating a project
# linking against static libraries if they exist. Default is NO since most people will not use this.
IF( NOT DEFINED PREFER_STATIC_LIBRARIES )
  SET( PREFER_STATIC_LIBRARIES "NO" CACHE BOOL "Decides if CMake should prefer static libraries to dynamic libraries when both exist." )
  MARK_AS_ADVANCED(PREFER_STATIC_LIBRARIES)
ENDIF( NOT DEFINED PREFER_STATIC_LIBRARIES )

IF( PREFER_STATIC_LIBRARIES )
  SET( CMAKE_FIND_LIBRARY_SUFFIXES .a;${CMAKE_FIND_LIBRARY_SUFFIXES} )
ENDIF( PREFER_STATIC_LIBRARIES )

SET( H3DUbitrack_COMPILE_FLAGS "-DH3DUBITRACK_EXPORTS" )

INCLUDE( "${H3DUbitrack_SOURCE_DIR}/H3DUbitrackSourceFiles.txt" )

SET(optionalLibs)

INCLUDE_DIRECTORIES( ../include )

SET(requiredLibs)

# Version of H3DUbitrack
SET( H3DUBITRACK_MAJOR_VERSION 1 )
SET( H3DUBITRACK_MINOR_VERSION 0 )
SET( H3DUBITRACK_BUILD_VERSION 0 )

# Use ThreadSafeFields
SET( H3DUBITRACK_USE_THREADSAFE_FIELDS 0 )

# enable g++ C++0x extensions
#IF(CMAKE_COMPILER_IS_GNUCXX)
#	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x")
#ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Needed to convert from \ to / on windows. Not needed in the FindModules since
# call to macro takes care of that there.
SET( CONVERTED_H3D_ROOT "" )
IF( EXISTS $ENV{H3D_ROOT} )
  FILE( TO_CMAKE_PATH $ENV{H3D_ROOT} CONVERTED_H3D_ROOT )
ENDIF( EXISTS $ENV{H3D_ROOT} )
SET(CMAKE_MODULE_PATH  ${H3DUbitrack_SOURCE_DIR}/modules ${CONVERTED_H3D_ROOT}/build/modules ${H3DUbitrack_SOURCE_DIR}/../../H3DAPI/build/modules/)

macro(getenv_path VAR)
   set(ENV_${VAR} $ENV{${VAR}})
   # replace won't work if var is blank
   if (ENV_${VAR})
     string( REGEX REPLACE "\\\\" "/" ENV_${VAR} ${ENV_${VAR}} )
   endif ()
endmacro(getenv_path)

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  MESSAGE(STATUS "USE 64bit")
  getenv_path(UBITRACKLIB_EXTERNAL64)
  IF(ENV_UBITRACKLIB_EXTERNAL64)
    SET(EXTERNAL_LIBRARIES_DIR "${ENV_UBITRACKLIB_EXTERNAL64}")
  ENDIF(ENV_UBITRACKLIB_EXTERNAL64)
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  MESSAGE(STATUS "USE 32bit")
  getenv_path(UBITRACKLIB_EXTERNAL32)
  IF(ENV_UBITRACKLIB_EXTERNAL32)
    SET(EXTERNAL_LIBRARIES_DIR "${ENV_UBITRACKLIB_EXTERNAL32}")
  ENDIF(ENV_UBITRACKLIB_EXTERNAL32)
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )

MESSAGE(STATUS "Use Externals Libraries Dir: ${EXTERNAL_LIBRARIES_DIR}")

IF( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  INCLUDE_DIRECTORIES( ${EXTERNAL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${H3DUTIL_INCLUDE_DIR} ) 
  INCLUDE_DIRECTORIES( ${HAPI_INCLUDE_DIR} ) 
  INCLUDE_DIRECTORIES( ${H3DAPI_INCLUDE_DIR} ) 
  
  SET( requiredLibs ${requiredLibs} H3DUtil HAPI H3DAPI )
ELSE( H3D_USE_DEPENDENCIES_ONLY )
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

ENDIF( H3D_USE_DEPENDENCIES_ONLY )

# UbiTrack  
FIND_PACKAGE(Ubitrack REQUIRED)

IF(UBITRACK_FOUND)
  SET(HAVE_UBITRACK 1)
  INCLUDE_DIRECTORIES( ${UBITRACK_INCLUDE_DIR} )
  # currently needed because of relative include :(
  INCLUDE_DIRECTORIES( "${UBITRACK_INCLUDE_DIR}/utFacade" )
  SET(requiredLibs ${requiredLibs} ${UBITRACK_LIBRARIES} )
ENDIF(UBITRACK_FOUND)

if(WIN32)
  set(H3DUbitrack_COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS} /EHsc /c /W3 /GR /wd4355 /wd4996 /wd4251 /wd4275 /wd4819 /wd4290")
  set(H3DUbitrack_LINK_FLAGS "/SUBSYSTEM:CONSOLE")
  set(H3DUbitrack_DEFINES "WIN32" "_MBCS" "_WIN32_WINNT=0x501")	
endif(WIN32) 

# settings for 3rdparty libraries
add_definitions(-DTIXML_USE_STL)
add_definitions(-DHAVE_TINYXML)

# Find TinyXML
FIND_PACKAGE(TinyXML REQUIRED)

IF(TINYXML_FOUND)
  SET(HAVE_TINYXML 1)
  INCLUDE_DIRECTORIES( ${TINYXML_INCLUDE_DIR} )
ENDIF(TINYXML_FOUND)

# Find LOG4CPP
FIND_PACKAGE(Log4cpp REQUIRED)

IF(LOG4CPP_FOUND)
  SET(HAVE_LOG4CPP 1)
  INCLUDE_DIRECTORIES( ${LOG4CPP_INCLUDE_DIRS} )
  SET(requiredLibs ${requiredLibs} ${LOG4CPP_LIBRARIES} )
ENDIF(LOG4CPP_FOUND)


# OpenHaptics Toolkit  
FIND_PACKAGE(OpenHaptics)

IF(OPENHAPTICS_FOUND)
  SET(HAVE_OPENHAPTICS 1)
  INCLUDE_DIRECTORIES( ${OPENHAPTICS_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${OPENHAPTICS_LIBRARIES} )
ENDIF(OPENHAPTICS_FOUND)


FIND_PACKAGE(CalibratedPhantom)

IF(CALIBRATEDPHANTOM_FOUND)
  SET(HAVE_CALIBRATEDPHANTOM 1)
  INCLUDE_DIRECTORIES( ${CALIBRATEDPHANTOM_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${CALIBRATEDPHANTOM_LIBRARIES} )
ENDIF(CALIBRATEDPHANTOM_FOUND)

FIND_PACKAGE(RemotePhantom)

IF(REMOTEPHANTOM_FOUND)
  INCLUDE_DIRECTORIES( ${REMOTEPHANTOM_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${REMOTEPHANTOM_LIBRARIES} )
  FIND_PACKAGE(MSGPACK REQUIRED)
  include_directories(${MSGPACK_INCLUDE_DIRS})
  set(requiredLibs ${requiredLibs} ${MSGPACK_LIBRARIES} )
  IF(MSGPACK_FOUND)
    SET(HAVE_REMOTEPHANTOM 1)
  ENDIF(MSGPACK_FOUND)
ENDIF(REMOTEPHANTOM_FOUND)


FIND_PACKAGE(HaptionHAPI)

IF(HAPTIONHAPI_FOUND)
  SET(HAVE_HAPTIONHAPI 1)
  INCLUDE_DIRECTORIES( ${HAPTIONHAPI_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${HAPTIONHAPI_LIBRARIES} )

  # Virtuose API
  FIND_PACKAGE(VirtuoseAPI REQUIRED)

  IF(VIRTUOSE_FOUND)
    SET(HAVE_VIRTUOSEAPI 1)
    INCLUDE_DIRECTORIES( ${VIRTUOSE_INCLUDE_DIR} )
    SET(requiredLibs ${requiredLibs} ${VIRTUOSE_LIBRARIES} )
  ENDIF(VIRTUOSE_FOUND)

ENDIF(HAPTIONHAPI_FOUND)


# Find Boost library. Required to compile.
IF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)
	set(BOOST_ROOT_DIR "${EXTERNAL_LIBRARIES_DIR}/boost")
	INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
ENDIF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)


#Boost bindings
IF(NOT WIN32)
  add_definitions("-DBOOST_UBLAS_BOUNDED_ARRAY_ALIGN=\\ __attribute__\\ \\(\\(aligned\\ \\(16\\)\\)\\)")
ENDIF(NOT WIN32)
add_definitions(-DBOOST_SPIRIT_USE_OLD_NAMESPACE)


# if(MSVC)
#   # force dynamic linking of boost libs on windows ..
#   add_definitions(-DBOOST_ALL_DYN_LINK)
# endif(MSVC)

SET(HAVE_BOOST 0)
set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.54 COMPONENTS thread date_time system filesystem regex chrono locale serialization program_options REQUIRED)
if(Boost_FOUND)
  add_definitions("-DBOOST_ALL_NO_LIB")
  add_definitions("-DBOOST_FILESYSTEM_VERSION=3")
  add_definitions(-DHAVE_BOOST)
  SET(HAVE_BOOST 1)
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
    SET(requiredLibs ${requiredLibs} ${Boost_LIBRARIES} )
endif(Boost_FOUND)

# Find Lapack library. Required to compile.
IF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)
  set(LAPACK_ROOT_DIR "${EXTERNAL_LIBRARIES_DIR}/lapack")
  set(CLAPACK_DIR "${EXTERNAL_LIBRARIES_DIR}/clapack")
ENDIF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)

SET(HAVE_LAPACK 0)
IF(WIN32)
  FIND_PACKAGE(CLAPACK)
  IF(CLAPACK_FOUND)
    SET(LAPACK_FOUND ${CLAPACK_FOUND})
    add_definitions(-DHAVE_CLAPACK)
    include_directories(${CLAPACK_INCLUDE_DIR})
    SET(LAPACK_LIBRARIES ${CLAPACK_LIBRARIES})
  ENDIF(CLAPACK_FOUND)

  IF(NOT LAPACK_FOUND AND DEFINED EXTERNAL_LIBRARIES_DIR)
    SET(LAPACK_LIB_DIR "${EXTERNAL_LIBRARIES_DIR}/lapack/lib")
    # for now just manually define the libraries ..
    SET(LAPACK_LIBRARIES "${LAPACK_LIB_DIR}/atlas.lib" 
               "${LAPACK_LIB_DIR}/cblas.lib" 
               "${LAPACK_LIB_DIR}/f77blas.lib"
               "${LAPACK_LIB_DIR}/g2c.lib"
               "${LAPACK_LIB_DIR}/gcc.lib"
               "${LAPACK_LIB_DIR}/lapack.lib"
               )
    SET(LAPACK_FOUND 1)
  ENDIF(NOT LAPACK_FOUND AND DEFINED EXTERNAL_LIBRARIES_DIR)
ELSE()
  FIND_PACKAGE(LAPACK REQUIRED)
ENDIF(WIN32)
IF(LAPACK_FOUND)
  add_definitions(-DHAVE_LAPACK)
  SET(HAVE_LAPACK 1)
ENDIF(LAPACK_FOUND)


#OpenCV 
IF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)
	set(OPENCV_ROOT_DIR "${EXTERNAL_LIBRARIES_DIR}/opencv" CACHE INTERNAL "OpenCV Root Dir")
ENDIF(WIN32 AND DEFINED EXTERNAL_LIBRARIES_DIR)

SET(HAVE_OPENCV 0)
FIND_PACKAGE(OpenCV)
IF(OPENCV_FOUND)
  MESSAGE(STATUS "Found: opencv, includes: ${OPENCV_INCLUDE_DIR}, libraries: ${OPENCV_LIBRARIES}")
  add_definitions(-DHAVE_OPENCV)
  SET(HAVE_OPENCV 1)
  INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIR} )
  SET(requiredLibs ${requiredLibs} ${OPENCV_LIBRARIES} )
ENDIF(OPENCV_FOUND)


# autogenerate H3DUbitrack.h depending on the libraries available.
CONFIGURE_FILE( ${H3DUbitrack_SOURCE_DIR}/../include/H3DUbitrack/H3DUbitrack.cmake ${H3DUbitrack_SOURCE_DIR}/../include/H3DUbitrack/H3DUbitrack.h )

IF(MSVC)
  # Add the H3DUbitrack.rc resource file if Visual Studio
  SET( H3DUbitrack_SRCS ${H3DUbitrack_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/H3DUbitrack.rc )
ENDIF(MSVC)

IF( COMMAND cmake_policy )
  IF( POLICY CMP0003 )
    cmake_policy(SET CMP0003 OLD)
  ENDIF( POLICY CMP0003 )
ENDIF( COMMAND cmake_policy )
ADD_LIBRARY(H3DUbitrack SHARED ${H3DUbitrack_SRCS} ${H3DUbitrack_HEADERS})

TARGET_LINK_LIBRARIES( H3DUbitrack ${requiredLibs} ${optionalLibs} )

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES DEBUG_POSTFIX "_d" )

IF(UNIX) 
  SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES OUTPUT_NAME h3dubitrack )
ENDIF(UNIX)

IF(MSVC)
  # Create variables
  SET( H3DUbitrack_LINK_FLAGS_DEBUG "" )
  
  SET( H3D_MSVC_VERSION 6 )
  SET( TEMP_MSVC_VERSION 1299 )
  WHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
    MATH( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
    MATH( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
  ENDWHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
  
  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 ( so far ) but we still set it for all.
  SET( H3DUbitrack_COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS} /Zc:wchar_t")
  
  IF( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    SET( H3DUbitrack_COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  ENDIF( ${MSVC_VERSION} GREATER 1399 )

  IF( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    SET( H3DUbitrack_COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS} /MP" )
    # Remove link against default library for debug builds.
    SET( H3DUbitrack_LINK_FLAGS_DEBUG "${H3DUbitrack_LINK_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt" )
  ENDIF( ${MSVC_VERSION} GREATER 1499 )

  SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_vc${H3D_MSVC_VERSION} )

  # Seems like H3DUbitrack needs to have incremental linking shut off for vc8. Since this is
  # not the most important feature it is shut off for all visual studio versions.
  SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES LINK_FLAGS_DEBUG "${H3DUbitrack_LINK_FLAGS_DEBUG} /INCREMENTAL:NO" )

  SET( H3DUBITRACK_SVN_VERSION "0" )
  # Find SubWCRev.exe
  FIND_FILE( SubWCRev
             NAMES "SubWCRev.exe"
             DOC   "Set to SubWCRev.exe that comes with TortoiseSVN. Used to find svn revision number." )
  MARK_AS_ADVANCED( SubWCRev )

  IF( SubWCRev )
    SET( H3DUBITRACK_SVN_VERSION "$WCREV$" )
  ENDIF( SubWCRev )

  # autogenerate H3DUbitrack.rc depending on the version
  CONFIGURE_FILE( H3DUbitrack.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/H3DUbitrack.rc )
  
  ADD_CUSTOM_COMMAND( TARGET H3DUbitrack
	PRE_BUILD
	COMMAND ${H3DUbitrack_SOURCE_DIR}/UpdateResourceFile 
	ARGS H3DUBITRACK ${CMAKE_CURRENT_BINARY_DIR}/H3DUbitrack.rc ${H3DUbitrack_SOURCE_DIR}/H3DUbitrack.rc.cmake
	${H3DUBITRACK_MAJOR_VERSION} ${H3DUBITRACK_MINOR_VERSION}
	${H3DUBITRACK_BUILD_VERSION} "${H3DUBITRACK_SVN_VERSION}"
	"H3DUbitrack_Output_Name"
	"$(TargetFileName)" )

  IF( SubWCRev )
    # Update SVN revision in file.
    EXECUTE_PROCESS( COMMAND ${SubWCRev} ${H3DUbitrack_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DUbitrack.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DUbitrack.rc )

    ADD_CUSTOM_COMMAND( TARGET H3DUbitrack 
                        PRE_BUILD 
                        COMMAND ${SubWCRev} 
                        ARGS ${H3DUbitrack_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DUbitrack.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DUbitrack.rc )
  ENDIF( SubWCRev )
ENDIF(MSVC)

SET( H3DUBITRACK_FULL_VERSION ${H3DUBITRACK_MAJOR_VERSION}.${H3DUBITRACK_MINOR_VERSION}.${H3DUBITRACK_BUILD_VERSION} ) 
SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES VERSION ${H3DUBITRACK_FULL_VERSION} )

# set compile flags for H3DUbitrack project
SET_TARGET_PROPERTIES( H3DUbitrack PROPERTIES COMPILE_FLAGS "${H3DUbitrack_COMPILE_FLAGS}" )

IF( UNIX )
# Install header files on non-windows system (e.g. Unix).
INSTALL( FILES ${H3DUbitrack_HEADERS}
         DESTINATION include/H3DUbitrack )
ENDIF( UNIX )

SET( DEFAULT_BIN_INSTALL "bin" )
SET( DEFAULT_LIB_INSTALL "lib" )

IF( WIN32 )
  SET( DEFAULT_BIN_INSTALL "bin32" )
  SET( DEFAULT_LIB_INSTALL "lib32" )
  IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( DEFAULT_BIN_INSTALL "bin64" )
    SET( DEFAULT_LIB_INSTALL "lib64" )
  ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
ENDIF( WIN32 )

# Install directories.
INSTALL( TARGETS H3DUbitrack
         LIBRARY DESTINATION ${DEFAULT_LIB_INSTALL} COMPONENT H3DUbitrack_cpack_runtime
         RUNTIME DESTINATION ${DEFAULT_BIN_INSTALL} COMPONENT H3DUbitrack_cpack_runtime
         ARCHIVE DESTINATION ${DEFAULT_LIB_INSTALL} COMPONENT H3DUbitrack_cpack_libraries)

IF( H3D_USE_DEPENDENCIES_ONLY )
  SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} PARENT_SCOPE )
ELSE( H3D_USE_DEPENDENCIES_ONLY )
	INCLUDE ( ${H3DUbitrack_SOURCE_DIR}/H3DUbitrackCPack.cmake)
ENDIF( H3D_USE_DEPENDENCIES_ONLY )
