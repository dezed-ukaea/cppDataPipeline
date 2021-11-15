# Try and find CURL
FIND_PACKAGE( CURL QUIET )
MESSAGE( STATUS "[Curl]" )

# If CURL is not Found, Install it
IF(NOT CURL_FOUND)

    SET( CURL_URL "https://github.com/curl/curl/archive/refs/tags/curl-7_80_0.zip" )

    MESSAGE( STATUS "\tCURL Will be installed." )
    MESSAGE( STATUS "\tURL: ${CURL_URL}" )
    include(FetchContent)
    FetchContent_Declare(
        CURL
        URL ${CURL_URL}
    )
    FetchContent_MakeAvailable(CURL)
    SET(CURL_INCLUDE_DIRS = ${curl_SOURCE_DIR}/include)
    SET(CURL_LIBRARIES libcurl)
ELSE()
    MESSAGE( STATUS "\tInclude Directory: ${CURL_INCLUDE_DIRS}" )
    MESSAGE( STATUS "\tLibraries: ${CURL_LIBRARIES}" )
ENDIF()