#include "scrc/registry/file_system.hxx"

#include <iostream>

namespace SCRC
{
    YAML::Node parse_yaml_(std::filesystem::path yaml_path)
    {
        return YAML::LoadFile(yaml_path.c_str());
    }

    LocalFileSystem::LocalFileSystem(std::filesystem::path config_file_path) :
        config_path_(config_file_path)
    {
        YAML::Node config_file_data_ = parse_yaml_(config_path_);

        std::filesystem::path data_directory_path_;

        try
        {
            data_directory_path_ = config_file_data_["data_directory"].as<std::string>();
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
};