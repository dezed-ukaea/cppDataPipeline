#ifndef __SCRC_FILESYSTEM_HXX__
#define __SCRC_FILESYSTEM_HXX__

#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <yaml-cpp/yaml.h>
#include <cstdlib>
#include <toml.hpp>

#include "scrc/utilities/logging.hxx"
#include "scrc/registry/data_object.hxx"
#include "scrc/objects/distributions.hxx"

namespace SCRC
{
    YAML::Node parse_config_();

    class LocalFileSystem
    {
        private:
            const std::filesystem::path config_path_;
            const YAML::Node config_data_;
            YAML::Node meta_data_() const;
        public:
            LocalFileSystem(std::filesystem::path config_file_path);
            std::filesystem::path get_data_store() const;
            std::vector<ReadObject::DataProduct*> read_data_products() const;
            std::string get_default_input_namespace() const;
            std::string get_default_output_namespace() const;

    };

    std::string get_first_key_(const toml::value data_table);
    Distribution* construct_dis_(const toml::value data_table);

    double read_point_estimate(const std::filesystem::path var_address);
    Distribution* read_distribution(const std::filesystem::path var_address);

    enum FILE_TYPE
    {
        TOML,
        HDF5,
        OTHER
    };
};

#endif