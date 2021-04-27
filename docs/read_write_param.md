# Reading and Writing of Parameters
Parameters are read from (or written to) TOML files, they are accessed by an identifier which is the combination of the `default_input_namespace`(or `default_output_namespace`) and the `data_product` key which is the relative address in the data store.

For the examples below, the `test` directory within the source repository is used.

## Example: Reading

```c++
#include "scrc/fdp.hxx"

using namespace SCRC;

int main() {

    const std::filesystem::path config_file = "test/config.yaml";

    DataPipeline* fdp = new DataPipeline;

    const double demo_param = fdp->read_point_estimate("demo_val/param");

    return 0;
}
```
