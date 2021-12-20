/*! **************************************************************************
 * @file fdp/registry/datapipeline.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File containing classes for interaction with the FDP data pipeline
 *
 * The classes within this file form the core objects for handling interaction
 * with the FDP FAIR Data pipeline. They contain methods which parse
 * configurations and control calls to the RestAPI itself.
 ****************************************************************************/
#ifndef __FDP_DATAPIPELINE_HXX__
#define __FDP_DATAPIPELINE_HXX__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "json/json.h"

#include "fdp/objects/config.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/registry/api.hxx"
#include "fdp/registry/data_object.hxx"
#include "fdp/registry/file_system.hxx"
#include "fdp/utilities/logging.hxx"

namespace FDP {

/*! **************************************************************************
 * @class DataPipelineImpl_
 * @brief private pointer-to-implementation class containing all backend methods
 * @author K. Zarebski (UKAEA)
 *
 * This class performs all behind the scenes operations acting as the backend to
 * the user called FDP::DataPipeline class.
 * This structure has been chosen to allow for unit tests to be created to test
 * all methods including private ones.
 *
 * @warning The class should not be used directly during implementation,
 * but rather via an FDP::DataPipeline instance.
 *
 *****************************************************************************/
class DataPipelineImpl_ {
  private:
    Config *config_;

public:
  
  /*! *************************************************************************
   * @brief construct a DataPipelineImpl_ instance from configurations and setup
   * @author K. Zarebski (UKAEA)
   *
   * @param config_file_path location of the local configuration file
   * @param access_token_file API authorisation token where required
   * @param log_level level for the output logging statements
   * @param api_location whether to use local/remote RestAPI endpoint
   ***************************************************************************/
  DataPipelineImpl_(const std::filesystem::path &config_file_path,
                    const std::filesystem::path &file_system_path,
                    const std::string token,
                    spdlog::level::level_enum log_level = spdlog::level::info,
                    RESTAPI api_location = RESTAPI::LOCAL);

  /**
   * @brief Destroy the Data Pipeline Impl_ object
   * 
   */
  ~DataPipelineImpl_();
  
/*! ************************************************************************
   * @brief Fetch all objects from the RestAPI matching the category 'object'
   * @author K. Zarebski (UKAEA)
   *
   * @return a JSON object containing all results from the RestAPI query
   **************************************************************************/
  Json::Value fetch_all_objects();

  /*! ************************************************************************
   * @brief Fetch all objects from the RestAPI matching the
   * category 'namespace'
   * @author K. Zarebski (UKAEA)
   *
   * @return a JSON object containing all results from the RestAPI query
   **************************************************************************/
  Json::Value fetch_all_namespaces();

  /*! ************************************************************************
   * @brief send a request to the RestAPI for information on an 'object' by
   * giving its registered identifier
   * @author K. Zarebski (UKAEA)
   *
   * @param identifier id representing the object in the registry
   * @return Json::Value JSON object containing metadata for the object
   **************************************************************************/
  Json::Value fetch_object_by_id(int identifier);

  /*! ************************************************************************
   * @brief Fetch all objects from the RestAPI matching the
   * category 'data_product'
   * @author K. Zarebski (UKAEA)
   *
   * @return a JSON object containing all results from the RestAPI query
   **************************************************************************/
  Json::Value fetch_all_data_products();

  /*! ************************************************************************
   * @brief Fetch all objects from the RestAPI matching the
   * category 'external_object'
   * @author K. Zarebski (UKAEA)
   *
   * @return a JSON object containing all results from the RestAPI query
   **************************************************************************/
  Json::Value fetch_all_external_objects();

  /*! ************************************************************************
   * @brief send a request to the RestAPI for information on an 'data_product'
   * by giving its registered identifier
   * @author K. Zarebski (UKAEA)
   *
   * @param identifier id representing the data product in the registry
   * @return Json::Value JSON object containing metadata for the object
   **************************************************************************/
  Json::Value fetch_data_product_by_id(int identifier);

