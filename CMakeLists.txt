CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )


IF (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    message(STATUS "CMake Build type not specified: Using ${CMAKE_BUILD_TYPE} by default")
ENDIF()

PROJECT( fdpapi
    VERSION 0.1.0
    DESCRIPTION "C++ API for the FAIR Data Pipeline"
    HOMEPAGE_URL ""
    LANGUAGES CXX C
)

IF( CMAKE_BUILD_TYPE MATCHES Debug )
    set( LIB_POST_FIX "-d" CACHE STRING "Libarary Postfix" FORCE)
    set( LIB_POST_FIX_MIN "d" CACHE STRING "Libarary Minimum Postfix" FORCE)
    message(STATUS "Library Postfix set to ${LIB_POST_FIX}")
ELSE()
    set( LIB_POST_FIX "" CACHE STRING "Libarary Postfix" FORCE)
    message(STATUS "Library Postfix set to ${LIB_POST_FIX}")
ENDIF()

INCLUDE( ExternalProject )
INCLUDE( GNUInstallDirs )

SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )

SET( FDPAPI_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include )
SET( FDPAPI_HEADER ${FDPAPI_INCLUDE_DIRS}/fdp.hxx )

OPTION( FDPAPI_BUILD_TESTS  "Build unit tests" OFF )
OPTION( FDPAPI_CODE_COVERAGE "Run GCov and LCov code coverage tools" OFF )

FIND_PACKAGE( HDF5 REQUIRED COMPONENTS C CXX )

SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/external" )

INCLUDE( external/digestpp.cmake )
INCLUDE( external/curl.cmake )
INCLUDE( external/yaml_cpp.cmake )
INCLUDE( external/jsoncpp.cmake )
INCLUDE( external/spdlog.cmake )
INCLUDE( external/toml11.cmake )

ADD_SUBDIRECTORY( src )

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/pkg-config/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY )
INSTALL( FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig )

IF( FDPAPI_BUILD_TESTS )
    INCLUDE( CTest )
    ADD_SUBDIRECTORY( test )
ENDIF()

IF( FDPAPI_CODE_COVERAGE AND FDPAPI_BUILD_TESTS )
    MESSAGE( STATUS "[Coverage]")
    SET( COVERAGE_COMPILER_FLAGS "--coverage -fprofile-arcs -ftest-coverage" )
	SET( CMAKE_CXX_FLAGS "-std=gnu++17 -O0 -Wall -Wextra -Werror ${COVERAGE_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS}" )
    SET( CMAKE_C_FLAGS "-O0 -Wall -Wextra ${COVERAGE_COMPILER_FLAGS} ${CMAKE_C_FLAGS}" )
    MESSAGE( "\t CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}" )
    MESSAGE( "\t CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}" )
    

	find_program( LCOV_PATH  NAMES lcov lcov.bat lcov.exe lcov.perl)
	find_program( GCOV_PATH gcov )
	
	set(LCOV_EXCLUDES "build/*" "/usr*")
	
	add_custom_target(coverage
	
        # Cleanup lcov
        COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} -directory . -b ${PROJECT_SOURCE_DIR} --zerocounters
	
	    # Create baseline to make sure untouched files show up in the report
		COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} -c -i -d . -b ${PROJECT_SOURCE_DIR} -o ${PROJECT_NAME}.base
	
	    # Run executables
		COMMAND ${CMAKE_BINARY_DIR}/test/${PROJECT_NAME}-tests ${COV_TEST_FLAGS}
	
	    # Capturing lcov counters and generating report
		COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --directory . -b ${PROJECT_SOURCE_DIR} --capture --output-file ${PROJECT_NAME}.capture
	
	    # add baseline counters
		COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} -a ${PROJECT_NAME}.base -a ${PROJECT_NAME}.capture --output-file ${PROJECT_NAME}.total
	
	    # filter collected data to final coverage report and merge outputs
		COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --remove ${PROJECT_NAME}.total ${LCOV_EXCLUDES} --output-file ${PROJECT_NAME}.info
	
        # Set output files as GENERATED (will be removed on 'make clean')
        BYPRODUCTS
            ${PROJECT_NAME}.base
            ${PROJECT_NAME}.capture
            ${PROJECT_NAME}.total
            ${PROJECT_NAME}.info
            ${PROJECT_NAME}  # report directory

        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        VERBATIM # Protect arguments to commands
        COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
	)
ENDIF()
