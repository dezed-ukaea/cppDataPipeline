SET( TOML11_URL "https://github.com/ToruNiina/toml11/archive/refs/tags/v3.7.0.zip" )
MESSAGE( STATUS "[TOML 11]" )

MESSAGE( STATUS "\tTOML 11 Will be installed." )
MESSAGE( STATUS "\tURL: ${TOML11_URL}" )

include(FetchContent)
FetchContent_Declare(
    TOMLCPP
    URL ${TOML11_URL}
)
FetchContent_MakeAvailable(TOMLCPP)