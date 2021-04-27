# SCRC FAIR Data Pipeline C++ API

Currently this API implementation consists of components, assembly of these will depend on the final structure of the API itself.

## Outline
The main class the user will interact with is `DataPipeline` which has only the required methods such as `read_estimate` etc. This class has a member which is a pointer to an underlying `DataPipelineImpl_` class which performs the various procedures required to handle the data. A logger has been used to give as much feedback to the user as possible, the verbosity being handled by a log level argument.

Development has been made mainly to this internal class, the idea being that the top layer API will be constructed once the schema/workflow for SCRC FDP is finalised.

## "Under the Hood"
The included unit tests show the API in action testing various methods for `DataPipelineImpl_`, `LocalFileSystem` and the extra utility methods required.

### API
The core of the framework is the `API` class which sends and receives information to the RestAPI via CURL. It has no specific knowledge about the pipeline itself.

```c++
#include "scrc/registry/api.hxx"
#include "json/json.h"

#include <string>
#include <filesystem>

using namespace SCRC;

void run_api() {
    API* api = new API("https://data.scrc.uk/api/");

    Json::Value data;
    data["website"] = "www.nosuchsite.uk";
    data["name"] = "test_data";
    data["abbreviation"] = "nss";

    const std::string api_token = "APITOKEN";

    api->request("object/1234");
    api->post("source", data, api_token);
}
```

### LocalFileSystem
This largely handles the translation of statements in the configuration file into the retrieval of or submission of objects in the registry. It includes most of the HDF5 reading methods.

#### Creating an Estimate
```c++
#include "scrc/registry/file_system.hxx"
#include "scrc/utilities/semver.hxx"

#include <string>
#include <filesystem>

using namespace SCRC;

void create_param() {
    const std::filesystem::path config_file = "path/to/config.yaml";
    LocalFileSystem* file_system = new LocalFileSystem(config_file);

    const version v1(0, 2, 0);
    const double value = 7;
    const std::string label = "my_params/param_a";

    create_estimate(value, label, v1, file_system);
}
```

#### Creating an Array
```c++
#include "scrc/registry/file_system.hxx"
#include "scrc/objects/data.hxx"

#include <string>
#include <filesystem>

using namespace SCRC;

void create_array() {
    const std::filesystem::path config_file = "path/to/config.yaml";
    LocalFileSystem* file_system = new LocalFileSystem(config_file);

    const std::filesystem::path data_product = "test_data/test_arrays";
    const std::filesystem::path component = "demo_1";

    const ArrayObject<float>* arr = new ArrayObject<float>(
        {"dimension_1", "dimension_2"},             // Dimension titles
        {{"a1", "a2", "a3"}, {"b1", "b2", "b3"}},   // Dimension names
        {3, 3},                                     // Dimension sizes
        {1., 2., 3., 4., 5., 6., 7., 8., 9.}
    );

    create_estimate(arr, data_product, component, file_system);
}
```

## Installation
You can build and test the library using CMake. All dependencies are externally fetched however it is recommended that you install the following in advance:

- [HDF5](http://www.hdfgroup.org/ftp/HDF5/current/src/)
- [CURL](https://curl.se/libcurl/)

compile the library and tests by running:

```
$ cmake -Bbuild -DSCRCAPI_BUILD_TESTS=ON
$ cmake --build build
```

## Unit Tests
The unit tests connect to the [SCRC remote registry API](https://data.scrc.uk), and so will not work if this service is no longer available.

Before running the tests you will need to download the test data sets by running:

```
$ bash test/test_setup.sh
```
you can then launch the GTest binary created during compilation:
```
$ ./build/test/scrcapi-tests
```