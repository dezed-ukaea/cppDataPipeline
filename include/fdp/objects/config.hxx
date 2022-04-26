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

#include <map>
#include <string>

#include <ghc/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "fdp/registry/api.hxx"
#include "fdp/objects/api_object.hxx"
#include "fdp/objects/io_object.hxx"

namespace FairDataPipeline {
    /**
     * @brief class for interacting with confifurations
     * 
     */
    class Config {
        public:
            typedef std::shared_ptr< Config > sptr;

            typedef std::map< std::string, IOObject > map_type;

        private:            
            const ghc::filesystem::path config_file_path_;
            const ghc::filesystem::path config_dir_;
            const ghc::filesystem::path script_file_path_;
            YAML::Node config_data_;            
            std::string api_url_;
            std::string token_;
            API::sptr api_;

            ApiObject::sptr user_;
            ApiObject::sptr author_;

            ApiObject::sptr config_storage_root_;
            ApiObject::sptr config_storage_location_;
            ApiObject::sptr config_file_type_;
            ApiObject::sptr config_obj_;

            ApiObject::sptr script_storage_root_;
            ApiObject::sptr script_storage_location_;
            ApiObject::sptr script_file_type_;
            ApiObject::sptr script_obj_;

            ApiObject::sptr code_repo_storage_root_;
            ApiObject::sptr code_repo_storage_location_;
            ApiObject::sptr code_repo_obj_;

            ApiObject::sptr code_run_;
            
            map_type writes_;
            map_type reads_;

            map_type outputs_;
            map_type inputs_;

            RESTAPI rest_api_location_ = RESTAPI::LOCAL;

            bool config_has_writes() const;
            bool config_has_reads() const;
            
            void initialise(RESTAPI api_location);
            void validate_config(ghc::filesystem::path yaml_path, RESTAPI api_location);
            /**
             * @brief Construct a new Config object
             * 
             * @param config_file_path the path to the config file
             * @param script_file_path the path to the script file
             * @param token the API token as a String
             * @param api_location whether or not the api is local
             */
            Config(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &script_file_path,
                    const std::string &token,
                    RESTAPI api_location);


        public:

            static Config::sptr construct(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &script_file_path,
                    const std::string &token,
                    RESTAPI api_location);


            /**
             * @brief Destroy the Config object
             * 
             */
            ~Config();
            
            /**
             * @brief Returns the meta_data Yaml Node from the config
             * 
             * @return YAML::Node 
             */
            YAML::Node meta_data_() const;
            /**
             * @brief Returns the writes Yaml Node from the config
             * 
             * @return YAML::Node 
             */
            YAML::Node config_writes_() const;

            /**
             * @brief Returns the reads Yaml Node from the config
             * 
             * @return YAML::Node 
             */
            YAML::Node config_reads_() const;

            /**
             * @brief Check whether the config has reads
             * 
             * @return true config has reads
             * @return false config does not have reads
             */
            bool has_reads() const;
            /**
             * @brief Check whether the config has writes
             * 
             * @return true the config has writes
             * @return false the config does not have writes
             */
            bool has_writes() const;
            /**
             * @brief Check whether any outputs have been added
             * 
             * @return true outputs have been added
             * @return false outputs have not been added
             */
            bool has_outputs() const;
            /**
             * @brief Check whether any inputs have been added
             * 
             * @return true inputs have been added
             * @return false inputs have not been added
             */
            bool has_inputs() const;

            /**
             * @brief Get the config file path object
             * 
             * @return ghc::filesystem::path 
             */
            
            ghc::filesystem::path get_config_file_path() const {return config_file_path_;}
            /**
             * @brief Get the script file path object
             * 
             * @return ghc::filesystem::path 
             */

            /**
             * @brief Get the script file path object
             * 
             * @return ghc::filesystem::path 
             */
            ghc::filesystem::path get_script_file_path() const {return script_file_path_;}

            /**
             * @brief Get the token as a string
             * 
             * @return std::string 
             */
            std::string get_token() const {return token_;}

            /**
             * @brief Get the api url
             * 
             * @return std::string 
             */
            std::string get_api_url() const {return api_url_;}
            /**
             * @brief Get the data store path
             * 
             * @return ghc::filesystem::path 
             */
            ghc::filesystem::path get_data_store() const;

            /**
             * @brief Get the default input namespace
             * 
             * @return std::string 
             */
            std::string get_default_input_namespace() const;

            /**
             * @brief Get the default output namespace
             * 
             * @return std::string 
             */
            std::string get_default_output_namespace() const;

            /**
             * @brief Get the config directory path as a string
             * 
             * @return std::string 
             */
            std::string get_config_directory() const;

            /**
             * @brief Get the code run uuid
             * 
             * @return const std::string&
             */
            std::string get_code_run_uuid() const;

            /**
             * @brief Provide a tempory file path for a given data product to be written
             * whilst recording metadata
             * 
             * @param data_product 
             * @return ghc::filesystem::path 
             */
            ghc::filesystem::path link_write( const std::string &data_product);

            /**
             * @brief Return the filepath to a given data product
             * 
             * @param data_product 
             * @return ghc::filesystem::path 
             */
            ghc::filesystem::path link_read(const std::string& data_product);

            /**
             * @brief Finalise the pipeline
             * Write any pending metadata to the registry
             * Rename any pending writes to their new filename(s):
             *  <file hash>.<extension>
             */
            void finalise();

            /**
             * @brief Read a given yaml file into a Yaml Node
             * 
             * @param yaml_path 
             * @return YAML::Node 
             */
            static YAML::Node parse_yaml(ghc::filesystem::path yaml_path);
            
            /**
             * @brief Get the config data as a Yaml Node
             * 
             * @return YAML::Node 
             */
            YAML::Node get_config_data() const {return config_data_;}

            /**
             * @brief Get the api object (used to interact with the pipeline directly)
             * 
             * @return std::shared_ptr<API> 
             */
             API::sptr get_api() const {return api_;}

            /**
             * @brief Get the rest api location (local / remote)
             * 
             * @return RESTAPI 
             */
            RESTAPI get_rest_api_location() const {return rest_api_location_;}

    };
};
#endif
