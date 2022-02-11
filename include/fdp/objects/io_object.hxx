#ifndef __FDP_IOOBJECT_HXX__
#define __FDP_IOOBJECT_HXX__

#include <cstddef>
#include <json/value.h>
#include <string>

#include "fdp/objects/api_object.hxx"
#include "fdp/utilities/logging.hxx"

namespace FDP {
/**
 * @brief Class for API objects
 *
 */
class IOObject {
private:
  std::string data_product_;
  std::string use_data_product_;
  std::string use_component_ = "None";
  std::string use_version_;
  std::string use_namespace_;
  ghc::filesystem::path path_;
  std::string data_product_description_ = "None";
  std::string component_description_ = "None";
  bool public_ = false;

  ApiObject::sptr component_obj_;
  ApiObject::sptr data_product_obj_;

public:
  /**
   * @brief Construct a new IOObject object (empty constructor)
   *
   */
  IOObject(){};
  /**
   * @brief Construct a new IOObject object
   * Used to store inputs and outputs for in the config
   *
   * @param data_product data_product name as string
   * @param use_data_product data_product name to be used as string
   * @param use_version data_product version to be used as string
   * @param use_namespace namespace to be used as string
   * @param path file path to data_product
   * @param data_product_description data product description
   * @param isPublic whether or not the data product should be public
   */
  IOObject(std::string data_product, std::string use_data_product,
           std::string use_version, std::string use_namespace,
           ghc::filesystem::path path, std::string data_product_description,
           bool isPublic)
      : data_product_(data_product), use_data_product_(use_data_product),
        use_version_(use_version), use_namespace_(use_namespace), path_(path),
        data_product_description_(data_product_description),
        public_(isPublic){};

  /**
   * @brief Construct a new IOObject object
   * Used to store inputs and outputs for in the config
   *
   * @param data_product data_product name as string
   * @param use_data_product data_product name to be used as string
   * @param use_version data_product version to be used as string
   * @param use_namespace namespace to be used as string
   * @param path file path to data_product
   * @param component_obj the data_product component object as a shared pointer
   * @param data_product_obj the data_product object as a shared pointer
   */
  IOObject(std::string data_product, std::string use_data_product,
           std::string use_version, std::string use_namespace,
           ghc::filesystem::path path, ApiObject &component_obj,
           ApiObject &data_product_obj)
      : data_product_(data_product), use_data_product_(use_data_product),
        use_version_(use_version), use_namespace_(use_namespace), path_(path),
        component_obj_(std::make_shared<ApiObject>(component_obj)),
        data_product_obj_(std::make_shared<ApiObject>(data_product_obj)){};

  /**
   * @brief Get the data product as a string
   *
   * @return std::string
   */
  std::string get_data_product() { return data_product_; }

  /**
   * @brief Get the use data product  as a string
   *
   * @return std::string
   */
  std::string get_use_data_product() { return use_data_product_; }

  /**
   * @brief Get the use component as a string
   *
   * @return std::string
   */
  std::string get_use_component() { return use_component_; }

  /**
   * @brief Get the use version  as a string
   *
   * @return std::string
   */
  std::string get_use_version() { return use_version_; }

  /**
   * @brief Get the use namespace as a string
   *
   * @return std::string
   */
  std::string get_use_namespace() { return use_namespace_; }

  /**
   * @brief Get the path of the data product
   *
   * @return ghc::filesystem::path
   */
  ghc::filesystem::path get_path() { return path_; }

  /**
   * @brief Get the data product description as a string
   *
   * @return std::string
   */
  std::string get_data_product_description() {
    return data_product_description_;
  }

  /**
   * @brief Get the component description as a string
   *
   * @return std::string
   */
  std::string get_component_description() { return component_description_; }

  /**
   * @brief Check whether the data product is public
   *
   * @return true the data product is public
   * @return false the data product is not public
   */
  bool is_public() { return public_; }

  /**
   * @brief Get the component object object
   *
   * @return std::shared_ptr<ApiObject>
   */
  ApiObject::sptr get_component_object() { return component_obj_; }

  /**
   * @brief Get the data product object object
   *
   * @return std::shared_ptr<ApiObject>
   */
  ApiObject::sptr get_data_product_object() { return data_product_obj_; }

  /**
   * @brief Set the component object object
   *
   * @param component_obj
   */
  void set_component_object(ApiObject &component_obj) {
    component_obj_ = std::make_shared<ApiObject>(component_obj);
  }

  /**
   * @brief Set the data product object object
   *
   * @param data_product_obj
   */
  void set_data_product_object(ApiObject &data_product_obj) {
    data_product_obj_ = std::make_shared<ApiObject>(data_product_obj);
  }
};
}; // namespace FDP

#endif
