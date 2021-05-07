#include "scrc/registry/data_object.hxx"

namespace SCRC {
namespace ReadObject {
DataProduct *data_product_from_yaml(YAML::Node yaml_data) {
  if (!yaml_data["use"]) {
    APILogger->error("Expected a 'use' key containing metadata for retrieving "
                     "the data product");
    throw config_parsing_error("Failed to determine data product metadata");
  }
  const YAML::Node use_node_ = yaml_data["use"];

  if (!use_node_) {
    std::string available_keys_ = "";
    throw config_parsing_error(
        "Expected 'use' key in data product entry, but none found");
  }

  const std::string version_ = use_node_["version"].as<std::string>();

  try {
    Versioning::version(version_);
  } catch (std::exception &e) {
    APILogger->error("Invalid version identifier '{0}'", version_);
    throw e;
  }

  const std::string file_path_ = yaml_data["data_product"].as<std::string>();
  const std::string name_space_ =
      (use_node_["namespace"]) ? use_node_["namespace"].as<std::string>() : "";

  return new DataProduct(file_path_, version_, name_space_);
}

ExternalObject *external_object_from_yaml(YAML::Node yaml_data) {
  if (!yaml_data["use"]) {
    APILogger->error("Expected a 'use' key containing metadata for retrieving "
                     "the external object");
    throw config_parsing_error("Failed to determine external object metadata");
  }
  const YAML::Node use_node_ = yaml_data["use"];
  DOI *version_ = nullptr;
  std::string unique_name_ = "";

  if (!use_node_["unique_name"] && !use_node_["doi"]) {
    APILogger->error(
        "Expected either key 'doi' or 'unique_name' for external object "
        "specifier in configuration");
    throw config_parsing_error("External object identifier retrieval failure");
  }

  if (use_node_["doi"]) {
    version_ = new DOI(doi_from_string(use_node_["doi"].as<std::string>()));
  }

  if (use_node_["unique_name"]) {
    unique_name_ = use_node_["unique_name"].as<std::string>();
  }

  const std::string title_ = (use_node_["title"])
                                 ? use_node_["title"].as<std::string>()
                                 : unique_name_;

  const std::filesystem::path cache_path_ =
      (use_node_["cache"]) ? use_node_["cache"].as<std::string>() : "";

  const std::string version_print_ = (version_) ? version_->to_string() : "";
  APILogger->debug("Found external object: {0}\n\t - Version: {1}\n\t - Unique "
                   "Name: {2}\n\t - Cache: {3}",
                   title_, version_print_, unique_name_, cache_path_.string());

  return new ExternalObject(title_, version_, unique_name_, cache_path_);
}
}; // namespace ReadObject

namespace RegisterObject {
Accessibility access_from_str(const std::string &access) {
  return Accessibility(int(access == "closed"));
}
}; // namespace RegisterObject

}; // namespace SCRC