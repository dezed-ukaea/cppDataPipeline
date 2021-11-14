SET( YAMLCPP_URL "https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.zip" )

MESSAGE( STATUS "[YAML-cpp]" )
MESSAGE( STATUS "\tYAML-cpp Will be installed." )
MESSAGE( STATUS "\tURL: ${YAMLCPP_URL}" )

include(FetchContent)
FetchContent_Declare(
    YAMLCPP
    URL ${YAMLCPP_URL}
)
FetchContent_MakeAvailable(YAMLCPP)
