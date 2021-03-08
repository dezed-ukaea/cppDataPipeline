ExternalProject_Add(
  semver_src
  GIT_REPOSITORY https://github.com/Neargye/semver.git
  GIT_TAG v0.3.0
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  GIT_SHALLOW TRUE
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property( semver_src source_dir )
SET( SEMVER_INCLUDE_DIRS ${source_dir}/include )

SET( SEMVER semver )
ADD_LIBRARY( ${SEMVER} INTERFACE )
TARGET_INCLUDE_DIRECTORIES( ${SEMVER} INTERFACE ${SEMVER_INCLUDE_DIRS} )