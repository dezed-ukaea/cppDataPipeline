ExternalProject_Add(
  digestpp_src
  GIT_REPOSITORY https://github.com/kerukuro/digestpp.git
  GIT_TAG 34ff2eeae397ed744d972d86b5a20f603b029fbd
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  GIT_SHALLOW TRUE
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property( digestpp_src source_dir )
SET( DIGESTPP_INCLUDE_DIRS ${source_dir} )

SET( DIGESTPP digestpp )
ADD_LIBRARY( ${DIGESTPP} INTERFACE )
TARGET_INCLUDE_DIRECTORIES( ${DIGESTPP} INTERFACE ${DIGESTPP_INCLUDE_DIRS} )