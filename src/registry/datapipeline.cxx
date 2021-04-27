#include "scrc/registry/datapipeline.hxx"

namespace SCRC {

Json::Value DataPipelineImpl_::fetch_all_objects() {
  APILogger->debug("DataPipelineImpl_: Fetching all objects from registry");
  return api->query(ObjectQuery(""));
}

Json::Value DataPipelineImpl_::fetch_all_data_products() {
  return api->query(DataProductQuery(""));
}

Json::Value DataPipelineImpl_::fetch_object_by_id(int identifier) {
  return api->query(ObjectQuery(std::to_string(identifier)));
}

Json::Value DataPipelineImpl_::fetch_data_product_by_id(int identifier) {
  return api->query(DataProductQuery(std::to_string(identifier)));
}

Json::Value DataPipelineImpl_::fetch_external_object_by_id(int identifier) {
  return api->query(ExternalObjectQuery(std::to_string(identifier)));
}

Json::Value DataPipelineImpl_::fetch_all_external_objects() {
  return api->query(ExternalObjectQuery(""));
}

Json::Value DataPipelineImpl_::fetch_all_namespaces() {
  return api->query(NamespaceQuery(""));
}

int DataPipelineImpl_::get_id_from_path(std::filesystem::path path) {
  return std::stoi(path.stem());
}

int DataPipelineImpl_::get_id_from_namespace(std::string name_space) {
  if (name_space.empty()) {
    throw std::invalid_argument(
        "Empty string given as name for namespace id retrieval");
  }

  NamespaceQuery qnamespace_ = NamespaceQuery();
  qnamespace_.append("name", name_space);
  Json::Value namespace_json_ = api->query(qnamespace_)[0];

  std::string url_str_ = namespace_json_["url"].asString();

  // Need to remove the trailing '/'
  const std::filesystem::path url_ = url_str_.substr(0, url_str_.size() - 1);

  if (url_.filename().empty()) {
    std::cout << url_.filename() << std::endl;
    throw json_parse_error("Failed to retrieve ID for namespace '" +
                           name_space + "'");
  }

  APILogger->debug("Retrieved ID '{0}' for namespace '{1}'",
                   url_.filename().string(), name_space);

  return std::stoi(url_.filename());
}

Json::Value DataPipelineImpl_::fetch_data_store_by_id(int identifier) {
  return api->query(StorageLocationQuery(std::to_string(identifier)));
}

Json::Value DataPipelineImpl_::fetch_store_root_by_id(int identifier) {
  return api->query(StorageRootQuery(std::to_string(identifier)));
}

std::filesystem::path DataPipelineImpl_::download_external_object(
    ReadObject::ExternalObject *external_object) {

  ExternalObjectQuery eoquery_ = ExternalObjectQuery();

  if (!external_object) {
    APILogger->error("DataPipeline:DownloadDataProduct: Could not download "
                     "data product due to metadata retrieval failure.");
    throw std::runtime_error("Attempt to access null pointer");
  }

  eoquery_.append("doi_or_unique_name", external_object->get_unique_id());

  const Json::Value res_ = api->query(eoquery_);

  if (res_.size() == 0) {
    throw rest_apiquery_error("No external object found for '" +
                              external_object->get_unique_id() + "'");
  }

  const std::string object_id_path_ = res_[0]["object"].asString();
  const std::string object_id_ =
      std::filesystem::path(
          object_id_path_.substr(0, object_id_path_.size() - 1))
          .filename();

  APILogger->debug("DataPipeline:ExternalObject: Retrieved ID '" + object_id_ +
                   "' for object '" + external_object->get_unique_id() + "'");

  Json::Value objres_ = fetch_object_by_id(std::stoi(object_id_));

  const std::string store_loc_ = objres_["storage_location"].asString();

  const std::string store_loc_id_ =
      std::filesystem::path(store_loc_.substr(0, store_loc_.size() - 1))
          .filename();

  const Json::Value store_res_ =
      fetch_data_store_by_id(std::stoi(store_loc_id_));

  const std::filesystem::path rel_file_loc_ = store_res_["path"].asString();
  const std::string hash_ = store_res_["hash"].asString();

  const std::string root_loc_str_ = store_res_["storage_root"].asString();
  const std::string root_loc_id_ =
      std::filesystem::path(root_loc_str_.substr(0, root_loc_str_.size() - 1))
          .filename();
  const Json::Value root_res_ = fetch_store_root_by_id(std::stoi(root_loc_id_));
  const std::filesystem::path root_loc_ = root_res_["root"].asString();

  APILogger->debug("DataPipeline: File download location '{0}'",
                   (root_loc_ / rel_file_loc_).string());

  const std::filesystem::path download_loc_ =
      (file_system->get_data_store() / rel_file_loc_).parent_path();

  if (std::filesystem::exists(download_loc_ / rel_file_loc_.filename())) {
    const std::string existing_hash_ =
        calculate_hash_from_file(download_loc_ / rel_file_loc_.filename());

    APILogger->debug("DataPipeline:HASH:\n\tCalculated: {0}\n\tRemote: {1}",
                     existing_hash_, hash_);

    if (hash_ == existing_hash_) {
      APILogger->debug("DataPipeline: File '{0}' is already latest version "
                       "and will not be updated",
                       (download_loc_ / rel_file_loc_.filename()).string());
      return download_loc_ / rel_file_loc_.filename();
    }
  }

  // Create directories if they do not exist
  if (!std::filesystem::exists(download_loc_)) {
    std::filesystem::create_directories(download_loc_);
  }

  api->download_file(root_loc_ / rel_file_loc_,
                     download_loc_ / rel_file_loc_.filename());

  if (!std::filesystem::exists(download_loc_ / rel_file_loc_.filename())) {
    throw sync_error("Failed to download data file for '" +
                     external_object->get_unique_id() + ":" +
                     rel_file_loc_.string() + "'");
  }

  return download_loc_ / rel_file_loc_.filename();
}

std::filesystem::path DataPipelineImpl_::download_data_product(
    ReadObject::DataProduct *data_product) {

  if (!data_product) {
    APILogger->error("DataPipeline:DownloadDataProduct: Could not download "
                     "data product due to metadata retrieval failure.");
    throw std::runtime_error("Attempt to access null pointer");
  }

  std::string namespace_local_ = data_product->get_namespace();

  // Check if there is a namespace for the entry, else use the global default
  const std::string namespace_ =
      (namespace_local_.empty()) ? file_system->get_default_input_namespace()
                                 : namespace_local_;
  const int ns_id_ = get_id_from_namespace(namespace_);
  const std::string reg_path_ = data_product->get_path();

  DataProductQuery dpquery_ = DataProductQuery();
  dpquery_.append("namespace", std::to_string(ns_id_));
  dpquery_.append("name", reg_path_);

  const Json::Value res_ = api->query(dpquery_);

  if (res_.size() == 0) {
    throw rest_apiquery_error("No data product found for '" + namespace_ + ":" +
                              reg_path_ + "'");
  }

  const std::string object_id_path_ = res_[0]["object"].asString();
  const std::string object_id_ =
      std::filesystem::path(
          object_id_path_.substr(0, object_id_path_.size() - 1))
          .filename();

  APILogger->debug("DataPipeline:DataProduct: Retrieved ID '" + object_id_ +
                   "' for object '" + namespace_ + ":" + reg_path_ + "'");

  Json::Value objres_ = fetch_object_by_id(std::stoi(object_id_));

  const std::string store_loc_ = objres_["storage_location"].asString();
  const std::string store_loc_id_ =
      std::filesystem::path(store_loc_.substr(0, store_loc_.size() - 1))
          .filename();
  const Json::Value store_res_ =
      fetch_data_store_by_id(std::stoi(store_loc_id_));
  const std::filesystem::path rel_file_loc_ = store_res_["path"].asString();
  const std::string hash_ = store_res_["hash"].asString();

  const std::string root_loc_str_ = store_res_["storage_root"].asString();
  const std::string root_loc_id_ =
      std::filesystem::path(root_loc_str_.substr(0, root_loc_str_.size() - 1))
          .filename();
  const Json::Value root_res_ = fetch_store_root_by_id(std::stoi(root_loc_id_));
  const std::filesystem::path root_loc_ = root_res_["root"].asString();

  APILogger->debug("DataPipeline: File download location '{0}'",
                   (root_loc_ / rel_file_loc_).string());

  const std::filesystem::path download_loc_ = file_system->get_data_store() /
                                              namespace_ /
                                              std::filesystem::path(reg_path_);

  if (std::filesystem::exists(download_loc_ / rel_file_loc_.filename())) {
    const std::string existing_hash_ =
        calculate_hash_from_file(download_loc_ / rel_file_loc_.filename());

    APILogger->debug("DataPipeline:HASH:\n\tCalculated: {0}\n\tRemote: {1}",
                     existing_hash_, hash_);

    if (hash_ == existing_hash_) {
      APILogger->debug("DataPipeline: File '{0}' is already latest version "
                       "and will not be updated",
                       (download_loc_ / rel_file_loc_.filename()).string());
      return download_loc_ / rel_file_loc_.filename();
    }
  }

  // Create directories if they do not exist
  if (!std::filesystem::exists(download_loc_)) {
    std::filesystem::create_directories(download_loc_);
  }

  api->download_file(root_loc_ / rel_file_loc_,
                     download_loc_ / rel_file_loc_.filename());

  if (!std::filesystem::exists(download_loc_ / rel_file_loc_.filename())) {
    throw sync_error("Failed to download data file for '" + namespace_ + ":" +
                     rel_file_loc_.string() + "'");
  }

  return download_loc_ / rel_file_loc_.filename();
}

void DataPipelineImpl_::push_new_coderun() {
  const std::string time_stamp_ = current_time_stamp();
}

std::string DataPipelineImpl_::new_source(const YAML::Node &data) {
  Json::Value post_data_;
  post_data_["name"] = data["source_name"].as<std::string>();
  post_data_["abbreviation"] = data["source_abbreviation"].as<std::string>();
  post_data_["website"] = data["source_website"].as<std::string>();

  return api->post("source", post_data_, read_key())["url"].asString();
}

std::string DataPipelineImpl_::new_storage_root(const YAML::Node &data) {
  Json::Value post_data_;
  const RegisterObject::Accessibility access_ =
      RegisterObject::access_from_str(data["accessibility"].as<std::string>());
  post_data_["name"] = data["name"].as<std::string>();
  post_data_["root"] = data["root"].as<std::string>();
  post_data_["accessibility"] =
      std::to_string(int(access_ == RegisterObject::Accessibility::CLOSED));

  return api->post("storage_root", post_data_, read_key())["url"].asString();
}

std::string DataPipelineImpl_::new_storage_location(const YAML::Node &data) {
  Json::Value post_data_;
  post_data_["path"] = data["path"].as<std::string>();
  post_data_["hash"] = data["hash"].as<std::string>();
  post_data_["storage_root_id"] = data["storage_root_id"].as<std::string>();

  return api->post("storage_location", post_data_, read_key())["url"]
      .asString();
}

std::string DataPipelineImpl_::new_external_object(const YAML::Node &data) {
  Json::Value post_data_;
  post_data_["doi_or_unique_name"] =
      data["doi_or_unique_name"].as<std::string>();
  post_data_["release_date"] = data["release_date"].as<std::string>();
  post_data_["title"] = data["title"].as<std::string>();
  post_data_["description"] = data["description"].as<std::string>();
  post_data_["version"] = data["version"].as<std::string>();
  post_data_["object"] = data["object"].as<std::string>();
  post_data_["source"] = data["source"].as<std::string>();
  post_data_["original_store"] = data["original_store"].as<std::string>();

  return api->post("external_object", post_data_, read_key())["url"].asString();
}

std::string DataPipelineImpl_::read_key() {
  if (!std::filesystem::exists(access_token_file_)) {
    throw std::runtime_error("Failed to find Authorisation token: '" +
                             access_token_file_.string() + "'");
  }

  if (access_token_file_.empty()) {
    APILogger->warn("API:Post: No access token provided for RestAPI POST");
    return access_token_file_;
  }

  std::ifstream key_(access_token_file_, std::ios::in);
  std::string key_str_;
  key_ >> key_str_;
  key_.close();
  return key_str_;
}

void DataPipelineImpl_::register_external_object(
    const YAML::Node &register_entry) {

  APILogger->debug("DataPipeline: Registering external object");

  const std::string file_type_ = register_entry["file_type"].as<std::string>();
  const std::filesystem::path root_ = register_entry["root"].as<std::string>();
  const std::string product_name_ =
      register_entry["product_name"].as<std::string>();
  const std::filesystem::path local_dir_ =
      file_system->get_data_store() /
      file_system->get_default_output_namespace() / product_name_;
  const std::string time_stamp_ = current_time_stamp();
  const std::filesystem::path temp_file_ =
      calculate_hash_from_string(time_stamp_) + "." + file_type_;
  const std::string path_ = register_entry["path"].as<std::string>();

  if (path_.find("SELECT") != std::string::npos &&
      path_.find("WHERE") != std::string::npos) {
    if (!std::filesystem::exists(local_dir_)) {
      std::filesystem::create_directories(local_dir_);
    }

    const std::string encoded_path_ =
        root_ / std::filesystem::path(url_encode(path_));
    api->download_file(encoded_path_, local_dir_ / temp_file_);
  }

  const std::string file_hash_ =
      calculate_hash_from_file(local_dir_ / temp_file_);

  APILogger->debug("Calculated new Hash: {0}", file_hash_);

  const std::filesystem::path new_filename_ = file_hash_ + "." + file_type_;

  const std::string source_id_ = new_source(register_entry);

  const RegisterObject::Accessibility access_ = RegisterObject::access_from_str(
      register_entry["accessibility"].as<std::string>());

  APILogger->debug("Storing data");

  const std::string source_root_id_ = new_storage_root(register_entry);

  const std::string storage_location_id_ = new_storage_location(register_entry);

  const std::string external_object_id = new_external_object(register_entry);

  APILogger->info("Added '{0}' as '{1}' to registry",
                  register_entry["external_object"].as<std::string>(),
                  external_object_id);
}

void DataPipelineImpl_::add_to_register(const std::string &alias) {
  const YAML::Node config_data_ = file_system->get_config_data();

  APILogger->info("Registering item '{0}'", alias);

  if (!config_data_["register"]) {
    throw config_parsing_error(
        "Expected key 'register' not found in configuration.");
  }

  const YAML::Node register_ = config_data_["register"];

  APILogger->debug("Iterating through registration items.");

  for (YAML::const_iterator it = register_.begin(); it != register_.end();
       ++it) {
    const YAML::Node to_register_(*it);
    for (const std::string &s : {"external_object", "data_product", "object"}) {
      if (to_register_[s] && to_register_[s].as<std::string>() == alias) {

        if (s == "external_object") {
          register_external_object(to_register_);
        }

        APILogger->debug("Found entry '{0}' of type '{1}'", alias, s);

        return;
      }
    }
  }

  throw config_parsing_error(
      "Failed to find an entry '" + alias +
      "' under a recognised key from "
      "[\"external_object\", \"data_product\", \"object\"]");
}
}; // namespace SCRC