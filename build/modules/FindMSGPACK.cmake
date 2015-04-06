# - Find msgpack
# Find the native Message Pack headers and libraries.
#
# MSGPACK_INCLUDE_DIRS - where to find msgpack.hpp, etc.
# MSGPACK_LIBRARIES - List of libraries when using MSGPACK.
# MSGPACK_FOUND - True if MSGPACK found.


# Look for the header file.
FIND_PATH(MSGPACK_INCLUDE_DIR NAMES msgpack.hpp)

# Look for the library.
FIND_LIBRARY(MSGPACK_LIBRARY NAMES msgpack libmsgpack)

# handle the QUIETLY and REQUIRED arguments and set MSGPACK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MSGPACK DEFAULT_MSG MSGPACK_LIBRARY MSGPACK_INCLUDE_DIR)

# Copy the results to the output variables.
IF(MSGPACK_FOUND)
  #MESSAGE(STATUS "MSGPACK found")
  SET(MSGPACK_LIBRARIES ${MSGPACK_LIBRARY})
  SET(MSGPACK_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})
ELSE(MSGPACK_FOUND)
  MESSAGE(STATUS "MSGPACK not found")
  SET(MSGPACK_LIBRARIES)
  SET(MSGPACK_INCLUDE_DIRS)
ENDIF(MSGPACK_FOUND)

MARK_AS_ADVANCED(MSGPACK_INCLUDE_DIR MSGPACK_LIBRARY)