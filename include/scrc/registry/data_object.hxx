/*! **************************************************************************
 * @file scrc/registry/data_object.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-05
 * @brief File containing RestAPI query objects
 *
 * This file contains classes to describe structures received from the RestAPI
 * and recognised by the local configuration file.
 ****************************************************************************/
#ifndef __SCRC_DATAOBJECT_HXX__
#define __SCRC_DATAOBJECT_HXX__

#include <ctime>
#include <exception>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "scrc/exceptions.hxx"
#include "scrc/utilities/doi.hxx"
#include "scrc/utilities/logging.hxx"
#include "scrc/utilities/semver.hxx"

namespace SCRC {
/*! **************************************************************************
 * @namespace SCRC::ReadObject
 * @brief Contains classes matching the structure of read statements within
 * the local API configuration
 * @author K. Zarebski (UKAEA)
 *
 ****************************************************************************/
namespace ReadObject {
/*! **************************************************************************
 * @class DataProduct
 * @brief class for reading data product entries within the local configuration
 * @author K. Zarebski (UKAEA)
 *
 * The DataProduct class has a structure that matches the YAML syntax within the
 * local configuration file. It provides an intermediate for storing the
 * information extracted from the config.
 ****************************************************************************/
class DataProduct {
private:
  const std::string namespace_;
  const version version_;
  const std::filesystem::path registry_path_;
  const std::filesystem::path cache_ = std::filesystem::path();
  const std::string title_ = "";

public:
  /*! ***********************************************************************
   * @brief Initialise a new data product instance
   * @author K. Zarebski (UKAEA)
   *
   * @param registry_path registry path of the data product
   * @param version semantic version number for the data product
   * @param name_space namespace containing the data product
   * @param title data product title
   * @param cache_path location within the cache of the local file system
   *************************************************************************/
  DataProduct(std::string registry_path, std::string version,
              std::string name_space = "", std::string title = "",
              std::filesystem::path cache_path = "")
      : registry_path_(registry_path), version_(version),
        namespace_(name_space), title_(title), cache_(cache_path) {}

  /*! **********************************************************************
   * @brief Get the registry path of the data product
   * @author K. Zarebski (UKAEA)
   * @return path of the data product within the registry
   * 
   ************************************************************************/
  std::filesystem::path get_path() const { return registry_path_; }

  /*! **********************************************************************
   * @brief Get the version of the data product as a version object
   * @author K. Zarebski (UKAEA)
   *
   * @return semantic version of the data product
   ************************************************************************/
  version get_version() const { return version_; }

  /*! **********************************************************************
   * @brief Get the namespace of the data product
   * @author K. Zarebski (UKAEA)
   *
   * @return namespace as a string
   ************************************************************************/
  std::string get_namespace() const { return namespace_; }

  /*! **********************************************************************
   * @brief Get the title of the data product
   * @author K. Zarebski (UKAEA)
   *
   * @return title as a string
   ************************************************************************/
  std::string get_title() const { return title_; }

  /*! **********************************************************************
   * @brief Get the local cache path of the data product
   * @author K. Zarebski (UKAEA)
   *
   * @return path of the data product in the local cache
   ************************************************************************/
  std::string get_cache_path() const { return cache_; }
};

/**
 * @class ExternalObject
 * @brief class for reading external object entries within the
 * local configuration read header
 * @author K. Zarebski (UKAEA)
 *
 * The ExternalObject class has a structure that matches the YAML syntax
 * within the local configuration file for reading of external objects.
 * It provides an intermediate for storing the information extracted
 * from the config.
 ****************************************************************************/
class ExternalObject {
private:
  const DOI *doi_ = nullptr;
  const std::string name_ = "";
  const std::string title_ = "";
  std::filesystem::path cache_location_ = std::filesystem::path();

public:
  /*! ***********************************************************************
   * @brief Construct a new External Object object
   * @author K. Zarebski (UKAEA)
   *
   * @param title external object title
   * @param doi the DOI of the external object if appropriate
   * @param name the unique name for the external object
   * @param cache_path location within the cache of the local file system
   *************************************************************************/
  ExternalObject(const std::string &title = "", const DOI *doi = nullptr,
                 const std::string &name = "",
                 std::filesystem::path cache_path = "")
      : title_(title), doi_(doi), name_(name), cache_location_(cache_path) {}

