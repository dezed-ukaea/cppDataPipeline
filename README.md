# FAIR Data Pipeline C++ API

**NOTE:** Currently this API implementation consists of components, assembly of these will depend on the final structure of the API itself.

## Contents
  - [Installation](#installation)
  - [Outline](#outline)
  - [API Backend Components](#api-backend-components)
    - [API](#api)
    - [LocalFileSystem](#localfilesystem)
      - [Reading an Estimate](#reading-an-estimate)
      - [Reading an Array](#reading-an-array)
      - [Reading a Data Table Column](#reading-a-data-table-column)
      - [Reading a Distribution](#reading-a-distribution)
      - [Creating an Estimate](#creating-an-estimate)
      - [Creating a Distribution](#creating-a-distribution)
      - [Creating an Array](#creating-an-array)
  - [Unit Tests](#unit-tests)

## Installation
You can build and test the library using CMake, this implementation requires C++17 or later as it makes use of the `ghc::filesystem` library. 
All dependencies are externally fetched except HDF5 which needs to be installed and HDF5_ROOT present as an environmental variable:
- [HDF5](http://www.hdfgroup.org/ftp/HDF5/current/src/)

It is recomended that you install CURL prior to installation of this API
- [CURL](https://curl.se/libcurl/)

compile the library and tests by running:

```
$ cmake -Bbuild -DFDPAPI_BUILD_TESTS=ON
$ cmake --build build
```

**Important** For multi-config compilers such as Visual Studio xcode or ninja the config type needs to be set to `Release` otherwise the API will not build for example:
```
$ cmake -Bbuild -DFDPAPI_BUILD_TESTS=ON
$ cmake --build build --config=Release
```

## Outline
The main class the user will interact with is `DataPipeline` which has only the required methods such as `link_read` etc. This class has a member which is a pointer to an underlying `DataPipelineImpl_` class which performs the various procedures required to handle the data. A logger has been used to give as much feedback to the user as possible, the verbosity being handled by a log level argument.


## Unit Tests
The unit tests connect to the [FDP remote registry API](https://data.scrc.uk), and so will not work if this service is no longer available.

Before running the tests you will need to download the test data sets by running:

```
$ bash test/test_setup.sh
```
you can then launch the GTest binary created during compilation:
```
$ ./build/bin/fdpapi-tests
```