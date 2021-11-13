FIND_PACKAGE( JsonCpp QUIET )

SET( JSONCPP_URL "https://github.com/open-source-parsers/jsoncpp.git" )
SET( JSONCPP_TAG "1.9.4" )

MESSAGE( STATUS "[JsonCPP]" )

IF( NOT JsonCpp_INCLUDE_DIRS )
    MESSAGE( STATUS "\tJsonCpp not installed on this system and so will be downloaded." )
    MESSAGE( STATUS "\tURL: ${JSONCPP_URL}" )
    MESSAGE( STATUS "\tTAG: ${JSONCPP_TAG}" )
    ExternalProject_Add(jsoncpp_src
        GIT_REPOSITORY ${JSONCPP_URL}
        GIT_TAG ${JSONCPP_TAG}
        GIT_SHALLOW TRUE
        CMAKE_CACHE_ARGS
            -DJSONCPP_WITH_TESTS:BOOL=OFF
            -DJSONCPP_WITH_EXAMPLE:BOOL=OFF
            -DJSONCPP_WITH_CMAKE_PACKAGE:BOOL=ON
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property( jsoncpp_src binary_dir )
    ExternalProject_Get_Property( jsoncpp_src source_dir )
    SET( JsonCpp_INCLUDE_DIRS ${source_dir}/include )
    IF ( WIN32)
        SET( JsonCpp_LIBRARIES ${binary_dir}/lib/${CMAKE_BUILD_TYPE}/jsoncpp${CMAKE_STATIC_LIBRARY_SUFFIX} )
    ELSE()
        SET( JsonCpp_LIBRARIES ${binary_dir}/lib/libjsoncpp${CMAKE_SHARED_LIBRARY_SUFFIX} )
    ENDIF()
ELSE()
    MESSAGE( STATUS "\tInclude Directory: ${JsonCpp_INCLUDE_DIRS}" )
    MESSAGE( STATUS "\tLibraries: ${JsonCpp_LIBRARIES}" )
ENDIF()