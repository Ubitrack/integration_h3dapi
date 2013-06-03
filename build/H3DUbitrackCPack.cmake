IF( NOT TARGET H3DUbitrack )
  MESSAGE( FATAL_ERROR "Include file H3DUbitrackCPack.cmake require the target H3DUbitrack to exist. Please add H3DUbitrack/build/CMakeLists.txt as subdirectory first." )
ENDIF( NOT TARGET H3DUbitrack )

# Add all sources, they are added to a variable called H3DUbitrack_SRCS defined
# in the included file. All header files are added to a variable called
# H3DUbitrack_HEADERS.
INCLUDE( ${H3DUbitrack_SOURCE_DIR}/H3DUbitrackSourceFiles.txt )

IF( GENERATE_CPACK_PROJECT )
  IF( WIN32 )
    # Add a cache variable which indicates where the Externals directory used for packaging
    # H3DUbitrack is located. If not set then FIND modules will be used instead.
    IF( NOT DEFINED H3DUbitrack_CPACK_EXTERNAL_ROOT )
      SET( H3DUbitrack_CPACK_EXTERNAL_ROOT "$ENV{H3D_EXTERNAL_ROOT}" CACHE PATH "Set to the External directory used with H3DUbitrack, needed to pack properly. If not set FIND_modules will be used instead." )
      MARK_AS_ADVANCED(H3DUbitrack_CPACK_EXTERNAL_ROOT)
    ENDIF( NOT DEFINED H3DUbitrack_CPACK_EXTERNAL_ROOT )
  ENDIF( WIN32 )

  # Set information properties about the project to install.
  set( CPACK_ALL_INSTALL_TYPES Full Developer )
  SET(CMAKE_MODULE_PATH ${H3DUbitrack_SOURCE_DIR}/modules )
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "H3DUbitrack. A bridge for H3D to use the Ubitrack Dataflow Library.")
  SET(CPACK_PACKAGE_VENDOR "Magic Vision Lab")
  SET(CPACK_PACKAGE_CONTACT "ulrich.eck@magicvisionlab.com" )
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${H3DUbitrack_SOURCE_DIR}/../ReadMe")
  SET(CPACK_RESOURCE_FILE_LICENSE "${H3DUbitrack_SOURCE_DIR}/../LICENSE")
  
  # Project to install.
  SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};H3DUbitrack;ALL;/")

  # Installation directory for package.
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "H3D" )
  
  # File patterns to ignore, common for all operating systems.
  SET( H3DUbitrack_CPACK_IGNORE_PATTERNS /\\\\.svn/
                                    \\\\.obj$
                                    \\\\.ncb$
                                    \\\\.log$
                                    \\\\.suo$
                                    \\\\.zip$
                                    \\\\.dir/
                                    \\\\.user$
                                    \\\\.cv$
                                    "/Debug(.)*/"
                                    "/debug(.)*/"
                                    /Release
                                    /release
                                    /linux
                                    /build/win32/
                                    /build/vc8
                                    /build/vc7
                                    /build/vc9
                                    /osx
                                    /notes\\\\.txt$ )

  SET(CPACK_PACKAGE_VERSION_MAJOR ${H3DUBITRACK_MAJOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_MINOR ${H3DUBITRACK_MINOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_PATCH ${H3DUBITRACK_BUILD_VERSION})
  
  IF(WIN32 AND NOT UNIX)
    SET(CPACK_NSIS_INSTALL_ROOT "C:" )
    SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON" )
    SET( CPACK_PACKAGE_START_MENU_NAME "H3DUbitrack for H3DAPI 2.2" )
    #Extra links to start menu if values are "ON"
    SET( CPACK_ADD_H3DUbitrackDOC_LINKS "ON" )
    #SET( NSIS_OPTIONS_NUMFIELDS "4" )
	
	# External binary directory to add to path.
    SET( CPACK_H3D_64_BIT "FALSE" )
    SET( CPACK_EXTERNAL_BIN "bin32" )
    IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      SET( CPACK_EXTERNAL_BIN "bin64" )
      SET( CPACK_H3D_64_BIT "TRUE" )
    ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    
    	
	#We dont set any lib/bin/header here because all of them are required and should be added by CMakeLists.txt by default
	
	# autogenerate NSIS.InstallOptions.ini depending on the version
    #CONFIGURE_FILE( ${H3DUbitrack_SOURCE_DIR}/modules/NSIS.InstallOptions.ini.in.cmake ${H3DUbitrack_SOURCE_DIR}/modules/NSIS.InstallOptions.ini.in )

  ELSE(WIN32 AND NOT UNIX)
    SET( H3DUbitrack_CPACK_IGNORE_PATTERNS ${H3DUbitrack_CPACK_IGNORE_PATTERNS}
                                      /plugin/
                                      "~$" )
  ENDIF(WIN32 AND NOT UNIX)
  
  #install header directory
  INSTALL( FILES ${H3DUbitrack_HEADERS}
           DESTINATION H3DUbitrack/include/H3DUbitrack
           COMPONENT H3DUbitrack_cpack_headers)

  # H3DUbitrack.cmake that goes to headers is not needed unless sources is required.
  INSTALL( FILES ${H3DUbitrack_SOURCE_DIR}/../include/H3DUbitrack/H3DUbitrack.cmake
			DESTINATION H3DUbitrack/include/H3D/H3DUbitrack
			COMPONENT H3DUbitrack_cpack_sources )
		   
  #install build files
  INSTALL( FILES ${H3DUbitrack_SOURCE_DIR}/CMakeLists.txt
				 ${H3DUbitrack_SOURCE_DIR}/H3DUbitrack.rc.cmake
				 ${H3DUbitrack_SOURCE_DIR}/H3DUbitrackCPack.cmake
				 ${H3DUbitrack_SOURCE_DIR}/H3DUbitrackSourceFiles.txt
				 ${H3DUbitrack_SOURCE_DIR}/UpdateResourceFile.exe
           DESTINATION H3DUbitrack/build
           COMPONENT H3DUbitrack_cpack_sources)
		   
  #install module directory
  INSTALL( DIRECTORY ${H3DUbitrack_SOURCE_DIR}/modules
           DESTINATION H3DUbitrack/build
           COMPONENT H3DUbitrack_cpack_sources)
		   
  #install data directory
  #INSTALL( DIRECTORY ${H3DUbitrack_SOURCE_DIR}/../data
  #         DESTINATION H3DUbitrack
  #         COMPONENT H3DUbitrack_cpack_sources)

  #install source directory
  INSTALL( DIRECTORY ${H3DUbitrack_SOURCE_DIR}/../src
           DESTINATION H3DUbitrack
           COMPONENT H3DUbitrack_cpack_sources)
		   
  #install x3d directory
  INSTALL( DIRECTORY ${H3DUbitrack_SOURCE_DIR}/../x3d
           DESTINATION H3DUbitrack
           COMPONENT H3DUbitrack_cpack_sources)
		   
  #install change log, readme, etc files
  INSTALL( FILES ${H3DUbitrack_SOURCE_DIR}/../changelog
				 ${H3DUbitrack_SOURCE_DIR}/../LICENSE
				 ${H3DUbitrack_SOURCE_DIR}/../ReadMe
           DESTINATION H3DUbitrack
           COMPONENT H3DUbitrack_cpack_sources)
  
  IF( ( EXISTS ${H3DUbitrack_SOURCE_DIR}/../doc/H3DUbitrack.tag ) AND ( EXISTS ${H3DUbitrack_SOURCE_DIR}/../doc/html ) )
    # Install documentation
    INSTALL( FILES ${H3DUbitrack_SOURCE_DIR}/../doc/H3DUbitrack.tag
             DESTINATION H3DUbitrack/doc
             COMPONENT H3DUbitrack_cpack_headers)
    INSTALL( DIRECTORY ${H3DUbitrack_SOURCE_DIR}/../doc/html
             DESTINATION H3DUbitrack/doc
             COMPONENT H3DUbitrack_cpack_headers)
  ENDIF( ( EXISTS ${H3DUbitrack_SOURCE_DIR}/../doc/H3DUbitrack.tag ) AND ( EXISTS ${H3DUbitrack_SOURCE_DIR}/../doc/html ) )

  # setting names and dependencies between components and also grouping them.
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_RUNTIME_DISPLAY_NAME "Runtime")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_RUNTIME_DESCRIPTION "The runtime libraries (dlls) for H3DUbitrack.")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_RUNTIME_DEPENDS H3DAPI_cpack_runtime )
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_RUNTIME_GROUP "H3DUbitrack_cpack_group")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_RUNTIME_INSTALL_TYPES Developer Full)
  
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_LIBRARIES_DISPLAY_NAME "Libraries")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_LIBRARIES_DESCRIPTION "H3DUbitrack libraries, needed for building against H3DUbitrack.")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_LIBRARIES_DEPENDS H3DAPI_cpack_libraries H3DUbitrack_cpack_headers )
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_LIBRARIES_GROUP "H3DUbitrack_cpack_group")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_LIBRARIES_INSTALL_TYPES Developer Full)
    
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_HEADERS_DISPLAY_NAME "C++ Headers")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_HEADERS_DESCRIPTION "H3DUbitrack C++ headers, needed for building against H3DUbitrack.")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_HEADERS_DEPENDS H3DAPI_cpack_headers H3DUbitrack_cpack_libraries )
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_HEADERS_GROUP "H3DUbitrack_cpack_group")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_HEADERS_INSTALL_TYPES Developer Full)
    
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_SOURCES_DISPLAY_NAME "C++ Source")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_SOURCES_DESCRIPTION "Everything needed to build H3DUbitrack.")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_SOURCES_DEPENDS H3DAPI_cpack_sources H3DUbitrack_cpack_headers )
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_SOURCES_GROUP "H3DUbitrack_cpack_group")
  set(CPACK_COMPONENT_H3DUBITRACK_CPACK_SOURCES_INSTALL_TYPES Full)
  
  set(CPACK_COMPONENT_GROUP_H3DUBITRACK_CPACK_GROUP_DISPLAY_NAME "H3DUbitrack")
  set(CPACK_COMPONENT_GROUP_H3DUBITRACK_CPACK_GROUP_DESCRIPTION "H3DUbitrack provides a bridge to connect H3D with the Ubitrack Dataflow Library.")
  
  # Add a cache variable H3D_cmake_runtime_path to point to cmake binary.
  SET (H3D_cmake_runtime_path_default "")
  IF( NOT DEFINED H3D_cmake_runtime_path )
    IF( WIN32 AND NOT UNIX )
      SET (VERSION_CMAKES "4.0" "3.9" "3.8" "3.7" "3.6" "3.5" "3.4" "3.3" "3.2" "3.1" "3.0" "2.9" "2.8" "2.7" "2.6")
      foreach (version_cmake ${VERSION_CMAKES} )
        IF (EXISTS "C:/Program Files/CMake ${version_cmake}/bin/cmake.exe")
          SET( H3D_cmake_runtime_path_default "C:/Program Files/CMake ${version_cmake}/bin/cmake.exe" )
          break()
        ENDIF (EXISTS "C:/Program Files/CMake ${version_cmake}/bin/cmake.exe")
        
        IF (EXISTS "C:/Program Files (x86)/CMake ${version_cmake}/bin/cmake.exe")
          SET( H3D_cmake_runtime_path_default "C:/Program Files (x86)/CMake ${version_cmake}/bin/cmake.exe" )
          break()
        ENDIF (EXISTS "C:/Program Files (x86)/CMake ${version_cmake}/bin/cmake.exe")
        
        IF ( EXISTS "C:/Program/CMake ${version_cmake}/bin/cmake.exe")
          SET( H3D_cmake_runtime_path_default "C:/Program/CMake ${version_cmake}/bin/cmake.exe" )
          break()
        ENDIF ( EXISTS "C:/Program/CMake ${version_cmake}/bin/cmake.exe")
      endforeach (version_cmake )
    ELSE( WIN32 AND NOT UNIX )
      SET( H3D_cmake_runtime_path_default "cmake" )
    ENDIF( WIN32 AND NOT UNIX )
    SET( H3D_cmake_runtime_path ${H3D_cmake_runtime_path_default} CACHE FILEPATH "The path to the cmake runtime." )
    MARK_AS_ADVANCED(H3D_cmake_runtime_path)
  ENDIF( NOT DEFINED H3D_cmake_runtime_path )
  
  IF(UNIX)
    SET(CPACK_SOURCE_INSTALLED_DIRECTORIES "${H3DUbitrack_SOURCE_DIR}/..;/" )	
    SET(CPACK_SOURCE_GENERATOR TGZ ZIP ) 
    SET(CPACK_SOURCE_PACKAGE_FILE_NAME "h3dubitrack-${H3DUBITRACK_MAJOR_VERSION}.${H3DUBITRACK_MINOR_VERSION}.${H3DUBITRACK_BUILD_VERSION}") 


    SET( H3DUbitrack_CPACK_IGNORE_PATTERNS ${H3DUbitrack_CPACK_IGNORE_PATTERNS}
				    "/CVS/;/.svn/;/.bzr/;/.hg/;/.git.*/;.swp$;.#;/#;~$")
    SET(CPACK_SOURCE_IGNORE_FILES ${H3DUbitrack_CPACK_IGNORE_PATTERNS} )
  ENDIF( UNIX )

  IF( H3D_cmake_runtime_path )
    SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD} 
                                                       COMMAND ${H3D_cmake_runtime_path} 
                                                       ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DUbitrack_cpack_runtime -P cmake_install.cmake 
                                                       COMMAND ${H3D_cmake_runtime_path} 
                                                       ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DUbitrack_cpack_libraries -P cmake_install.cmake
                                                       COMMAND ${H3D_cmake_runtime_path} 
                                                       ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DUbitrack_cpack_examples_runtime -P cmake_install.cmake )
    
    IF (NOT H3D_USE_DEPENDENCIES_ONLY)
      ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/DummyFile
                          COMMAND echo )
      ADD_CUSTOM_TARGET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                         DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/DummyFile )

      ADD_CUSTOM_COMMAND( TARGET INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                          POST_BUILD
                          ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD}
                         )
      ADD_DEPENDENCIES( INSTALL_RUNTIME_AND_LIBRARIES_ONLY H3DUbitrack ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
    ENDIF (NOT H3D_USE_DEPENDENCIES_ONLY)
  ELSE( H3D_cmake_runtime_path )
    MESSAGE (STATUS "H3D_cmake_runtime_path is not set, please set it to continue")
  ENDIF( H3D_cmake_runtime_path )
  
  IF (NOT H3D_USE_DEPENDENCIES_ONLY)
	INCLUDE(CPack)
  ENDIF( NOT H3D_USE_DEPENDENCIES_ONLY)
  #CPack said: could not find load file Debian
  # INCLUDE(UseDebian)
  # IF(DEBIAN_FOUND)
    # ADD_DEBIAN_TARGETS(H3DUbitrack)
  # ENDIF(DEBIAN_FOUND)
ENDIF( GENERATE_CPACK_PROJECT )
