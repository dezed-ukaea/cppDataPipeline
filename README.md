# FAIR Data Pipeline C++ API

**NOTE:** Currently this API implementation consists of components, assembly of these will depend on the final structure of the API itself.

## Contents
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
  - [Installation](#installation)
  - [Unit Tests](#unit-tests)

## Outline
The main class the user will interact with is `DataPipeline` which has only the required methods such as `read_estimate` etc. This class has a member which is a pointer to an underlying `DataPipelineImpl_` class which performs the various procedures required to handle the data. A logger has been used to give as much feedback to the user as possible, the verbosity being handled by a log level argument.

Development has been made mainly to this internal class, the idea being that the top layer API will be constructed once the schema/workflow for FAIR Data Pipeline is finalised.

## API Backend Components
The included unit tests show the API in action testing various methods for `DataPipelineImpl_`, `LocalFileSystem` and the extra utility methods required.

### API
The core of the framework is the `API` class which sends and receives information to the RestAPI via CURL. It has no specific knowledge about the pipeline itself.

```c++
#include "fdp/registry/api.hxx"
#include "json/json.h"

#include <string>
#include <filesystem>

using namespace FDP;

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


#### Reading an Estimate
```c++
#include "fdp/registry/file_system.hxx"

#include <filesystem>

using namespace FDP;

void read_param_demo() {
    const std::filesystem::path toml_file = "/path/to/estimate/0.1.0.toml";
    const double value = read_point_estimate_from_toml(toml_file);
}
```

#### Reading an Array
```c++
#include "fdp/registry/file_system.hxx"
#include "fdp/objects/data.hxx"

#include <filesystem>

using namespace FDP;

void read_array_demo() {
    const std::filesystem::path array_file = "path/to/array/0.20210427.0.h5";
    const std::filesystem::path component = "test_array";
    const ArrayObject<float>* read_array(array_file, component);
}
```

#### Reading a Data Table Column
```c++
#include "fdp/registry/file_system.hxx"
#include "fdp/objects/data.hxx"

#include <string>
#include <filesystem>

using namespace FDP;

void read_table_col_demo() {
    const std::filesystem::path array_file = "path/to/array/0.20210427.0.h5";
    const std::filesystem::path component = "test_table";
    const std::string column = "col_1";
    const DataTableColumn<float>* read_table_column(array_file, component, column);
}
```

#### Reading a Distribution
```c++
#include "fdp/registry/file_system.hxx"
#include "fdp/objects/distributions.hxx"

#include <filesystem>
#include <random>

using namespace FDP;

void read_distribution_demo() {
    const std::filesystem::path toml_file = "/path/to/distribution/0.1.0.toml";
    const Distribution* dis = read_distribution_from_toml(toml_file);

    std::random_device rd;
    std::mt19937* gen = new std::mt19937(rd());
    const double random_val = dis->generate(gen);
}
```

#### Creating an Estimate
```c++
#include "fdp/registry/file_system.hxx"
#include "fdp/utilities/semver.hxx"

#include <string>
#include <filesystem>

using namespace FDP;

void create_param_demo() {
    const std::filesystem::path config_file = "path/to/config.yaml";
    LocalFileSystem* file_system = new LocalFileSystem(config_file);

    const version v1(0, 2, 0);
    const double value = 7;
    const std::string label = "my_params/param_a";

    create_estimate(value, label, v1, file_system);
}
```
#### Creating a Distribution

```c++
#include "fdp/registry/file_system.hxx"
#include "fdp/utilities/semver.hxx"

#include <string>
#include <filesystem>

using namespace FDP;

void create_dis_demo() {
    const std::filesystem::path config_file = "path/to/config.yaml";
    LocalFileSystem* file_system = new LocalFileSystem(config_file);

    const version v1(0, 2, 0);
    const std::string label = "my_distributions/dis_a";

    const double mu = 5.2;
    const double sigma = 1.2;

    Normal* dis = new Normal(mu, sigma);

    create_distribution(dis, label, v1, file_system);
}
```

#### Creating an Array
```c++
#include "fdp/registry/file_system.hxx"
#include "fdp/objects/data.hxx"

#include <string>
#include <filesystem>

using namespace FDP;

void create_array_demo() {
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

    create_array(arr, data_product, component, file_system);
}
```

## Installation
You can build and test the library using CMake, this implementation requires C++17 or later as it makes use of the `std::filesystem` library. All dependencies are externally fetched however it is recommended that you install the following in advance:

- [HDF5](http://www.hdfgroup.org/ftp/HDF5/current/src/)
- [CURL](https://curl.se/libcurl/)

compile the library and tests by running:

```
$ cmake -Bbuild -DFDPAPI_BUILD_TESTS=ON
$ cmake --build build
```

## Unit Tests
The unit tests connect to the [FDP remote registry API](https://data.scrc.uk), and so will not work if this service is no longer available.

Before running the tests you will need to download the test data sets by running:

```
$ bash test/test_setup.sh
```
you can then launch the GTest binary created during compilation:
```
$ ./build/test/fdpapi-tests
```