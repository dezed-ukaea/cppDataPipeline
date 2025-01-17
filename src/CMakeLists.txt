# Find and add the .cxx files to SRC_FILES
FILE( GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx)

IF(WIN32)
    list(APPEND SRC_FILES ${FDPAPI_INCLUDE_DIRS}/windows_sys/time.cpp )
ENDIF()

# Add the (Static) Project Library using SRC_FILES
ADD_LIBRARY( ${FDPAPI} STATIC ${SRC_FILES} )

# Set libraries compiled in debug mode to end in 'd'
SET_TARGET_PROPERTIES( ${FDPAPI} PROPERTIES DEBUG_POSTFIX "d" )

# Set the public header
SET_TARGET_PROPERTIES( ${FDPAPI} PROPERTIES PUBLIC_HEADER "${FDPAPI_CODE_COVERAGE}" )

# Add include directories for Package Headers
TARGET_INCLUDE_DIRECTORIES( ${FDPAPI} PRIVATE ${FDPAPI_INCLUDE_DIRS} )
TARGET_INCLUDE_DIRECTORIES( ${FDPAPI} PUBLIC ${jsoncpp_SOURCE_DIR}/include )
TARGET_INCLUDE_DIRECTORIES( ${FDPAPI} PUBLIC ${yamlcpp_SOURCE_DIR}/include )
TARGET_INCLUDE_DIRECTORIES( ${FDPAPI} PUBLIC ${CURL_INCLUDE_DIRS} )
TARGET_INCLUDE_DIRECTORIES( ${FDPAPI} PUBLIC ${digestpp_SOURCE_DIR} )
TARGET_INCLUDE_DIRECTORIES( ${FDPAPI} PUBLIC ${ghc_SOURCE_DIR}/include )
# Toml 11 is a header only library no need to link
TARGET_INCLUDE_DIRECTORIES( ${FDPAPI} PUBLIC ${tomlcpp_SOURCE_DIR} )

# Link the package libraries - Packages will be automatically found during fetch_content
TARGET_LINK_LIBRARIES( ${FDPAPI} PUBLIC jsoncpp_static )
TARGET_LINK_LIBRARIES( ${FDPAPI} PUBLIC digestpp )  
TARGET_LINK_LIBRARIES( ${FDPAPI} PUBLIC yaml-cpp )
TARGET_LINK_LIBRARIES( ${FDPAPI} PUBLIC ghc_filesystem )
TARGET_LINK_LIBRARIES( ${FDPAPI} PUBLIC ${CURL_LIBRARIES} )

# Install the libraries
INSTALL( TARGETS ${FDPAPI} 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)