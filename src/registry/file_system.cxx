#include "scrc/registry/file_system.hxx"

#include <iostream>

namespace SCRC
{
    YAML::Node parse_yaml_(std::filesystem::path yaml_path)
    {
        return YAML::LoadFile(yaml_path.c_str());
    }

    LocalFileSystem::LocalFileSystem(std::filesystem::path config_file_path) :
        config_path_(config_file_path),
        config_data_(parse_yaml_(config_file_path))
    {

        std::filesystem::path data_directory_path_;

        try
        {
            data_directory_path_ = config_data_["data_directory"].as<std::string>();
        }
        catch(const YAML::BadConversion& e)
        {
            throw std::runtime_error(
                "Failed to retrieve value for 'data_directory' key from config,"
                "either key does not exist, or value could not be converted to string"
            );
        }
        const std::filesystem::path config_loc_dir_(config_path_.parent_path());
        
        // Create the data directory within the config file id it does not exist
        std::filesystem::create_directory((config_loc_dir_ / data_directory_path_).c_str());

    }

    std::string LocalFileSystem::get_namespace() const
    {
        return config_data_["namespace"].as<std::string>();
    }

    std::vector<MetadataSubset*> LocalFileSystem::get_data_products() const
    {
        std::vector<MetadataSubset*> data_products_;
        const YAML::Node read_node = config_data_["read"];

        for(auto& item : read_node)
        {
            const YAML::Node use_node_ = item["use"];
            const YAML::Node where_node_ = item["where"];
            const std::string version_ = use_node_["version"].as<std::string>();
            const std::string file_path_ = where_node_["data_product"].as<std::string>();
            data_products_.push_back(new MetadataSubset(file_path_, version_));
        }

        if(data_products_.empty()) throw std::runtime_error("No data products found in config");
    
        return data_products_;
    }
};