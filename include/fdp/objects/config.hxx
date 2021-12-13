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
#include <regex>

#include "fdp/registry/file_system.hxx"
#include "fdp/registry/api.hxx"
#include "fdp/objects/api_object.hxx"

namespace FDP {
    /**
     * @brief 
     * 
     */

    class Config {
        private:            
            const std::filesystem::path config_file_path_;
            const std::filesystem::path config_dir_;
            const std::filesystem::path script_file_path_;
            YAML::Node config_data_;            
            std::string api_url_;
            std::string token_;
            std::shared_ptr<API> api_;

            std::unique_ptr<ApiObject> user_;
            std::unique_ptr<ApiObject> author_;

            std::unique_ptr<ApiObject> config_storage_root_;
            std::unique_ptr<ApiObject> config_storage_location_;
            std::unique_ptr<ApiObject> config_file_type_;
            std::unique_ptr<ApiObject> config_obj_;

            std::unique_ptr<ApiObject> script_storage_root_;
            std::unique_ptr<ApiObject> script_storage_location_;
            std::unique_ptr<ApiObject> script_file_type_;
            std::unique_ptr<ApiObject> script_obj_;

            std::unique_ptr<ApiObject> code_repo_storage_root_;
            std::unique_ptr<ApiObject> code_repo_storage_location_;
            std::unique_ptr<ApiObject> code_repo_;
            

        public:
            Config(const std::filesystem::path &config_file_path,
            const std::string &token,
            RESTAPI api_location);

            ~Config();
            
            YAML::Node meta_data_() const;

            std::filesystem::path get_config_file_path() const {return config_file_path_;}
            std::string get_token() const {return token_;}
            std::string get_api_url() const {return api_url_;}
            std::filesystem::path get_data_store() const;

            std::string get_default_input_namespace() const;
            std::string get_default_output_namespace() const;

            void initialise(RESTAPI api_location);
            void validate_config(std::filesystem::path yaml_path, RESTAPI api_location);

            static YAML::Node parse_yaml(std::filesystem::path yaml_path);
            
            YAML::Node get_config_data() const {return config_data_;}

            std::shared_ptr<API> get_api(){return api_;}

    };
};
#endif