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
		COMMAND ${LCOV_PATH} --gcov-tool ${GCOV_PATH} --remove ${PROJECT_NAME}.total ${LCOV_EXCLUDES} --output-file coverage.info
	
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
