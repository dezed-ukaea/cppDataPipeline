FIND_PACKAGE( Threads REQUIRED )

ENABLE_TESTING()

SET( CMAKE_CXX_FLAGS "-DTESTDIR=\\\"${CMAKE_CURRENT_SOURCE_DIR}\\\" ${CMAKE_CXX_FLAGS}" )

SET( TEST_NAME ${PROJECT_NAME}-tests )

# assume built-in pthreads on MacOS
IF( APPLE )
    SET( CMAKE_THREAD_LIBS_INIT "-lpthread" )
    SET( CMAKE_HAVE_THREADS_LIBRARY 1 )
    SET( CMAKE_USE_WIN32_THREADS_INIT 0 )
    SET( CMAKE_USE_PTHREADS_INIT 1 )
    SET( THREADS_PREFER_PTHREAD_FLAG ON )
ENDIF()

IF( WIN32 )
    option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)
    option(gtest_disable_pthreads "Disable uses of pthreads in gtest." ON)
ENDIF()


# Prevent overriding the parent project's compiler/linker
# settings on Windows

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find all files matching test naming (test_<name>.cxx)
#FILE(GLOB_RECURSE src_unit CONFIGURE_DEPENDS "test_*.cxx")

# Add the test files to the executable
ADD_EXECUTABLE( ${TEST_NAME} ${src_unit} )

# Add compile options to tests
ADD_COMPILE_OPTIONS( ${TEST_NAME} ${BUILD_TYPE_COMPILE_FLAGS} )

# Add includes to tests
TARGET_INCLUDE_DIRECTORIES( ${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include
                            ${TEST_NAME} PUBLIC ${pybind11_INCLUDE_DIRS}
                            ${TEST_NAME} PUBLIC ${PYTHON_INCLUDE_DIRS})

MESSAGE(STATUS "----- Configuring Test Build -----")
MESSAGE(STATUS "[Libraries]:")
MESSAGE(STATUS "\tAPI C++ BINDINGS: ${CMAKE_SOURCE_DIR}")
MESSAGE(STATUS "\tGTEST: ${GTEST_INCLUDE_DIRS}")
MESSAGE(STATUS "\tGMOCK: ${GMOCK_INCLUDE_DIRS}")
MESSAGE(STATUS "\tFLAGS: ${CMAKE_CXX_FLAGS}")
MESSAGE(STATUS "----------------------------------")

# Link gtests with tests
TARGET_LINK_LIBRARIES( ${TEST_NAME} PRIVATE ${PROJECT_NAME} gtest gtest_main)

# Windows Requires the compiled DLL files to be in the same directory (alternative add these directories to PATH)
IF( WIN32 )
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_BINARY_DIR}/bin/Release"
            $<TARGET_FILE_DIR:${TEST_NAME}>)
    add_custom_command(TARGET ${TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_BINARY_DIR}/jsoncpp_src-prefix/src/jsoncpp_src-build/bin/Release"
            $<TARGET_FILE_DIR:${TEST_NAME}>)
ENDIF()
# Include GoogleTest Library
INCLUDE( GoogleTest )
# Use GoogleTest Libary to Discover the tests
gtest_discover_tests( ${TEST_NAME} )