#include "fdp/objects/config.hxx"

namespace FDP
{
    FDP::Config::Config(const std::filesystem::path &config_file_path,
        const std::filesystem::path &access_token_file,
        spdlog::level::level_enum log_level,
        RESTAPI api_location) :
            access_token_file_(access_token_file),
            config_file_path_(config_file_path),
            config_data_(validated_config(config_file_path, api_location))
        {
        
        }
    Config::~Config(){
            //delete api;
    }

    YAML::Node FDP::Config::parse_yaml(std::filesystem::path yaml_path) {
        APILogger->debug("[Config]: Reading configuration file '{0}'",
                        yaml_path.string().c_str());
        return YAML::LoadFile(yaml_path.string().c_str());
    }

    YAML::Node FDP::Config::meta_data_() const {
        return config_data_["run_metadata"];
    }

    std::filesystem::path FDP::Config::get_data_store() const {
        return std::filesystem::path(FDP::Config::meta_data_()["write_data_store"].as<std::string>());
    }

    std::string FDP::Config::get_default_input_namespace() const {
    if (!meta_data_()["default_input_namespace"]) {
        throw config_parsing_error(
            "Cannot find value for expected key 'default_input_namespace'");
    }

    return meta_data_()["default_input_namespace"].as<std::string>();
    }

    std::string FDP::Config::get_default_output_namespace() const {
    if (!meta_data_()["default_output_namespace"]) {
        throw config_parsing_error(
            "Cannot find value for expected key 'default_output_namespace'");
    }

    return meta_data_()["default_output_namespace"].as<std::string>();
    }

    YAML::Node FDP::Config::validated_config(std::filesystem::path yaml_path, RESTAPI api_location) const{
        YAML::Node config = parse_yaml(yaml_path);
        if (!config["run_metadata"]) {
            APILogger->error(
                "Failed to obtain run metadata, the key 'run_metadata' is not"
                " present in the configuration");
            throw config_parsing_error("No run metadata provided");
            }

            if (api_location == RESTAPI::LOCAL &&
                !meta_data_()["local_data_registry_url"]) {
            APILogger->error("Could not determine URL for local repository, key "
                            "'local_data_registry_url' "
                            "is absent from configuration");
            throw config_parsing_error("Failed to read local repository url");
            }

            else if (api_location == RESTAPI::REMOTE &&
                    !meta_data_()["remote_data_registry_url"]) {
            APILogger->error("Could not determine URL for remote repository, key "
                            "'remote_data_registry_url' "
                            "is absent from configuration");
            throw config_parsing_error("Failed to read remote repository url");
            }
        return config;
    }

}; // namespace FDP
