#include "fdp/objects/config.hxx"

namespace FDP {
FDP::Config::Config(const std::filesystem::path &config_file_path,
                    const std::filesystem::path &script_file_path,
                    const std::string &token, 
                    RESTAPI api_location)
    : token_(token), config_file_path_(config_file_path), script_file_path_(script_file_path) {
  validate_config(config_file_path, api_location);
  initialise(api_location);
}
Config::~Config() {
  // delete api;
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
  return std::filesystem::path(
      FDP::Config::meta_data_()["write_data_store"].as<std::string>());
}

std::string FDP::Config::get_default_input_namespace() const {
  return meta_data_()["default_input_namespace"].as<std::string>();
}

std::string FDP::Config::get_default_output_namespace() const {
  return meta_data_()["default_output_namespace"].as<std::string>();
}

void FDP::Config::validate_config(std::filesystem::path yaml_path,
                                  RESTAPI api_location) {
  config_data_ = parse_yaml(yaml_path);
  if (!config_data_["run_metadata"]) {
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

  else if (!(api_location == RESTAPI::LOCAL ||
             api_location == RESTAPI::REMOTE)) {
    APILogger->error("Unrecognised API location");
    throw config_parsing_error("Failed to resolve registry location");
  }

  if (!meta_data_()["write_data_store"]) {
    APILogger->error("Failed to Read: [\"write_data_store\"] from {0}", yaml_path.string());
    throw config_parsing_error("Failed to Read: [\"write_data_store\"] from " + yaml_path.string());
  }
  if (!meta_data_()["default_input_namespace"]) {
    APILogger->error("Failed to Read: [\"default_input_namespace\"] from {0}", yaml_path.string());
    throw config_parsing_error("Failed to Read: [\"default_input_namespace\"] from " + yaml_path.string());
  }
  if (!meta_data_()["default_output_namespace"]) {
    APILogger->error("Failed to Read: [\"default_output_namespace\"] from {0}", yaml_path.string());
    throw config_parsing_error("Failed to Read: [\"default_output_namespace\"] from " + yaml_path.string());
  }

  if (!meta_data_()["latest_commit"]) {
    APILogger->error("Failed to Read: [\"latest_commit\"] from {0}", yaml_path.string());
    throw config_parsing_error("Failed to Read: [\"latest_commit\"] from " + yaml_path.string());
  }

  if (!meta_data_()["remote_repo"]) {
    APILogger->error("Failed to Read: [\"remote_repo\"] from {0}", yaml_path.string());
    throw config_parsing_error("Failed to Read: [\"remote_repo\"] from " + yaml_path.string());
  }

  if (!meta_data_()["description"]) {
    APILogger->error("Failed to Read: [\"description\"] from {0}", yaml_path.string());
    throw config_parsing_error("Failed to Read: [\"description\"] from " + yaml_path.string());
  }

  if (!FileExists(script_file_path_.string())) {
    APILogger->error("Submission script: {0} does not exist", script_file_path_.string());
    throw std::runtime_error("Submission script: " + script_file_path_.string() + " does not exist");
  }
}

void FDP::Config::initialise(RESTAPI api_location) {
  // Set API URL
  if (api_location == RESTAPI::REMOTE) {
    api_url_ = API::append_with_forward_slash(
        meta_data_()["remote_data_registry_url"].as<std::string>());
  } else {
    api_url_ = API::append_with_forward_slash(
        meta_data_()["local_data_registry_url"].as<std::string>());
  }

  APILogger->info("Reading {0} from local filestore",
                  config_file_path_.string());
  // Create and API object as a shared pointer
  api_ = std::make_shared<API>(api_url_);

  // Get the admin user from registry
  Json::Value user_json_;
  user_json_["username"] = "admin";
  std::unique_ptr<ApiObject> user_ptr_(new ApiObject(
      api_->get_by_json_query("users", user_json_, 200, token_)[0]));
  user_ = std::move(user_ptr_);
  if (user_->is_empty()) {
    APILogger->error("User: Admin Not Found");
    throw std::runtime_error("User: Admin Not Found");
  }

  //Get the author by querying the user_author table
  Json::Value user_author_json_;
  user_author_json_["user"] = user_->get_id();
  Json::Value user_author_ = api_->get_by_json_query("user_author", user_author_json_, 200, token_)[0];
  std::unique_ptr<ApiObject> author_ptr_(new ApiObject(
      api_->get_by_id("author", ApiObject::get_id_from_string(user_author_["author"].asString()), 200, token_)));
  author_ = std::move(author_ptr_);
  if (author_->is_empty()) {
    APILogger->error(
        "Author for User Admin not found please ensure you have run fair init");
    throw std::runtime_error(
        "Author Not Found: Please ensure you have run fair init");
  }

  // Create Config Storage Root
  Json::Value config_storage_root_value_;
  config_storage_root_value_["root"] = meta_data_()["write_data_store"].as<std::string>();
  config_storage_root_value_["local"] = api_location == RESTAPI::LOCAL; 
  std::unique_ptr<ApiObject> config_storage_root_ptr_(new ApiObject(
    api_->post("storage_root", config_storage_root_value_, token_)));
  config_storage_root_ = std::move(config_storage_root_ptr_);

  // Remove the Write Data Store from config file path
  Json::Value config_storage_location_value_;
  if(config_file_path_.string().find(meta_data_()["write_data_store"].as<std::string>()) !=std::string::npos){
    config_storage_location_value_["path"] = config_file_path_.string().replace(
    config_file_path_.string().find(meta_data_()["write_data_store"].as<std::string>()),
    sizeof(meta_data_()["write_data_store"].as<std::string>()) - 1, "");
  }
  else {
    config_storage_location_value_["path"] = config_file_path_.string();
  }
  config_storage_location_value_["public"] = true;  
  config_storage_location_value_["hash"] = calculate_hash_from_file(config_file_path_);
  config_storage_location_value_["storage_root"] = config_storage_root_->get_uri();
  std::unique_ptr<ApiObject> config_storage_location_ptr_(new ApiObject(
    api_->post("storage_location", config_storage_location_value_, token_)));
  config_storage_location_ = std::move(config_storage_location_ptr_);

  Json::Value config_value_;
  config_value_["description"] = "Working config.yaml in datastore";
  config_value_["storage_location"] = "";
  Json::Value author_id_ = author_->get_uri();
  config_value_["authors"].append(author_id_);

  Json::Value  config_file_type_value;
  config_file_type_value["name"] = "yaml";
  config_file_type_value["extension"] = "yaml";
  Json::Value config_file_type_url_ = api_->post("file_type", config_file_type_value, token_)["url"];

  config_value_["file_type"] = config_file_type_url_;

  APILogger->info("Writing config file {0} to registry", config_file_path_.string());

  std::unique_ptr<ApiObject> config_obj_ptr(new ApiObject(
    api_->post("object", config_value_, token_)));
  config_obj_ = std::move(config_obj_ptr);

    Json::Value script_storage_location_value_;

  if(script_file_path_.string().find(meta_data_()["write_data_store"].as<std::string>()) !=std::string::npos){
    script_storage_location_value_["path"] = script_file_path_.string().replace(
    script_file_path_.string().find(meta_data_()["write_data_store"].as<std::string>()),
    sizeof(meta_data_()["write_data_store"].as<std::string>()) - 1, "");
  }
  else {
    script_storage_location_value_["path"] = script_file_path_.string();
  }

  script_storage_location_value_["hash"] = calculate_hash_from_file(script_file_path_);
  script_storage_location_value_["public"] = true;
  script_storage_location_value_["storage_root"] = config_storage_root_->get_uri();

  std::unique_ptr<ApiObject> script_storage_location_ptr_(new ApiObject(
    api_->post("storage_location", script_storage_location_value_, token_)));
  script_storage_location_ = std::move(script_storage_location_ptr_);

  // @todo What happens if a unix executable without and extension is given
  Json::Value script_file_type_value_;
  script_file_type_value_["name"] = "C++ Submission Script" + script_file_path_.extension().string();
  script_file_type_value_["extension"] = script_file_path_.extension().string();
  Json::Value script_file_type_url_ = api_->post("file_type", script_file_type_value_, token_)["url"];

  Json::Value script_value_;
  script_value_["description"] = "Working script location in datastore";
  script_value_["authors"].append(author_id_);
  script_value_["filetype"] = script_file_type_url_.asString();
  script_value_["storage_location"] = script_storage_location_->get_uri();

  APILogger->info("Writing script file {0} to registry", script_file_path_.string());

  std::unique_ptr<ApiObject> script_obj_ptr(new ApiObject(
    api_->post("object", script_value_, token_)));
  script_obj_ = std::move(script_obj_ptr);

  Json::Value repo_storage_root_value_;
  repo_storage_root_value_["root"] = "https://github.com";
  repo_storage_root_value_["local"] = false;

  std::unique_ptr<ApiObject> code_repo_root_ptr(new ApiObject(
   api_->post("storage_root", repo_storage_root_value_, token_)));
  code_repo_storage_root_ = std::move(code_repo_root_ptr);

  Json::Value repo_storage_location_value_;
  repo_storage_location_value_["hash"] = meta_data_()["latest_commit"].as<std::string>();
  repo_storage_location_value_["public"] = true;
  repo_storage_location_value_["storage_root"] = code_repo_storage_root_->get_uri();
  repo_storage_location_value_["path"] = meta_data_()["remote_repo"].as<std::string>();

  std::unique_ptr<ApiObject> code_repo_location_ptr(new ApiObject(
   api_->post("storage_location", repo_storage_location_value_, token_)));
  code_repo_storage_location_ = std::move(code_repo_location_ptr);

  Json::Value code_repo_obj_value_;
  code_repo_obj_value_["description"] = "Processing Script Location";
  code_repo_obj_value_["storage_location"] = code_repo_storage_location_->get_uri();
  code_repo_obj_value_["authors"].append(author_id_);

  std::unique_ptr<ApiObject> code_repo_obj_ptr(new ApiObject(
   api_->post("object", code_repo_obj_value_, token_)));
  code_repo_obj_ = std::move(code_repo_obj_ptr);

  Json::Value code_run_value_;
  code_run_value_["run_date"] = current_time_stamp();
  code_run_value_["description"] = meta_data_()["description"].as<std::string>();
  code_run_value_["code_repo"] = code_repo_obj_->get_uri();
  code_run_value_["model_config"] = config_obj_->get_uri();
  code_run_value_["submission_script"] = script_obj_->get_uri();
  code_run_value_["input_urls"] = Json::arrayValue;
  code_run_value_["output_urls"] = Json::arrayValue;

  APILogger->info("Writing new code run to registry");

  std::unique_ptr<ApiObject> code_run_ptr(new ApiObject(
   api_->post("object", code_run_value_, token_)));
  code_run_ = std::move(code_run_ptr);

  APILogger->info("Code run {0} successfully generated", code_run_->get_value_as_string("uuid"));

}

std::string Config::get_config_directory() const{
  return config_file_path_.parent_path().string();
};

std::string Config::get_code_run_uuid() const{
  return code_run_->get_value_as_string("uuid");
};

}; // namespace FDP
