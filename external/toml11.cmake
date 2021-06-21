SET( TOML11_URL "https://github.com/ToruNiina/toml11.git" )
SET( TOML11_COMMIT "v3.6.1" )

MESSAGE( STATUS "[TOML 11]" )

MESSAGE( STATUS "\tTOML 11 not installed on this system and so will be downloaded." )
MESSAGE( STATUS "\tURL: ${TOML11_URL}" )
MESSAGE( STATUS "\tTAG: ${TOML11_COMMIT}" )

ExternalProject_Add(toml11_src
    GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
    GIT_TAG v3.6.1
    GIT_SHALLOW TRUE
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property( toml11_src source_dir )
SET( TOML11_INCLUDE_DIRS ${source_dir} )