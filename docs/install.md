# Installation

The C++ API is built via CMake which in addition handles the requirements, and requires C++17 or later. Note however that not all scenarios have been tested with regard to the absence of certain libraries from the system. It is therefore recommended that you install the following prior to compilation:

- [HDF5](http://www.hdfgroup.org/ftp/HDF5/current/src/)
- [CURL](https://curl.se/libcurl/)

To build the library run CMake on the repository:

```
$ cmake -Bbuild
$ cmake --build build
```
or you can optionally add the repository as an external dependency to your own project:
```cmake
# Project CMakeLists.txt
INCLUDE( ExternalProject )

ExternalProject_Add(
  scrcapi_src
  GIT_REPOSITORY https://github.com/ScottishCovidResponse/SCRC-Cpp-API.git
  CONFIGURE_COMMAND ""
  GIT_SHALLOW TRUE
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property( scrcapi_src source_dir )
SET( SCRCAPI_INCLUDE_DIRS ${source_dir}/include )

SET( SCRCAPI scrcapi )
ADD_LIBRARY( ${SCRCAPI} INTERFACE )
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} INTERFACE ${SCRCAPI_INCLUDE_DIRS} )
```