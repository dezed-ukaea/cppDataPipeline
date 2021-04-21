#include "scrc/registry/file_system.hxx"
namespace SCRC {
YAML::Node parse_yaml_(std::filesystem::path yaml_path) {
  APILogger->debug("LocalFileSystem: Reading configuration file '{0}'",
                   yaml_path.string().c_str());
  return YAML::LoadFile(yaml_path.c_str());
}

LocalFileSystem::LocalFileSystem(std::filesystem::path config_file_path)
    : config_path_(config_file_path),
      config_data_(parse_yaml_(config_file_path)) {
  APILogger->debug("LocalFileSystem: Parsing configuration file");
  const std::filesystem::path data_directory_path_ = get_data_store();

  // Create the data directory within the config file if it does not exist
  if (!std::filesystem::exists(data_directory_path_)) {
    APILogger->info("Creating data directory at '{0}'",
                    data_directory_path_.c_str());
    std::filesystem::create_directory(data_directory_path_);
  }
}

YAML::Node LocalFileSystem::meta_data_() const {
  return config_data_["run_metadata"];
}

std::filesystem::path LocalFileSystem::get_data_store() const {
  if (meta_data_()["default_data_store"]) {
    std::string data_dir_ =
        meta_data_()["default_data_store"].as<std::string>();

    // Handle case where tilde used for HOME for environment variable
    if (data_dir_[0] == '~') {
      data_dir_ =
          std::getenv("HOME") + data_dir_.substr(1, data_dir_.size() - 1);
    }

    // Handle case where '/' used meaning relative to config file path
    else if (data_dir_[0] == '/') {
      data_dir_ = config_path_.parent_path() /
                  data_dir_.substr(1, data_dir_.size() - 1);
    }

    return data_dir_;
  } else {
    return std::filesystem::path(std::getenv("HOME")) /
           std::filesystem::path("datastore");
  }
}

std::string LocalFileSystem::get_default_input_namespace() const {
  if (!meta_data_()["default_input_namespace"]) {
    throw config_parsing_error(
        "Cannot find value for expected key 'default_input_namespace'");
  }

  return meta_data_()["default_input_namespace"].as<std::string>();
}

std::string LocalFileSystem::get_default_output_namespace() const {
  if (!meta_data_()["default_output_namespace"]) {
    throw config_parsing_error(
        "Cannot find value for expected key 'default_output_namespace'");
  }

  return meta_data_()["default_output_namespace"].as<std::string>();
}

std::string get_first_key_(const toml::value data_table) {
  for (const auto &[k, v] : data_table.as_table()) {
    return k;
  }

  return "";
}

std::vector<ReadObject::DataProduct *>
LocalFileSystem::read_data_products() const {
  APILogger->debug("LocalFileSystem:ReadDataProducts: Reading data products list");

  std::vector<ReadObject::DataProduct *> data_products_;
  const YAML::Node read_node = config_data_["read"];

  APILogger->debug("LocalFileSystem:ReadDataProducts: Iterating through entries");
  for (auto &item : read_node) {
    if(item["data_product"]) {
      ReadObject::DataProduct* object_ = ReadObject::data_product_from_yaml(item);
      if(!object_)
      {
        APILogger->error("Failed to retrieve data product metadata for '{0}'", item["data_product"].as<std::string>());
        throw config_parsing_error("Data product metadata extraction failed");
      }
      data_products_.push_back(object_);
    }
  }

  if (data_products_.empty())
    throw config_parsing_error("No data products found in config");

  return data_products_;
}

std::vector<ReadObject::ExternalObject *>
LocalFileSystem::read_external_objects() const {
  APILogger->debug("LocalFileSystem:ReadExternalObjects: Reading external objects list");

  std::vector<ReadObject::ExternalObject *> external_objects_;
  const YAML::Node read_node = config_data_["read"];

  APILogger->debug("LocalFileSystem:ReadExternalObjects: Iterating through entries");
  for (auto &item : read_node) {
    if(item["external_object"])
    {
      ReadObject::ExternalObject* object_ = ReadObject::external_object_from_yaml(item);
      if(!object_)
      {
        APILogger->error("Failed to retrieve external object metadata for '{0}'", item["external_object"].as<std::string>());
        throw config_parsing_error("External object metadata extraction failed");
      }
      external_objects_.push_back(object_);
    }
  }

  if (external_objects_.empty())
    throw config_parsing_error("No data products found in config");

  return external_objects_;
}


double read_point_estimate_from_toml(const std::filesystem::path var_address) {
  if (!std::filesystem::exists(var_address)) {
    throw std::runtime_error("File '" + var_address.string() +
                             "' could not be opened as it does not exist");
  }

  const auto toml_data_ = toml::parse(var_address.string());

  const std::string first_key_ = get_first_key_(toml_data_);

  if (!toml::get<toml::table>(toml_data_).at(first_key_).contains("type")) {
    throw std::runtime_error("Expected 'type' tag but none found");
  }

  if (static_cast<std::string>(
          toml_data_.at(first_key_).at("type").as_string()) !=
      "point-estimate") {
    throw std::runtime_error(
        "Expected 'point-estimate' for type but got '" +
        static_cast<std::string>(toml_data_.at("type").as_string()) + "'");
  }

  return (toml_data_.at(first_key_).at("value").is_floating())
             ? toml_data_.at(first_key_).at("value").as_floating()
             : static_cast<double>(
                   toml_data_.at(first_key_).at("value").as_integer());
}

Distribution *construct_dis_(const toml::value data_table) {
  const std::string first_key_ = get_first_key_(data_table);
  const std::string dis_type_ =
      data_table.at(first_key_).at("distribution").as_string();

  if (dis_type_ == "normal") {
    const double mean =
        (data_table.at(first_key_).at("mu").is_floating())
            ? data_table.at(first_key_).at("mu").as_floating()
            : static_cast<double>(
                  data_table.at(first_key_).at("mu").as_integer());
    const double sd =
        (data_table.at(first_key_).at("sigma").is_floating())
            ? data_table.at(first_key_).at("sigma").as_floating()
            : static_cast<double>(
                  data_table.at(first_key_).at("sigma").as_integer());
    return new Normal(mean, sd);
  }

  else if (dis_type_ == "gamma") {
    const double k = (data_table.at(first_key_).at("k").is_floating())
                         ? data_table.at(first_key_).at("k").as_floating()
                         : static_cast<double>(
                               data_table.at(first_key_).at("k").as_integer());
    const double theta =
        (data_table.at(first_key_).at("theta").is_floating())
            ? data_table.at(first_key_).at("theta").as_floating()
            : static_cast<double>(
                  data_table.at(first_key_).at("theta").as_integer());
    return new Gamma(k, theta);
  }

  throw std::runtime_error("Distribution currently unsupported");
}

Distribution *
read_distribution_from_toml(const std::filesystem::path var_address) {
  if (!std::filesystem::exists(var_address)) {
    throw std::runtime_error("File '" + var_address.string() +
                             "' could not be opened as it does not exist");
  }

  const auto toml_data_ = toml::parse(var_address.string());

  const std::string first_key_ = get_first_key_(toml_data_);

  if (!toml::get<toml::table>(toml_data_).at(first_key_).contains("type")) {
    throw std::runtime_error("Expected 'type' tag but none found");
  }

  if (static_cast<std::string>(
          toml_data_.at(first_key_).at("type").as_string()) != "distribution") {
    throw std::runtime_error(
        "Expected 'distribution' for type but got '" +
        static_cast<std::string>(toml_data_.at("type").as_string()) + "'");
  }

  return construct_dis_(toml_data_);
}
}; // namespace SCRC