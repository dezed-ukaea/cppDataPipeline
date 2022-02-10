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

set( FDPAPI fdpapi )

# Set Output Directories to avoid issues on multi release compilers
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include External Project and GNUInstallDirs - For Installing Libries
INCLUDE( ExternalProject )
INCLUDE( GNUInstallDirs )

# Set C++ Standard to 17
SET( CMAKE_CXX_STANDARD 11 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )

# Set Include and Header Variables
SET( FDPAPI_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include )
SET( FDPAPI_HEADER ${FDPAPI_INCLUDE_DIRS}/fdp.hxx )

# Default Options for Tests and Code Coverage
OPTION( FDPAPI_BUILD_TESTS  "Build unit tests" OFF )
OPTION( FDPAPI_CODE_COVERAGE "Run GCov and LCov code coverage tools" OFF )

# Set Module Path to include external directory
SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/external" )

# Include external library files
INCLUDE( external/jsoncpp.cmake )
INCLUDE( external/digestpp.cmake )
INCLUDE( external/curl.cmake )
INCLUDE( external/yaml_cpp.cmake )
INCLUDE( external/spdlog.cmake )
INCLUDE( external/toml11.cmake )
INCLUDE( external/ghc.cmake )

# Add the src dirctory compiling it's CMakeLists.txt
ADD_SUBDIRECTORY( src )

# Configure the .in files and install
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/pkg-config/${FDPAPI}.pc.in ${FDPAPI}.pc @ONLY )
INSTALL( FILES ${CMAKE_BINARY_DIR}/${FDPAPI}.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig )

# Compile Tests if specified
IF( FDPAPI_BUILD_TESTS )
    # Include CTest for testing
    INCLUDE( CTest )
    # Add the test dirctory compiling it's CMakeLists.txt
    ADD_SUBDIRECTORY( test )
ENDIF()

# Compile Code Coverage if Specified with Tests
IF( FDPAPI_CODE_COVERAGE AND FDPAPI_BUILD_TESTS )
    if(CMAKE_COMPILER_IS_GNUCXX)
        LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
        include(CodeCoverage)
        APPEND_COVERAGE_COMPILER_FLAGS()

        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 --coverage")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

        #set(COVERAGE_EXCLUDES 'build/*' 'usr/*')
        #set(Coverage_BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
        setup_target_for_coverage_gcovr_sonar(NAME coverage
                              EXECUTABLE bin/${FDPAPI}-tests
                              DEPENDENCIES ${FDPAPI}-tests
                              BASE_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif()
ENDIF()
