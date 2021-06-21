SET( DIGESTCPP_URL "https://github.com/kerukuro/digestpp.git" )
SET( DIGESTCPP_COMMIT "34ff2eeae397ed744d972d86b5a20f603b029fbd" )

MESSAGE( STATUS "[DigestCPP]" )

MESSAGE( STATUS "\tDigestCpp not installed on this system and so will be downloaded." )
MESSAGE( STATUS "\tURL: ${DIGESTCPP_URL}" )
MESSAGE( STATUS "\tCOMMIT HASH: ${DIGESTCPP_COMMIT}" )

ExternalProject_Add(
  digestpp_src
  GIT_REPOSITORY ${DIGESTCPP_URL}
  GIT_TAG ${DIGESTCPP_COMMIT}
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