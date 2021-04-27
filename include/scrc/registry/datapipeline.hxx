#ifndef __SCRC_DATAPIPELINE_HXX__
#define __SCRC_DATAPIPELINE_HXX__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "json/json.h"

#include "scrc/objects/metadata.hxx"
#include "scrc/registry/api.hxx"
#include "scrc/registry/data_object.hxx"
#include "scrc/registry/file_system.hxx"
#include "scrc/utilities/logging.hxx"

namespace SCRC {
class DataPipelineImpl_ {
public:
  const LocalFileSystem *file_system;
  API *api = nullptr;
  const std::filesystem::path access_token_file_;

  DataPipelineImpl_(const std::filesystem::path &config_file_path,
                    const std::filesystem::path &access_token_file,
                    spdlog::level::level_enum log_level = spdlog::level::info,
                    RESTAPI api_location = RESTAPI::LOCAL)
      : file_system(new LocalFileSystem(config_file_path)),
        access_token_file_(access_token_file) {
    const YAML::Node config_data_ = file_system->get_config_data();

    if (!config_data_["run_metadata"]) {
      APILogger->error(
          "Failed to obtain run metadata, the key 'run_metadata' is not"
          " present in the configuration");
      throw config_parsing_error("No run metadata provided");
    }

    const YAML::Node run_metadata_ = config_data_["run_metadata"];

    if (api_location == RESTAPI::LOCAL &&
        !run_metadata_["local_data_registry_url"]) {
      APILogger->error("Could not determine URL for local repository, key "
                       "'local_data_registry_url' "
                       "is absent from configuration");
      throw config_parsing_error("Failed to read local repository url");
    }

    else if (api_location == RESTAPI::REMOTE &&
             !run_metadata_["remote_data_registry_url"]) {
      APILogger->error("Could not determine URL for remote repository, key "
                       "'remote_data_registry_url' "
                       "is absent from configuration");
      throw config_parsing_error("Failed to read remote repository url");
    }

    const YAML::Node api_ = (api_location == RESTAPI::LOCAL)
                                ? run_metadata_["local_data_registry_url"]
                                : run_metadata_["remote_data_registry_url"];
    const std::filesystem::path api_root_ = api_.as<std::string>();

    api = new API(api_root_);

    spdlog::set_default_logger(APILogger);
    APILogger->set_level(log_level);

    APILogger->info("\n[Configuration]\n\t- Config Path: {0}\n\t- API Root: "
                    "{1}\n\t- SCRC API Token File: {2}",
                    config_file_path.string(), api_root_.string(),
                    (access_token_file.empty()) ? "None"
                                                : access_token_file.string());
  }

  Json::Value fetch_all_objects();
  Json::Value fetch_all_namespaces();
  Json::Value fetch_object_by_id(int identifier);
  Json::Value fetch_all_data_products();
  Json::Value fetch_all_external_objects();
  Json::Value fetch_data_product_by_id(int identifier);
  Json::Value fetch_store_root_by_id(int identifier);
  Json::Value fetch_data_store_by_id(int identifier);
  Json::Value fetch_external_object_by_id(int identifier);
  std::filesystem::path
  download_data_product(ReadObject::DataProduct *data_product);
  std::filesystem::path
  download_external_object(ReadObject::ExternalObject *external_object);
  int get_id_from_namespace(std::string name_space);
  int get_id_from_path(std::filesystem::path path);

  void push_new_coderun();
  void add_to_register(const std::string &alias);
  void register_external_object(const YAML::Node &register_entry);
  std::string new_source(const YAML::Node &data);
  std::string new_storage_root(const YAML::Node &data);
  std::string new_storage_location(const YAML::Node &data);
  std::string new_external_object(const YAML::Node &data);
  std::string read_key();
};

class DataProductQuery : public Query {
public:
  DataProductQuery()
      : Query("data_product", {"updated_by", "last_updated", "object",
                               "namespace", "name", "version"}) {}
  DataProductQuery(std::filesystem::path query_path)
      : Query("data_product", query_path) {}
};

class ObjectQuery : public Query {
public:
  ObjectQuery()
      : Query("object",
              {"components", "code_repo_of", "config_of",
               "submission_script_of", "external_object", "quality_control",
               "keywords", "authors", "licences", "data_product",
               "code_repo_release", "metadata", "updated_by", "last_updated",
               "storage_location", "description", "file_type", "issues"}) {}

  ObjectQuery(std::filesystem::path query_path) : Query("object", query_path) {}
};

class StorageLocationQuery : public Query {
public:
  StorageLocationQuery()
      : Query("storage_location",
              {"location_for_object", "original_store_of", "updated_by",
               "last_updated", "path", "hash", "storage_root"}) {}

  StorageLocationQuery(std::filesystem::path query_path)
      : Query("storage_location", query_path) {}
};

class StorageRootQuery : public Query {
public:
  StorageRootQuery()
      : Query("storage_root", {"name", "root", "path", "accessibility"}) {}

  StorageRootQuery(std::filesystem::path query_path)
      : Query("storage_root", query_path) {}
};

class ExternalObjectQuery : public Query {
public:
  ExternalObjectQuery()
      : Query("external_object", {
                                     "doi_or_unique_name",
                                     "primary_not_supplement",
                                     "release_date",
                                     "title",
                                     "description",
                                     "version",
                                     "object",
                                     "source",
                                     "original_store",
                                 }) {}

  ExternalObjectQuery(std::filesystem::path query_path)
      : Query("external_object", query_path) {}
};

class NamespaceQuery : public Query {
public:
  NamespaceQuery() : Query("namespace", std::vector<std::string>{"name"}) {}

  NamespaceQuery(std::filesystem::path query_path)
      : Query("namespace", query_path) {}
};
}; // namespace SCRC

#endif