  /*! ***********************************************************************
   * @brief retrieve the unique identifier for the external object, this is
   * either the DOI or, if not applicable, the unique name
   * @author K. Zarebski (UKAEA)
   *
   * @return unique identifier for the external object
   *************************************************************************/
  std::string get_unique_id() const {
    if (doi_) {
      return doi_->to_string();
    }
    return name_;
  }
};

/*! *************************************************************************
 * @brief convert a data product entry within the configuration into a
 * DataProduct instance
 * @author K. Zarebski (UKAEA)
 *
 * @param yaml_data YAML data read from the configuration file
 * @return a pointer to the new data product instance
 ***************************************************************************/
DataProduct *data_product_from_yaml(YAML::Node yaml_data);

/*! *************************************************************************
 * @brief convert an external object entry within the configuration into an
 * ExternalObject instance
 * @author K. Zarebski (UKAEA)
 *
 * @param yaml_data YAML data read from the configuration file
 * @return a pointer to the new external object instance
 ***************************************************************************/
ExternalObject *external_object_from_yaml(YAML::Node yaml_data);
}; // namespace ReadObject

/*! *************************************************************************
 * @namespace SCRC::RegisterObject
 * @brief Contains classes matching the structure of register statements
 * within the local API configuration
 * @author K. Zarebski (UKAEA)
 * 
 ***************************************************************************/
namespace RegisterObject {
/*! *************************************************************************
 * @enum Accessibility
 * @brief options for specifying whether an object can be accessed publicly
 * @author K. Zarebski (UKAEA)
 * 
 ***************************************************************************/
enum Accessibility {
  OPEN,  /*!< object can be accessed publicly */
  CLOSED /*!< object is private and requires authorisation to be accessed */
};

/*! *************************************************************************
 * @brief convert string access statement to Accessibility
 * @author K. Zarebski (UKAEA)
 *
 * @param access accessibility as string
 * @return accessibility as Accessibility type
 ***************************************************************************/
Accessibility access_from_str(const std::string &access);

/*! *************************************************************************
 * @class ExternalObject
 * @brief class for reading external object entries within the
 * local configuration registration header
 * @author K. Zarebski (UKAEA)
 *
 * This class represents the metadata for the registration of an external
 * object within the registry, this matches the YAML syntax for the
 * local API configuration
 ***************************************************************************/
class ExternalObject {
private:
  const std::string source_name_ = "";
  const std::string source_abbreviation_ = "";
  const std::string source_website_ = "";

  const std::string root_name_ = "";
  const std::string root_url_ = "";

  const std::string title_ = "";
  const std::string description_ = "";
  const std::string unique_name_ = "";
  const std::filesystem::path product_name_ = "";
  const std::string file_type_ = "";
  const tm release_date_;
  const version version_ = version();
  const bool primary_ = true;
  const Accessibility accessibility_ = Accessibility::OPEN;

public:
  /*! ***********************************************************************
   * @brief construct an external object registration entry from the
   * local configuration YAML metadata
   * @author K. Zarebski (UKAEA)
   *
   * @param entry read from the local configuration register header
   *************************************************************************/
  ExternalObject(YAML::Node yaml_data);

  /*! ***********************************************************************
   * @brief retrieve the name of the source for this entry
   * @author K. Zarebski (UKAEA)
   *
   * @return source name as string
   *************************************************************************/
  std::string get_source_name() const { return source_name_; }

  /*! ***********************************************************************
   * @brief retrieve the abbreviation name of the source for this entry
   * @author K. Zarebski (UKAEA)
   *
   * @return abbreviation as string
   *************************************************************************/
  std::string get_source_abbreviation() const { return source_abbreviation_; }

  /*! ***********************************************************************
   * @brief retrieve the website URL for this entry
   * @author K. Zarebski (UKAEA)
   *
   * @return URL of the source website as string
   */
  std::string get_source_website() const { return source_website_; }
};
}; // namespace RegisterObject

/*! *************************************************************************
 * @namespace SCRC::WriteObject
 * @brief Contains classes matching the structure of write statements
 * within the local API configuration
 * @author K. Zarebski (UKAEA)
 * 
 ***************************************************************************/
namespace WriteObject {
class DataProduct {
private:
  const std::filesystem::path registry_path_;
  const std::string component_;
};
}; // namespace WriteObject
}; // namespace SCRC

#endif