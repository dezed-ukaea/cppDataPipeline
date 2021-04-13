ExternalProject_Add(jsoncpp_src
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG 1.9.4
    GIT_SHALLOW TRUE
    CMAKE_CACHE_ARGS
        -DJSONCPP_WITH_TESTS:BOOL=OFF
        -DJSONCPP_WITH_EXAMPLE:BOOL=OFF
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property( jsoncpp_src binary_dir )
ExternalProject_Get_Property( jsoncpp_src source_dir )
SET( JSONCPP_INCLUDE_DIRS ${source_dir}/include )
SET( JSONCPP ${binary_dir}/lib/libjsoncpp${CMAKE_SHARED_LIBRARY_SUFFIX} )