  /*! ************************************************************************
   * @brief send a request to the RestAPI for information on an 'storage_root'
   * by giving its registered identifier
   * @author K. Zarebski (UKAEA)
   *
   * @param identifier id representing the data store root in the registry
   * @return Json::Value JSON object containing metadata for the object
   **************************************************************************/
  Json::Value fetch_store_root_by_id(int identifier);

  /*! ************************************************************************
   * @brief send a request to the RestAPI for information on an
   *'storage_location' by giving its registered identifier
   * @author K. Zarebski (UKAEA)
   *
   * @param identifier id representing the data store in the registry
   * @return Json::Value JSON object containing metadata for the object
   **************************************************************************/
  Json::Value fetch_data_store_by_id(int identifier);

  /*! ************************************************************************
   * @brief send a request to the RestAPI for information on an
   *'external_object' by giving its registered identifier
   * @author K. Zarebski (UKAEA)
   *
   * @param identifier id representing the external object in the registry
   * @return Json::Value JSON object containing metadata for the object
   **************************************************************************/
  Json::Value fetch_external_object_by_id(int identifier);

  /*! ************************************************************************
   * @brief download a data product from the registry
   * @author K. Zarebski (UKAEA)
   *
   * @param data_product an instance of FDP::ReadObject::DataProduct
   **************************************************************************/
  std::filesystem::path
  download_data_product(ReadObject::DataProduct *data_product);

  /*! ************************************************************************
   * @brief download an external object from the registry
   * @author K. Zarebski (UKAEA)
   *
   * @param data_product an instance of FDP::ReadObject::ExternalObject
   **************************************************************************/
  std::filesystem::path
  download_external_object(ReadObject::ExternalObject *external_object);

  /*! ************************************************************************
   * @brief fetch the identifier for a given namespace by name
   * @author K. Zarebski (UKAEA)
   *
   * @param name_space the name of the namespace
   * @return the integer identifer for the namespace in the registry
   **************************************************************************/
  int get_id_from_namespace(std::string name_space);

  /*! ************************************************************************
   * @brief retrieve the identifier part of a RestAPI URL for an object
   * @author K. Zarebski (UKAEA)
   *
   * @param path the URL/path ending in the identifier
   * @return the integer identifer obtained from the path
   **************************************************************************/
  int get_id_from_path(std::filesystem::path path);

  /*! ************************************************************************
   * @brief register a new code run in the registry
   * @author K. Zarebski (UKAEA)
   * @todo this method is incomplete!
   **************************************************************************/
  void push_new_coderun();

  /*! ************************************************************************
   * @brief add an entry to the registry based on metadata
   * in the configuration
   * @author K. Zarebski (UKAEA)
   *
   * @param alias label for the object to upload
   **************************************************************************/
  void add_to_register(const std::string &alias);

  /*! ************************************************************************
   * @brief add an external object to the registry using the given metadata
   * @author K. Zarebski (UKAEA)
   *
   * @param register_entry YAML node containing the metadata
   **************************************************************************/
  void register_external_object(const YAML::Node &register_entry);

  /*! ************************************************************************
   * @brief add a new data source to the registry
   * @author K. Zarebski (UKAEA)
   *
   * @param data YAML node containing source metadata
   * @return the address of the newly created source object on the registry
   **************************************************************************/
  std::string new_source(const YAML::Node &data);

  /*! ************************************************************************
   * @brief add a new storage root to the registry
   * @author K. Zarebski (UKAEA)
   *
   * @param data YAML node containing source metadata
   * @return the address of the newly created storage root object on
   * the registry
   **************************************************************************/
  std::string new_storage_root(const YAML::Node &data);

  /*! ************************************************************************
   * @brief add a new storage location to the registry
   * @author K. Zarebski (UKAEA)
   *
   * @param data YAML node containing source metadata
   * @return the address of the newly created storage location object
   * on the registry
   **************************************************************************/
  std::string new_storage_location(const YAML::Node &data);

