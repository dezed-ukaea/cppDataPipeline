SET( JSONCPP_URL "https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.5.zip" )

MESSAGE( STATUS "[JsonCPP]" )
MESSAGE( STATUS "\tJsonCpp Will be installed." )
MESSAGE( STATUS "\tURL: ${JSONCPP_URL}" )

SET (JSONCPP_WITH_TESTS OFF CACHE INTERNAL "Don't Build cpp tests")

include(FetchContent)
FetchContent_Declare(
    JsonCpp
    URL ${JSONCPP_URL}
)
FetchContent_MakeAvailable(JsonCpp)
