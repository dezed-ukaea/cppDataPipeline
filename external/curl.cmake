FIND_PACKAGE( CURL )

IF(NOT CURL_FOUND)
    ExternalProject_Add(curl_src
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-7_75_0
        GIT_SHALLOW TRUE
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property( curl_src source_dir )
    ExternalProject_Get_Property( curl_src binary_dir )
    SET( CURL_INCLUDE_DIRS ${source_dir}/include )
    SET( CURL_LIBRARIES ${binary_dir}/lib/libcurl${CMAKE_SHARED_LIBRARY_SUFFIX} )
ENDIF()