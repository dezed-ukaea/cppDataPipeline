FIND_PACKAGE( CURL QUIET )

MESSAGE( STATUS "[Curl]" )

SET( CURL_URL "https://github.com/curl/curl.git" )
SET( CURL_TAG "curl-7_75_0" )

IF(NOT CURL_FOUND)
    MESSAGE( STATUS "\tCurl not installed on this system and so will be downloaded." )
    MESSAGE( STATUS "\tURL: ${CURL_URL}" )
    MESSAGE( STATUS "\tTAG: ${CURL_TAG}" )
    ExternalProject_Add(curl_src
        GIT_REPOSITORY ${CURL_URL}
        GIT_TAG ${CURL_TAG}
        GIT_SHALLOW TRUE
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property( curl_src source_dir )
    ExternalProject_Get_Property( curl_src binary_dir )
    SET( CURL_INCLUDE_DIRS ${source_dir}/include )
    SET( CURL_LIBRARIES ${binary_dir}/lib/libcurl${CMAKE_SHARED_LIBRARY_SUFFIX} )
ELSE()
    MESSAGE( STATUS "\tInclude Directory: ${CURL_INCLUDE_DIRS}" )
    MESSAGE( STATUS "\tLibraries: ${CURL_LIBRARIES}" )
ENDIF()