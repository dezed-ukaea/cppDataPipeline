SET( YAML_CPP "yaml-cpp" )
ExternalProject_Add(
    ${YAML_CPP}_src
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.6.3
    GIT_SHALLOW TRUE
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS
        -DYAML_CPP_BUILD_TESTS:BOOL=OFF
        -DYAML_BUILD_SHARED_LIBS:BOOL=ON
)

ExternalProject_Get_Property( ${YAML_CPP}_src binary_dir )
ExternalProject_Get_Property( ${YAML_CPP}_src source_dir )
SET( YAML_CPP_INCLUDE_DIRS ${source_dir}/include )
SET( YAML_CPP_LIBRARIES ${binary_dir}/lib${YAML_CPP}${CMAKE_SHARED_LIBRARY_SUFFIX} )