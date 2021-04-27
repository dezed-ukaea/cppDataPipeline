# SCRC FAIR Data Pipeline C++ API
This library provides an interface for the Scottish COVID Response Consortium's FAIR Data Pipeline which is a data version control and metadata tracking system.

## Features

The interaction with data is based on a user defined configuration file written in YAML, this allows the system to:

- Read/write N dimensional arrays from/to HDF5 files along with metadata such as axis labels and titles.
- Read/write point estimates from/to TOML files.
- Read/write distributions as parameters from/to TOML files.
- Register sources to the pipeline.
