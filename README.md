# FAIR Data Pipeline C++ API

[![FDP C++ API](https://github.com/FAIRDataPipeline/cppDataPipeline/actions/workflows/fdp_cpp_api.yaml/badge.svg)](https://github.com/FAIRDataPipeline/cppDataPipeline/actions/workflows/fdp_cpp_api.yaml)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)
[![software-checklist][https://img.shields.io/badge/software-checklist-3891A6]][https://github.com/FAIRDataPipeline/cppDataPipeline/blob/main/software-checklist.md]
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5874670.svg)](https://doi.org/10.5281/zenodo.5874670)

## Contents
  - [Installation](#installation)
  - [Outline](#outline)
  - [Unit Tests](#unit-tests)

## Installation
You can build and test the library using CMake, this implementation requires `C++11`.
All dependencies are externally fetched except HDF5 which needs to be installed and `HDF5_ROOT` present as an environmental variable:
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
The unit tests use the local registry, this needs to be running prior to running the tests see: [the CLI documentation](https://github.com/FAIRDataPipeline/FAIR-CLI#registry)

Tests can be lauched using the following command:
### Unix
```
$ ./build/bin/fdpapi-tests
```

### Windows
```
$ build\bin\Release\fdpapi-tests.exe
```