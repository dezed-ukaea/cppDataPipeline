#include "fdp/objects/config.hxx"

namespace FairDataPipeline {

    Config::sptr Config::construct(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &script_file_path,
                    const std::string &token,
                    RESTAPI api_location)
    {
        Config::sptr pobj = Config::sptr( new Config( 
                    config_file_path
                    , script_file_path
                    , token
                    , api_location ) );
        return pobj;
    }



FairDataPipeline::Config::Config(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &script_file_path,
                    const std::string &token, 
                    RESTAPI api_location)
    : token_(token), config_file_path_(config_file_path), script_file_path_(script_file_path),
    rest_api_location_(api_location) {
  validate_config(config_file_path, api_location);
  initialise(api_location);

    }

Config::~Config() {
}

YAML::Node FairDataPipeline::Config::parse_yaml(ghc::filesystem::path yaml_path) {
  logger::get_logger()->debug() 
      << "[Config]: Reading configuration file '" << yaml_path.string().c_str() << "'";
  return YAML::LoadFile(yaml_path.string().c_str());
}

YAML::Node FairDataPipeline::Config::meta_data_() const {
  return config_data_["run_metadata"];
}

YAML::Node FairDataPipeline::Config::config_writes_() const{
  return config_data_["write"];
}

YAML::Node FairDataPipeline::Config::config_reads_() const{
  return config_data_["read"];
}

bool FairDataPipeline::Config::config_has_writes() const{
  if(config_data_["write"]){
    return true;
  }
  return false;
}

bool FairDataPipeline::Config::config_has_reads() const{
  if(config_data_["read"]){
    return true;
  }
  return false;
}

bool FairDataPipeline::Config::has_writes() const{
  return ! writes_.empty();
}

bool FairDataPipeline::Config::has_reads() const{
  return ! reads_.empty();
}

bool FairDataPipeline::Config::has_inputs() const{
  return ! inputs_.empty();
}

bool FairDataPipeline::Config::has_outputs() const{
  return ! outputs_.empty();
}

ghc::filesystem::path FairDataPipeline::Config::get_data_store() const {
  return ghc::filesystem::path(
      FairDataPipeline::Config::meta_data_()["write_data_store"].as<std::string>());
}

std::string FairDataPipeline::Config::get_default_input_namespace() const {
  return meta_data_()["default_input_namespace"].as<std::string>();
}

std::string FairDataPipeline::Config::get_default_output_namespace() const {
  return meta_data_()["default_output_namespace"].as<std::string>();
}

void FairDataPipeline::Config::validate_config(ghc::filesystem::path yaml_path,
                                  RESTAPI api_location) {
  config_data_ = parse_yaml(yaml_path);
  if (!config_data_["run_metadata"]) {
    logger::get_logger()->error()
        <<  "Failed to obtain run metadata, the key 'run_metadata' is not"
        " present in the configuration";
    throw config_parsing_error("No run metadata provided");
  }

  if (api_location == RESTAPI::LOCAL &&
      !meta_data_()["local_data_registry_url"]) {
    logger::get_logger()->error()
        << "Could not determine URL for local repository, key "
        "'local_data_registry_url' "
        "is absent from configuration";
    throw config_parsing_error("Failed to read local repository url");
  }

  else if (api_location == RESTAPI::REMOTE &&
           !meta_data_()["remote_data_registry_url"]) {
    logger::get_logger()->error()
        <<"Could not determine URL for remote repository, key "
        "'remote_data_registry_url' "
        "is absent from configuration";
    throw config_parsing_error("Failed to read remote repository url");
  }

  else if (!(api_location == RESTAPI::LOCAL ||
             api_location == RESTAPI::REMOTE)) {
    logger::get_logger()->error() << "Unrecognised API location";
    throw config_parsing_error("Failed to resolve registry location");
  }

  if (!meta_data_()["write_data_store"]) {
    logger::get_logger()->error() 
        << "Failed to Read: [\"write_data_store\"] from " 
        << yaml_path.string();
    throw config_parsing_error("Failed to Read: [\"write_data_store\"] from " + yaml_path.string());
  }
  if (!meta_data_()["default_input_namespace"]) {
    logger::get_logger()->error() 
        << "Failed to Read: [\"default_input_namespace\"] from " << yaml_path.string();
    throw config_parsing_error("Failed to Read: [\"default_input_namespace\"] from " + yaml_path.string());
  }
  if (!meta_data_()["default_output_namespace"]) {
    logger::get_logger()->error() 
        << "Failed to Read: [\"default_output_namespace\"] from " << yaml_path.string();
    throw config_parsing_error("Failed to Read: [\"default_output_namespace\"] from " + yaml_path.string());
  }

  if (!meta_data_()["latest_commit"]) {
    logger::get_logger()->error() 
        << "Failed to Read: [\"latest_commit\"] from " << yaml_path.string();
    throw config_parsing_error("Failed to Read: [\"latest_commit\"] from " + yaml_path.string());
  }

  if (!meta_data_()["remote_repo"]) {
    logger::get_logger()->error() 
        << "Failed to Read: [\"remote_repo\"] from " << yaml_path.string();
    throw config_parsing_error("Failed to Read: [\"remote_repo\"] from " + yaml_path.string());
  }

  if (!meta_data_()["description"]) {
    logger::get_logger()->error()
        << "Failed to Read: [\"description\"] from " << yaml_path.string();
    throw config_parsing_error("Failed to Read: [\"description\"] from " + yaml_path.string());
  }

  if (!file_exists(script_file_path_.string())) {
    logger::get_logger()->error() 
        << "Submission script: "
        <<  script_file_path_.string()
        << " does not exist";
    throw std::runtime_error("Submission script: " + script_file_path_.string() + " does not exist");
  }

  if(!meta_data_()["public"]){
    meta_data_()["public"] = "true";
  }
  else{
    try
    {
      meta_data_()["public"].as<bool>();
    }
    catch(const std::exception& e)
    {
      meta_data_()["public"] = "true";
    }
    
  }

}

void FairDataPipeline::Config::initialise(RESTAPI api_location) {
  // Set API URL
  if (api_location == RESTAPI::REMOTE) {
    api_url_ = API::append_with_forward_slash(
        meta_data_()["remote_data_registry_url"].as<std::string>());
  } else {
    api_url_ = API::append_with_forward_slash(
        meta_data_()["local_data_registry_url"].as<std::string>());
  }

  logger::get_logger()->info() << "Reading " 
      << config_file_path_.string()
      << " from local filestore";
  // Create and API object as a shared pointer
  api_ = API::construct(api_url_);

  // Get the admin user from registry
  Json::Value user_json_;
  user_json_["username"] = "admin";

  Json::Value j = api_->get_by_json_query("users", user_json_, 200, token_);
  this->user_ = ApiObject::from_json( j [0]);

  if (user_->is_empty()) {
    logger::get_logger()->error() << "User: Admin Not Found";
    throw std::runtime_error("User: Admin Not Found");
  }

  //Get the author by querying the user_author table
  Json::Value user_author_json_;
  user_author_json_["user"] = user_->get_id();
  Json::Value user_author_ = api_->get_by_json_query("user_author", user_author_json_, 200, token_)[0];

  Json::Value j_author = api_->get_by_id("author", ApiObject::get_id_from_string(user_author_["author"].asString()), 200, token_);

  this->author_ = ApiObject::from_json( j_author );

  if (author_->is_empty()) {
    logger::get_logger()->error()
        <<  "Author for User Admin not found please ensure you have run fair init";
    throw std::runtime_error(
        "Author Not Found: Please ensure you have run fair init");
  }

  // Create Config Storage Root
  Json::Value config_storage_root_value_;
  config_storage_root_value_["root"] = meta_data_()["write_data_store"].as<std::string>();
  config_storage_root_value_["local"] = api_location == RESTAPI::LOCAL; 

  Json::Value j_storage_root = api_->post_storage_root(config_storage_root_value_, token_);
  this->config_storage_root_  = ApiObject::from_json( j_storage_root );

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
  config_storage_location_value_["path"] = remove_backslash_from_path(config_storage_location_value_["path"].asString());
  config_storage_location_value_["path"] = API::remove_leading_forward_slash(config_storage_location_value_["path"].asString());
  config_storage_location_value_["public"] = true;  
  config_storage_location_value_["hash"] = calculate_hash_from_file(config_file_path_);
  config_storage_location_value_["storage_root"] = config_storage_root_->get_uri();


  Json::Value j_storage_location = api_->post("storage_location", config_storage_location_value_, token_);
  this->config_storage_location_ = ApiObject::from_json( j_storage_location );

  Json::Value config_value_;
  config_value_["description"] = "Working config.yaml in datastore";
  config_value_["storage_location"] = config_storage_location_->get_uri();
  Json::Value author_id_ = author_->get_uri();
  config_value_["authors"].append(author_id_);

  Json::Value  config_file_type_value;
  config_file_type_value["name"] = "yaml";
  config_file_type_value["extension"] = "yaml";
  Json::Value config_file_type_url_ = api_->post("file_type", config_file_type_value, token_)["url"];

  config_value_["file_type"] = config_file_type_url_;

  logger::get_logger()->info() 
      << "Writing config file " 
      <<  config_file_path_.string()
      << " to registry", config_file_path_.string();

  Json::Value j_config_obj = api_->post("object", config_value_, token_);
 
  this->config_obj_ = ApiObject::from_json( j_config_obj );

    Json::Value script_storage_location_value_;

  if(script_file_path_.string().find(meta_data_()["write_data_store"].as<std::string>()) !=std::string::npos){
    script_storage_location_value_["path"] = script_file_path_.string().replace(
    script_file_path_.string().find(meta_data_()["write_data_store"].as<std::string>()),
    sizeof(meta_data_()["write_data_store"].as<std::string>()) - 1, "");
  }
  else {
    script_storage_location_value_["path"] = script_file_path_.string();
  }
  script_storage_location_value_["path"] = remove_backslash_from_path(script_storage_location_value_["path"].asString());
  script_storage_location_value_["path"] = API::remove_leading_forward_slash(script_storage_location_value_["path"].asString());
  script_storage_location_value_["hash"] = calculate_hash_from_file(script_file_path_);
  script_storage_location_value_["public"] = true;
  script_storage_location_value_["storage_root"] = config_storage_root_->get_uri();

  Json::Value j_script_storage_location = api_->post("storage_location", script_storage_location_value_, token_);

  this->script_storage_location_ = ApiObject::from_json( j_script_storage_location );

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

  logger::get_logger()->info() 
      << "Writing script file " 
      << script_file_path_.string() 
      << " to registry";

  Json::Value j_script_obj = api_->post("object", script_value_, token_);
  this->script_obj_ = ApiObject::from_json( j_script_obj );

  Json::Value repo_storage_root_value_;
  repo_storage_root_value_["root"] = "https://github.com/";
  repo_storage_root_value_["local"] = false;

  Json::Value j_code_repo_root = api_->post("storage_root", repo_storage_root_value_, token_);
  this->code_repo_storage_root_ = ApiObject::from_json( j_code_repo_root );

  std::string repo_storage_path_ = std::regex_replace(meta_data_()["remote_repo"].as<std::string>(), std::regex(repo_storage_root_value_["root"].asString()), "");

  Json::Value repo_storage_location_value_;
  repo_storage_location_value_["hash"] = meta_data_()["latest_commit"].as<std::string>();
  repo_storage_location_value_["public"] = true;
  repo_storage_location_value_["storage_root"] = code_repo_storage_root_->get_uri();
  repo_storage_location_value_["path"] = repo_storage_path_;

  Json::Value j_code_repo_location = api_->post("storage_location", repo_storage_location_value_, token_);
  this->code_repo_storage_location_ = ApiObject::from_json( j_code_repo_location );

  Json::Value code_repo_obj_value_;
  code_repo_obj_value_["description"] = "Processing Script Location";
  code_repo_obj_value_["storage_location"] = code_repo_storage_location_->get_uri();
  code_repo_obj_value_["authors"].append(author_id_);

  Json::Value j_code_repo_obj = api_->post("object", code_repo_obj_value_, token_);
  this->code_repo_obj_ = ApiObject::from_json( j_code_repo_obj );

  Json::Value code_run_value_;
  code_run_value_["run_date"] = current_time_stamp();
  code_run_value_["description"] = meta_data_()["description"].as<std::string>();
  code_run_value_["code_repo"] = code_repo_obj_->get_uri();
  code_run_value_["model_config"] = config_obj_->get_uri();
  code_run_value_["submission_script"] = script_obj_->get_uri();
  code_run_value_["input_urls"] = Json::arrayValue;
  code_run_value_["output_urls"] = Json::arrayValue;

  logger::get_logger()->info() << "Writing new code run to registry";

  Json::Value j_code_run = api_->post("code_run", code_run_value_, token_);
  this->code_run_ = ApiObject::from_json( j_code_run );

  logger::get_logger()->info() 
      << "Code run " 
      <<  code_run_->get_value_as_string("uuid") 
      << " successfully generated";
}

std::string Config::get_config_directory() const{
  return config_file_path_.parent_path().string();
};

std::string Config::get_code_run_uuid() const{
  return code_run_->get_value_as_string("uuid");
};


ghc::filesystem::path Config::link_write( const std::string& data_product){
  if (!config_has_writes()){
    logger::get_logger()->error()
        << "Config Error: Write has not been specified in the given config file";
    throw config_parsing_error("Config Error: Write has not been specified in the given config file");
  }

  //logger::get_logger()->info() << "Node Type: " 
  //<< config_writes_().Type());
  
  YAML::Node currentWrite;

  if(config_writes_().IsSequence()){
    for (YAML::const_iterator it = config_writes_().begin(); it != config_writes_().end(); ++it) {
      if(it->as<YAML::Node>()["data_product"]){
        if(it->as<YAML::Node>()["data_product"].as<std::string>() == data_product)
        {
          currentWrite = it->as<YAML::Node>();
        }
      }

    }
  }
  else{
    logger::get_logger()->error()
        << "Config Error: Write has not been specified in the given config file";
    throw config_parsing_error("Config Error: Write has not been specified in the given config file");
  }

  if(!currentWrite)
  {
      logger::get_logger()->error()
          << "Config Error: Cannot Find "
          << data_product
          << " in writes";

    throw config_parsing_error("Config Error: cannot find " + data_product + "in writes");
  }

  if(!currentWrite["description"])
  {
    logger::get_logger()->error() 
        << "Config Error: Cannot Find description of "
        << data_product
        << " in writes";
    throw config_parsing_error("Config Error: cannot find description of " + data_product + "in writes");
  }

  if(!currentWrite["file_type"])
  {
    logger::get_logger()->error()
        << "Config Error: Cannot Find file_type of " 
        << data_product
        << " in writes";
    throw config_parsing_error("Config Error: cannot find file_type of " + data_product + "in writes");
  }

  if(!currentWrite["use"]["version"]){
    logger::get_logger()->info() 
        << "Use: Version not found in "
        << data_product 
        << ", using version 0.0.1 by default";
    currentWrite["use"]["version"] = "0.0.1";
  }

  if(!currentWrite["use"]["data_product"]){
    currentWrite["use"]["data_product"] = currentWrite["data_product"].as<std::string>();
  }

    if(!currentWrite["use"]["namespace"]){
    currentWrite["use"]["namespace"] = meta_data_()["default_output_namespace"].as<std::string>();
  }

  std::string filename_("dat-" + generate_random_hash() + "." + currentWrite["file_type"].as<std::string>());
  ghc::filesystem::path path_ = ghc::filesystem::path(meta_data_()["write_data_store"].as<std::string>()) / currentWrite["use"]["namespace"].as<std::string>() / currentWrite["use"]["data_product"].as<std::string>() / filename_;

  logger::get_logger()->info() << "Link Path: " << path_.string();

  // Create Directory
  ghc::filesystem::create_directories(path_.parent_path().string());

  writes_[data_product] = IOObject(data_product, 
    currentWrite["data_product"].as<std::string>(),
    currentWrite["use"]["version"].as<std::string>(),
    currentWrite["use"]["namespace"].as<std::string>(),
    path_,
    currentWrite["description"].as<std::string>(),
    meta_data_()["public"].as<bool>()
    );
  return path_;

}

ghc::filesystem::path FairDataPipeline::Config::link_read( const std::string &data_product){
  YAML::Node currentRead;

  auto it = inputs_.find("data_product");
  if (it != inputs_.end()) {
      return it->second.get_path();
  }

  if(config_reads_().IsSequence()){
    for (YAML::const_iterator it = config_reads_().begin(); it != config_reads_().end(); ++it) {
      if(it->as<YAML::Node>()["data_product"]){
        if(it->as<YAML::Node>()["data_product"].as<std::string>() == data_product)
        {
          currentRead = it->as<YAML::Node>();
        }
      }

    }
  }
  else{
    logger::get_logger()->error() 
        << "Config Error: Write has not been specified in the given config file";
    throw config_parsing_error("Config Error: Write has not been specified in the given config file");
  }

  if(!currentRead)
  {
    logger::get_logger()->error() 
        << "Config Error: Cannot Find " 
        << data_product
        << " in reads";
    throw config_parsing_error("Config Error: cannot find " + data_product + "in reads");
  }

  if(!currentRead["use"]["version"]){
    logger::get_logger()->info() 
        << "Use: Version not found in "
        << data_product
        << ", using version 0.0.1 by default";
    currentRead["use"]["version"] = "0.0.1";
  }

  if(!currentRead["use"]["data_product"]){
    currentRead["use"]["data_product"] = currentRead["data_product"].as<std::string>();
  }

  if(!currentRead["use"]["namespace"]){
    currentRead["use"]["namespace"] = meta_data_()["default_input_namespace"].as<std::string>();
  }

  Json::Value namespaceData;
  namespaceData["name"] = currentRead["use"]["namespace"].as<std::string>();

  Json::Value j_namespace = api_->get_by_json_query("namespace", namespaceData)[0];
  ApiObject::sptr namespaceObj = ApiObject::from_json( j_namespace );

  if (namespaceObj->is_empty()){
    logger::get_logger()->error()
        << "Namespace Error: could not find namespace " 
        <<  currentRead["use"]["namespace"].as<std::string>() 
        << " in registry";
    throw std::runtime_error("Namespace Error: could not find namespace " + currentRead["use"]["namespace"].as<std::string>() + " in Registry");
  }

  Json::Value dataProductData;
  dataProductData["name"] = currentRead["use"]["data_product"].as<std::string>();
  dataProductData["version"] = currentRead["use"]["version"].as<std::string>();
  dataProductData["namespace"] = namespaceObj->get_id();
  
  Json::Value j_data_prod_obj = api_->get_by_json_query("data_product", dataProductData)[0];
  ApiObject::sptr dataProductObj = ApiObject::from_json( j_data_prod_obj );
  //ApiObject dataProductObj = ApiObject(api_->get_by_json_query("data_product", dataProductData)[0]);
  if (dataProductObj->is_empty()){
    logger::get_logger()->error() 
        << "data_product Error: could not find data_product "
        <<  currentRead["use"]["data_product"].as<std::string>() 
        << " in registry";
    throw std::runtime_error("Namespace Error: could not find data_product " + currentRead["use"]["data_product"].as<std::string>() + " in Registry");
  }

  Json::Value _j_ = api_->get_by_id("object", ApiObject::get_id_from_string(dataProductObj->get_value_as_string("object")));
  ApiObject::sptr obj = ApiObject::from_json( _j_ );

  if (obj->is_empty()){
    logger::get_logger()->error() 
        << "data_product Error: could not find data_product object "
        << dataProductObj->get_value_as_string("object") 
        << " in registry";
    throw std::runtime_error("Namespace Error: could not find data_product object " + dataProductObj->get_value_as_string("object") + " in Registry");
  }

  Json::Value componentData;
  componentData["object"] = obj->get_id();
  Json::Value j_obj_component = api_->get_by_json_query("object_component", componentData)[0];
  ApiObject::sptr componentObj = ApiObject::from_json( j_obj_component );

  if (componentObj->is_empty()){
    logger::get_logger()->error() 
        << "data_product object Error: could not find data_product component_object for " 
        << obj->get_value_as_string("object") 
        << " in registry";
    throw std::runtime_error("data_product object Error: could not find data_product component_object for " + obj->get_value_as_string("object") + " in Registry");
  }

    
  Json::Value j_tmp2 = api_->get_by_id("storage_location", ApiObject::get_id_from_string(obj->get_value_as_string("storage_location")));

  ApiObject::sptr storageLocationObj = ApiObject::from_json( j_tmp2 );

  if (storageLocationObj->is_empty()){
    logger::get_logger()->error() << "data_product object Error: could not find storage_location for " 
    << obj->get_value_as_string("object") << " in registry";

    throw std::runtime_error("data_product object Error: could not find storage_location for " + obj->get_value_as_string("object") + " in Registry");
  }

  Json::Value j_tmp3 = api_->get_by_id("storage_root",  ApiObject::get_id_from_string(storageLocationObj->get_value_as_string("storage_root")));
  ApiObject::sptr storageRootObj = ApiObject::from_json( j_tmp3 );

  if (storageRootObj->is_empty()){
      std::string obj_str = obj->get_value_as_string("object");
    
      logger::get_logger()->error() 
        << "data_product object Error: could not find storage_root for " << obj_str << " in registry";

    throw std::runtime_error("data_product object Error: could not find storage_root for " + obj_str + " in Registry");
  } 

  ghc::filesystem::path path_ = ghc::filesystem::path(remove_local_from_root(storageRootObj->get_value_as_string("root"))) / 
    API::remove_leading_forward_slash(storageLocationObj->get_value_as_string("path"));

  reads_[data_product] = IOObject(data_product, 
    currentRead["data_product"].as<std::string>(),
    currentRead["use"]["version"].as<std::string>(),
    currentRead["use"]["namespace"].as<std::string>(),
    path_,
    *componentObj,
    *dataProductObj
    );

  return path_;
}

void FairDataPipeline::Config::finalise(){

  if(has_writes()){
      Config::map_type::iterator it;
    for (it = writes_.begin(); it != writes_.end(); it++){
      IOObject& currentWrite = it->second;

      if(! file_exists(currentWrite.get_path().string())){
        logger::get_logger()->error() 
            << "File Error: Cannot Find file for write" << currentWrite.get_use_data_product();

        throw std::runtime_error("File Error Cannot Find file for write: " + currentWrite.get_use_data_product());
      }

      Json::Value storageData;
      storageData["hash"] = calculate_hash_from_file(currentWrite.get_path());
      storageData["storage_root"] = config_storage_root_->get_id();
      storageData["public"] = currentWrite.is_public();


      ApiObject::sptr storageLocationObj = ApiObject::from_json(api_->get_by_json_query("storage_location", storageData)[0]);
      ApiObject::sptr StorageRootObj;

      ghc::filesystem::path newPath;
      std::string extension = currentWrite.get_path().extension().string();

      if (!storageLocationObj->is_empty()){
        remove(currentWrite.get_path());

        StorageRootObj = ApiObject::from_json(api_->get_by_id("storage_root", ApiObject::get_id_from_string(storageLocationObj->get_value_as_string("storage_root"))));

        newPath = ghc::filesystem::path(remove_local_from_root(StorageRootObj->get_value_as_string("root"))) / storageLocationObj->get_value_as_string("path");

      }
      else {
        ghc::filesystem::path tmpFilename = currentWrite.get_path().filename();
        ghc::filesystem::path newFileName = ghc::filesystem::path(storageData["hash"].asString() + extension);

        newPath = ghc::filesystem::path(remove_local_from_root(get_data_store().string())) / currentWrite.get_use_namespace() / currentWrite.get_use_data_product() / newFileName;

        ghc::filesystem::rename(currentWrite.get_path().string(), newPath.string());

        ghc::filesystem::path str_path =  ghc::filesystem::path(currentWrite.get_use_namespace()) / currentWrite.get_use_data_product() / newFileName;

        storageData["path"] = str_path.string();
        storageData["path"] = remove_backslash_from_path(storageData["path"].asString());
        storageData["path"] = API::remove_leading_forward_slash(storageData["path"].asString());
        storageData["storage_root"] = config_storage_root_->get_uri();

        storageLocationObj = ApiObject::from_json(api_->post("storage_location", storageData, token_));

      }

      Json::Value filetypeData;
      filetypeData["name"] = extension;
      filetypeData["extension"] = extension;
      ApiObject::sptr filetypeObj = ApiObject::from_json(api_->post("file_type", filetypeData, token_));

      Json::Value namespaceData;
      namespaceData["name"] = currentWrite.get_use_namespace();

      ApiObject::sptr  namespaceObj = ApiObject::from_json( api_->get_by_json_query("namespace", namespaceData)[0]);
      if (namespaceObj->is_empty()){
        namespaceObj = ApiObject::from_json(api_->post("namespace", namespaceData, token_));
      }

      Json::Value dataproductData;
      dataproductData["name"] = currentWrite.get_use_data_product();
      dataproductData["version"] = currentWrite.get_use_version();
      dataproductData["namespace"] = namespaceObj->get_uri();

      Json::Value j_dataProd = api_->get_by_json_query("data_product", dataproductData)[0];

      ApiObject::sptr dataProductObj = ApiObject::from_json( j_dataProd );
      ApiObject::sptr obj;
      std::string componentUrl;

      if(!dataProductObj->is_empty()){
          Json::Value _j_ = api_->get_by_id("object", ApiObject::get_id_from_string(dataProductObj->get_value_as_string("object")));
        obj = ApiObject::from_json( _j_ );
        componentUrl = obj->get_first_component();
      }
      else{
        Json::Value objData;
        objData["description"] = currentWrite.get_data_product_description();
        objData["storage_location"] = storageLocationObj->get_uri();
        Json::Value author_id_ = author_->get_uri();
        objData["authors"].append(author_id_);
        objData["file_type"] = filetypeObj->get_uri();
        
        Json::Value j_tmp_obj = api_->post("object", objData, token_);
        obj = ApiObject::from_json( j_tmp_obj );

        if (currentWrite.get_use_component() != "None"){
          //@todo allow use_component
          componentUrl = obj->get_first_component();
        }
        else{        
          componentUrl = obj->get_first_component();
        }

        dataproductData["object"] = obj->get_uri();

        
        Json::Value j_tmp = api_->post("data_product", dataproductData, token_);
        dataProductObj = ApiObject::from_json( j_tmp);

      }

      Json::Value j_componen_obj = api_->get_by_id("object_component", ApiObject::get_id_from_string(componentUrl));
      ApiObject::sptr  componentObj = ApiObject::from_json( j_componen_obj );

      currentWrite.set_component_object( *componentObj);
      currentWrite.set_data_product_object( *dataProductObj );

      outputs_[currentWrite.get_data_product()] = currentWrite;      

    }
  }

  if(has_reads()){
    map_type::iterator it;
    for (it = reads_.begin(); it != reads_.end(); it++){
      IOObject& currentRead = it->second;
      inputs_[currentRead.get_data_product()] = currentRead;
    }
  }

  Json::Value patch_data;

  if(has_outputs()){
    map_type::iterator it;
    for (it = outputs_.begin(); it != outputs_.end(); it++){
      IOObject& currentOutput = it->second;
      Json::Value output = currentOutput.get_component_object()->get_uri();
      patch_data["outputs"].append(output);
      //logger::get_logger()->info() 
      //<< "Writing 
      //<< currentOutput.get_use_data_product()
      //<< " to local registry";
      logger::get_logger()->info() 
          << "Writing " <<  currentOutput.get_use_data_product() << " to local registry";

    }
  }

  if(has_inputs()){
   map_type::iterator it;
    for (it = inputs_.begin(); it != inputs_.end(); it++){
      IOObject& currentInput = it->second;
      Json::Value input = currentInput.get_component_object()->get_uri();
      patch_data["inputs"].append(input);
      logger::get_logger()->info() 
          << "Writing " <<  currentInput.get_use_data_product() << " to local registry";
    }
  }

  std::string code_run_endpoint = "code_run/" + std::to_string(code_run_->get_id());
  logger::get_logger()->info() << "Code Run: " << code_run_endpoint;

  Json::Value j_code_run = api_->patch(code_run_endpoint, patch_data, token_);
  this-> code_run_ = ApiObject::from_json( j_code_run );

}

}; // namespace FairDataPipeline
