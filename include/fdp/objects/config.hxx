/**
 * @file config.hxx
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef __FDP_CONFIG_HXX__
#define __FDP_CONFIG_HXX__

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "fdp/registry/file_system.hxx"
#include "fdp/registry/api.hxx"

namespace FDP {
    /**
     * @brief 
     * 
     */

    class Config {
        private:            
            const std::filesystem::path config_file_path_;
            const std::filesystem::path config_dir_;
            const YAML::Node config_data_;
            const std::filesystem::path script_file_path_;
            const std::string api_url_;
            const std::string token_;
            const std::filesystem::path access_token_file_;

        public:
            Config(const std::filesystem::path &config_file_path,
            const std::filesystem::path &access_token_file,
            spdlog::level::level_enum log_level,
            RESTAPI api_location);

            ~Config();
            
            YAML::Node meta_data_() const;

            std::filesystem::path get_config_file_path() const {return config_file_path_;}
            std::filesystem::path get_access_token_file() const {return access_token_file_;}
            std::filesystem::path get_data_store() const;

            std::string get_default_input_namespace() const;
            std::string get_default_output_namespace() const;

            static YAML::Node parse_yaml(std::filesystem::path yaml_path);
            YAML::Node validated_config(std::filesystem::path yaml_path, RESTAPI api_location) const;
            YAML::Node get_config_data() const {return config_data_;}

    };
};
#endif