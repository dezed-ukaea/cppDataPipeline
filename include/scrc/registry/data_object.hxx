#ifndef __SCRC_DATAOBJECT_HXX__
#define __SCRC_DATAOBJECT_HXX__

#include <exception>
#include <filesystem>
#include <ctime>
#include <yaml-cpp/yaml.h>

#include "scrc/utilities/doi.hxx"
#include "scrc/utilities/logging.hxx"
#include "scrc/utilities/semver.hxx"
#include "scrc/exceptions.hxx"

namespace SCRC
{
    namespace ReadObject
    {
        class DataProduct
        {
            private:
                const std::string namespace_;
                const version version_;
                const std::filesystem::path registry_path_;
                const std::filesystem::path cache_ = std::filesystem::path();
                const std::string title_ = "";
            public:
                DataProduct(
                    std::string registry_path,
                    std::string version,
                    std::string name_space="",
                    std::string title="",
                    std::filesystem::path cache_path=""
                ) :
                    registry_path_(registry_path),
                    version_(version),
                    namespace_(name_space),
                    title_(title),
                    cache_(cache_path)
                {}
                std::filesystem::path get_path() const {return registry_path_;}
                version get_version() const {return version_;}
                std::string get_namespace() const {return namespace_;}
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
                ExternalObject(std::string title="", DOI doi=DOI(), std::filesystem::path cache_path="") :
                    title_(title),
                    doi_(doi),
                    cache_location_(cache_path)
                {}
        };

        DataProduct* data_product_from_yaml(YAML::Node yaml_data);
        ExternalObject* external_object_from_yaml(YAML::Node yaml_data);
    };

    namespace RegisterObject
    {
        enum Accessibility
        {
            OPEN,
            CLOSED
        };

        class ExternalObject
        {
            private:
                const std::string source_name_ = "";
                const std::string source_abbreviation_ = "";
                const std::string source_website_ = "";

                const std::string root_name_ = "";
                const std::string root_url_ = "";

                const std::string title_ = "";
                const std::string description_ = "";
                const std::string unique_name_ = "";
                const std::filesystem::path product_name_ = "";
                const std::string file_type_ = "";
                const tm release_date_;
                const version version_ = version();
                const bool primary_ = true;
                const Accessibility accessibility_ = Accessibility::OPEN;
            public:
                ExternalObject(YAML::Node yaml_data);
        };
    };

    namespace WriteObject
    {
        class DataProduct
        {
            private:
                const std::filesystem::path registry_path_;
                const std::string component_;
        };
    };
};

#endif