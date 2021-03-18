FIND_PACKAGE( yaml-cpp 0.6.0 )

IF(NOT ${YAML_CPP_INCLUDE_DIRS})
    ExternalProject_Add(
        yaml_src
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-0.6.3
        GIT_SHALLOW TRUE
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property( yaml_src binary_dir )
    ExternalProject_Get_Property( yaml_src source_dir )
    SET( YAML_CPP_INCLUDE_DIRS ${source_dir}/include )
    SET( YAML_CPP_LIBRARIES ${binary_dir}/lib/libyaml${CMAKE_SHARED_LIBRARY_SUFFIX} )
ENDIF()