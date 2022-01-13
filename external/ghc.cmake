MESSAGE( STATUS "[GHC]" )

set(GHC_URL "https://github.com/gulrak/filesystem/archive/refs/tags/v1.5.10.zip")

MESSAGE( STATUS "\tGHC Will be installed." )
MESSAGE( STATUS "\tURL: ${GHC_URL}" )

include(FetchContent)
FetchContent_Declare(
    GHC
    URL ${GHC_URL}
)
FetchContent_MakeAvailable(GHC)