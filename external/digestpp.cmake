SET( DIGESTCPP_URL "https://github.com/kerukuro/digestpp.git" )
SET( DIGESTCPP_COMMIT "34ff2eeae397ed744d972d86b5a20f603b029fbd" )


# windows.h will conflict with min functions in digestcpp
# Because of macro definitions of min and max
# So tell the compiler to exclude min and max macros in windows
IF(WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-DNOGDI)
ENDIF()

MESSAGE( STATUS "[DigestCPP]" )

MESSAGE( STATUS "\tDigestCpp Will be installed." )
MESSAGE( STATUS "\tURL: ${DIGESTCPP_URL}" )
MESSAGE( STATUS "\tCOMMIT HASH: ${DIGESTCPP_COMMIT}" )

include(FetchContent)
FetchContent_Declare(
    DIGESTPP
    GIT_REPOSITORY ${DIGESTCPP_URL}
    GIT_TAG ${DIGESTCPP_COMMIT}
)
FetchContent_MakeAvailable(DIGESTPP)

ADD_LIBRARY( digestpp INTERFACE )
TARGET_INCLUDE_DIRECTORIES( digestpp INTERFACE ${digestpp_SOURCE_DIR} )