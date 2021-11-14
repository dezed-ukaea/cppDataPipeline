MESSAGE( STATUS "[SPD Log]" )

set(SPDLOG_URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.9.2.zip")

MESSAGE( STATUS "\tSPD Will be installed." )
MESSAGE( STATUS "\tURL: ${SPDLOG_URL}" )

include(FetchContent)
FetchContent_Declare(
    SPDLOG
    URL ${SPDLOG_URL}
)
FetchContent_MakeAvailable(SPDLOG)