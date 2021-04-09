#include "scrc/registry/file_system.hxx"

#include <iostream>

namespace SCRC
{
    YAML::Node parse_yaml_(std::filesystem::path yaml_path)
    {
        APILogger->debug("LocalFileSystem: Reading configuration file '{0}'", yaml_path.string().c_str());
        return YAML::LoadFile(yaml_path.c_str());
    }

    LocalFileSystem::LocalFileSystem(std::filesystem::path config_file_path) :
        config_path_(config_file_path),
        config_data_(parse_yaml_(config_file_path))
    {
        APILogger->debug("LocalFileSystem: Parsing configuration file");
        const std::filesystem::path data_directory_path_ = get_data_store();

        // Create the data directory within the config file if it does not exist
        if(!std::filesystem::exists(data_directory_path_))
        {
            APILogger->info("Creating data directory at '{0}'", data_directory_path_.c_str());
            std::filesystem::create_directory(data_directory_path_);
        }
    }

    YAML::Node LocalFileSystem::meta_data_() const
    {
        return config_data_["run_metadata"];
    }

    std::filesystem::path LocalFileSystem::get_data_store() const
    {
        if(meta_data_()["default_data_store"])
        {
            std::string data_dir_ = meta_data_()["default_data_store"].as<std::string>();

            // Handle case where tilde used for HOME for environment variable
            if(data_dir_[0] == '~')
            {
                data_dir_ = std::getenv("HOME")+data_dir_.substr(1, data_dir_.size()-1);
            }

            // Handle case where '/' used meaning relative to config file path
            else if(data_dir_[0] == '/')
            {
                data_dir_ = config_path_.parent_path() / data_dir_.substr(1, data_dir_.size()-1);
            }

            return data_dir_;
        }
        else
        {
            return std::filesystem::path(std::getenv("HOME")) / std::filesystem::path("datastore");
        }
    }

    std::string LocalFileSystem::get_default_input_namespace() const
    {
        if(!meta_data_()["default_input_namespace"])
        {
            throw config_parsing_error("Cannot find value for expected key 'default_input_namespace'");
        }

        return meta_data_()["default_input_namespace"].as<std::string>();
    }

    std::string LocalFileSystem::get_default_output_namespace() const
    {
        if(!meta_data_()["default_output_namespace"])
        {
            throw config_parsing_error("Cannot find value for expected key 'default_output_namespace'");
        }

        return meta_data_()["default_output_namespace"].as<std::string>();
    }

    std::vector<ReadObject::DataProduct*> LocalFileSystem::read_data_products() const
    {
        APILogger->debug("LocalFileSystem: Reading data products list");

        std::vector<ReadObject::DataProduct*> data_products_;
        const YAML::Node read_node = config_data_["read"];

        APILogger->debug("LocalFileSystem: Iterating through entries");
        for(auto& item : read_node)
        {
            data_products_.push_back(ReadObject::data_product_from_yaml(item));
        }

        if(data_products_.empty()) throw config_parsing_error("No data products found in config");
    
        return data_products_;
    }
};