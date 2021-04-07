#ifndef __SCRC_FILESYSTEM_HXX__
#define __SCRC_FILESYSTEM_HXX__

#include <string>
#include <vector>
#include <filesystem>
#include <semver.hpp>
#include <yaml-cpp/yaml.h>
#include <cstdlib>

#include "scrc/utilities/doi.hxx"
#include "scrc/utilities/logging.hxx"

namespace SCRC
{
    YAML::Node parse_config_();

    class DataProduct
    {
        private:
            const std::string namespace_;
            const semver::version version_;
            const std::filesystem::path registry_path_;
            std::filesystem::path cache_ = std::filesystem::path();
            std::string title_ = "";
        public:
            DataProduct(std::string registry_path, std::string version) :
                registry_path_(registry_path),
                version_(version)
            {}
            std::filesystem::path get_path() const {return registry_path_;}
            semver::version get_version() const {return version_;}
            std::string get_title() const {return title_;}
            std::string get_cache_path() const {return cache_;}
    };

    class ExternalObject
    {
        private:
            const DOI doi_;
            const std::string title_ = "";
            std::filesystem::path cache_location_ = std::filesystem::path();
        public:
            ExternalObject(DOI doi, std::string title="") :
                doi_(doi),
                title_(title)
            {}
    };

    class LocalFileSystem
    {
        private:
            const std::filesystem::path config_path_;
            const YAML::Node config_data_;
            YAML::Node meta_data_() const;
        public:
            LocalFileSystem(std::filesystem::path config_file_path);
            std::filesystem::path get_data_store() const;
            std::vector<DataProduct*> read_data_products() const;
            std::string get_default_input_namespace() const;
            std::string get_default_output_namespace() const;
    };
};

#endif