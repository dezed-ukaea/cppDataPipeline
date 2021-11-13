SET( YAML_CPP "yaml-cpp" )
find_package( yaml-cpp QUIET )
SET( YAMLCPP_TAG "yaml-cpp-0.6.3" )
SET( YAMLCPP_URL "https://github.com/jbeder/yaml-cpp.git" )

MESSAGE( STATUS "[YAML-cpp]" )

IF( NOT YAMLCPP_INCLUDE_DIR )
    MESSAGE( STATUS "\tYAML-cpp not installed on this system and so will be downloaded." )
    MESSAGE( STATUS "\tURL: ${YAMLCPP_URL}" )
    MESSAGE( STATUS "\tTAG: ${YAMLCPP_TAG}" )
    ExternalProject_Add(
        ${YAML_CPP}_src
        GIT_REPOSITORY ${YAMLCPP_URL}
        GIT_TAG ${YAMLCPP_TAG}
        GIT_SHALLOW TRUE
        INSTALL_COMMAND ""
        CMAKE_CACHE_ARGS
            -DYAML_CPP_BUILD_TESTS:BOOL=OFF
            -DYAML_BUILD_SHARED_LIBS:BOOL=ON
    )

    ExternalProject_Get_Property( ${YAML_CPP}_src binary_dir )
    ExternalProject_Get_Property( ${YAML_CPP}_src source_dir )
    SET( YAMLCPP_INCLUDE_DIR ${source_dir}/include )
    IF ( WIN32)
        SET( YAMLCPP_LIBRARY ${binary_dir}/${CMAKE_BUILD_TYPE}/${YAML_CPP}${CMAKE_STATIC_LIBRARY_SUFFIX} )
    ELSE()
        SET( YAMLCPP_LIBRARY ${binary_dir}/lib${YAML_CPP}${CMAKE_SHARED_LIBRARY_SUFFIX} )
    ENDIF()
ELSE()
    MESSAGE( STATUS "\tInclude Directory: ${YAMLCPP_INCLUDE_DIR}" )
    MESSAGE( STATUS "\tLibraries: ${YAMLCPP_LIBRARY}" )
ENDIF()