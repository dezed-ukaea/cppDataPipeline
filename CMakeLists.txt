# Ensure that cmake has fetch_content commands with minimum of 3.13
CMAKE_MINIMUM_REQUIRED( VERSION 3.13 )

#  Disable Building in Debug as HD5 
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
IF(isMultiConfig)
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
    MESSAGE("Multi-Config Generator found, please ensure you are building with a realease configuration")
ELSE()
    if(NOT CMAKE_BUILD_TYPE)
        message("Defaulting to release build.")
        set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
    ENDIF()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
    # set the valid options for cmake-gui drop-down list
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release")
ENDIF()

# Define Project
PROJECT( fdpapi
    VERSION 0.1.0
    DESCRIPTION "C++ API for the FAIR Data Pipeline"
    HOMEPAGE_URL "https://github.com/FAIRDataPipeline/cppDataPipeline"
    LANGUAGES CXX C
)

# Use HDF5 Static Libraries on Windows
if ( WIN32 )
    SET (HDF5_USE_STATIC_LIBRARIES TRUE)
ENDIF()

# Find HDF5 Package
find_package(HDF5 COMPONENTS C CXX)

# If the HDF5 Package is not found stop
if (NOT HDF5_FOUND)
    message( FATAL_ERROR "HDF5 Library Not Found, 
        Please install it and ensure the Environment Variable HDF5_ROOT is set")
ENDIF()

# Set Output Directories to avoid issues on multi release compilers
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include External Project and GNUInstallDirs - For Installing Libries
INCLUDE( ExternalProject )
INCLUDE( GNUInstallDirs )

# Set C++ Standard to 17
SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )

# Set Include and Header Variables
SET( FDPAPI_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include )
SET( FDPAPI_HEADER ${FDPAPI_INCLUDE_DIRS}/fdp.hxx )

# Default Options for Tests and Code Coverage
OPTION( FDPAPI_BUILD_TESTS  "Build unit tests" OFF )
OPTION( FDPAPI_CODE_COVERAGE "Run GCov and LCov code coverage tools" OFF )

# Set Module Path to include external directory
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/external" )

# Include external library files
INCLUDE( external/jsoncpp.cmake )
INCLUDE( external/digestpp.cmake )
INCLUDE( external/curl.cmake )
INCLUDE( external/yaml_cpp.cmake )
INCLUDE( external/spdlog.cmake )
INCLUDE( external/toml11.cmake )

# Add the src dirctory compiling it's CMakeLists.txt
ADD_SUBDIRECTORY( src )

# Configure the .in files and install
CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/pkg-config/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY )
INSTALL( FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig )

# Compile Tests if specified
IF( FDPAPI_BUILD_TESTS )
    # Include CTest for testing
    INCLUDE( CTest )
    # Add the test dirctory compiling it's CMakeLists.txt
    ADD_SUBDIRECTORY( test )
ENDIF()

# Compile Code Coverage if Specified with Tests
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