  /*! ************************************************************************
   * @brief add a new external object to the registry
   * @author K. Zarebski (UKAEA)
   *
   * @param data YAML node containing source metadata
   * @return the address of the newly created source object on the registry
   **************************************************************************/
  std::string new_external_object(const YAML::Node &data);

  /*! ************************************************************************
   * @brief read the authorisation key from the input token file
   * @author K. Zarebski (UKAEA)
   *
   * @return the key as a string
   **************************************************************************/

  std::string static read_token(const std::filesystem::path &token_path);
};

/*! **************************************************************************
 * @class DataProductQuery
 * @brief class for constructing the correct URL query string for a data
 * product query
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
class DataProductQuery : public Query {
public:
  /*! ************************************************************************
   * @brief construct an empty data product query object
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
  DataProductQuery()
      : Query("data_product", {"updated_by", "last_updated", "object",
                               "namespace", "name", "version"}) {}

  /*! ************************************************************************
   * @brief construct a data product directly from a query path
   * @author K. Zarebski (UKAEA)
   *
   * @param query_path suffix of API request URL containing query fragment
   **************************************************************************/
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

/*! **************************************************************************
 * @class StorageLocationQuery
 * @brief class for constructing the correct URL query string for a storage
 * location query
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
class StorageLocationQuery : public Query {
public:
  /*! ************************************************************************
   * @brief construct an empty storage location query object
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
  StorageLocationQuery()
      : Query("storage_location",
              {"location_for_object", "original_store_of", "updated_by",
               "last_updated", "path", "hash", "storage_root"}) {}

  /*! ************************************************************************
   * @brief construct a storage location query directly from a query path
   * @author K. Zarebski (UKAEA)
   *
   * @param query_path suffix of API request URL containing query fragment
   **************************************************************************/
  StorageLocationQuery(std::filesystem::path query_path)
      : Query("storage_location", query_path) {}
};

/*! **************************************************************************
 * @class StorageRootQuery
 * @brief class for constructing the correct URL query string for a storage
 * root query
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
class StorageRootQuery : public Query {
public:
  /*! ************************************************************************
   * @brief construct an empty storage location query object
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
  StorageRootQuery()
      : Query("storage_root", {"name", "root", "path", "accessibility"}) {}

  /*! ************************************************************************
   * @brief construct a storage root query directly from a query path
   * @author K. Zarebski (UKAEA)
   *
   * @param query_path suffix of API request URL containing query fragment
   **************************************************************************/
  StorageRootQuery(std::filesystem::path query_path)
      : Query("storage_root", query_path) {}
};

/*! **************************************************************************
 * @class ExternalObjectQuery
 * @brief class for constructing the correct URL query string for an external
 * object query
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
class ExternalObjectQuery : public Query {
public:
  /*! ************************************************************************
   * @brief construct an empty storage location query object
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
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

  /*! ************************************************************************
   * @brief construct an external object query directly from a query path
   * @author K. Zarebski (UKAEA)
   *
   * @param query_path suffix of API request URL containing query fragment
   **************************************************************************/
  ExternalObjectQuery(std::filesystem::path query_path)
      : Query("external_object", query_path) {}
};

/*! **************************************************************************
 * @class NamespaceQuery
 * @brief class for constructing the correct URL query string for a namespace
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
class NamespaceQuery : public Query {
public:
  /*! ************************************************************************
   * @brief construct an empty namespace query object
   * @author K. Zarebski (UKAEA)
   *
   **************************************************************************/
  NamespaceQuery() : Query("namespace", std::vector<std::string>{"name"}) {}

  /*! ************************************************************************
   * @brief construct a namespace query directly from a query path
   * @author K. Zarebski (UKAEA)
   *
   * @param query_path suffix of API request URL containing query fragment
   **************************************************************************/
  NamespaceQuery(std::filesystem::path query_path)
      : Query("namespace", query_path) {}
};
}; // namespace FDP

#endif