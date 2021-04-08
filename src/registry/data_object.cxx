#include "scrc/registry/data_object.hxx"

namespace SCRC
{
    namespace ReadObject
    {
        DataProduct* data_product_from_yaml(YAML::Node yaml_data)
        {
            APILogger->debug("ReadObject: Extracting entry metadata.");
            const YAML::Node use_node_ = yaml_data["use"];

            if(!use_node_)
            {
                std::string available_keys_ = "";
                throw config_parsing_error("Expected 'use' key in data product entry, but none found");
            }

            const std::string version_ = use_node_["version"].as<std::string>();
            const std::string file_path_ = yaml_data["data_product"].as<std::string>();

            return new DataProduct(file_path_, version_);
        }

        ExternalObject* external_object_from_yaml(YAML::Node yaml_data)
        {
            const YAML::Node use_node_ = yaml_data["use"];
            const DOI version_ = doi_from_string(use_node_["doi"].as<std::string>());
            const std::string title_ = use_node_["title"].as<std::string>();

            const std::filesystem::path cache_path_ = (use_node_["cache"]) ? use_node_["cache"].as<std::string>() : "";

            return new ExternalObject(title_, version_, cache_path_);
        }
    };
};