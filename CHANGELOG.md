# Unreleased
- Added functions for reading requested objects in the configuration file.
- Reading of configuration YAML file using `yaml-cpp`.
- Reading and writing of TOML files to represent parameters and distributions using `toml11`.
- Reading and writing of arrays to/from a HDF5 file.
- Hashing of file names and file content via SHA1.
- Semantic versioning via a dedicated `version` class.
- Added ability to talk to the RestAPI via the CURL library.
- Use of `spdlog` as a logging library for plenty of output during debugging.