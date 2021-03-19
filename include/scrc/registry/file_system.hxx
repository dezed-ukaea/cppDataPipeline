#ifndef __SCRC_FILESYSTEM_HXX__
#define __SCRC_FILESYSTEM_HXX__

#include <string>
#include <filesystem>
#include <semver.hpp>
#include <yaml-cpp/yaml.h>

namespace SCRC
{
    YAML::Node parse_config_();

    class MetadataSubset
    {
        private:
            const semver::version version_;
            const std::filesystem::path data_product_;
        public:
            MetadataSubset(std::string data_product, std::string version) :
                data_product_(data_product),
                version_(version)
            {}
            semver::version get_version() const {return version_;}
            std::filesystem::path get_path() const {return data_product_;}
                
    };

    class LocalFileSystem
    {
        private:
            const std::filesystem::path config_path_;
            const YAML::Node config_data_;
        public:
            LocalFileSystem(std::filesystem::path config_file_path);
            std::string get_namespace() const;
            std::vector<MetadataSubset*> get_data_products() const;
    };
};

#endif