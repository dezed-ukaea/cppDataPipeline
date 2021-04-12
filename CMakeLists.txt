CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

PROJECT( scrcapi VERSION 0.1.0 LANGUAGES CXX )

INCLUDE( ExternalProject )

SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )

SET( SCRCAPI_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include )

INCLUDE( external/digestpp.cmake )
INCLUDE( external/curl.cmake )
INCLUDE( external/yaml_cpp.cmake )
INCLUDE( external/jsoncpp.cmake )
INCLUDE( external/spdlog.cmake )

ADD_SUBDIRECTORY( src )

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    MESSAGE(STATUS "Including Code Coverage")
    SET( COVERAGE_COMPILER_FLAGS "-g -fprofile-arcs -ftest-coverage" )
	SET( CMAKE_CXX_FLAGS "-std=gnu++17 -O0 -Wall -Wextra ${COVERAGE_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS}" )
    INCLUDE( CTest )
    ADD_SUBDIRECTORY( test )

    INCLUDE( external/coverage.cmake )
    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE ${PROJECT_NAME}-tests
        DEPENDENCIES ${PROJECT_NAME}
        EXCLUDE ${CMAKE_BINARY_DIR}
    )
ENDIF()
