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
        APILogger->debug("LocalFileSystem: Parsing configuration file '{0}'", config_path_.c_str());
        const std::filesystem::path data_directory_path_ = get_data_store();

        // Create the data directory within the config file if it does not exist
        APILogger->debug("Creating data directory at '{0}'", data_directory_path_.c_str());
        std::filesystem::create_directory(data_directory_path_);

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
            if(data_dir_[0] == '~')
            {
                data_dir_ = std::getenv("HOME")+data_dir_.substr(1, data_dir_.size()-1);
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
            throw std::runtime_error("Cannot find value for expected key 'default_input_namespace'");
        }

        return meta_data_()["default_input_namespace"].as<std::string>();
    }

    std::string LocalFileSystem::get_default_output_namespace() const
    {
        if(!meta_data_()["default_output_namespace"])
        {
            throw std::runtime_error("Cannot find value for expected key 'default_output_namespace'");
        }

        return meta_data_()["default_output_namespace"].as<std::string>();
    }

    std::vector<DataProduct*> LocalFileSystem::read_data_products() const
    {
        std::vector<DataProduct*> data_products_;
        const YAML::Node read_node = config_data_["read"];

        for(auto& item : read_node)
        {
            const YAML::Node use_node_ = item["use"];
            const std::string version_ = use_node_["version"].as<std::string>();
            const std::string file_path_ = item["data_product"].as<std::string>();
            data_products_.push_back(new DataProduct(file_path_, version_));
        }

        if(data_products_.empty()) throw std::runtime_error("No data products found in config");
    
        return data_products_;
    }
};