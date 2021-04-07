ExternalProject_Add(spdlog_src
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.8.5
    GIT_SHALLOW TRUE
    CMAKE_CACHE_ARGS
        -DSPDLOG_BUILD_SHARED:BOOL=ON
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property( spdlog_src binary_dir )
ExternalProject_Get_Property( spdlog_src source_dir )
SET( SPDLOG_INCLUDE_DIRS ${source_dir}/include )
SET( SPDLOG ${binary_dir}/libspdlog${CMAKE_SHARED_LIBRARY_SUFFIX} )