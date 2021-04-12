ExternalProject_Add(toml11_src
    GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
    GIT_TAG v3.6.1
    GIT_SHALLOW TRUE
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property( toml11_src source_dir )
SET( TOML11_INCLUDE_DIRS ${source_dir} )