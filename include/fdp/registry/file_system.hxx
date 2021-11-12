/*! **************************************************************************
 * @file fdp/registry/file_system.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File containing classes for interacting with the local file system
 *
 * The classes within this file contain methods for interacting with the
 * local file system. These include the reading of configurations, and
 * the reading/writing of data to locally.
 ****************************************************************************/
#ifndef __FDP_FILESYSTEM_HXX__
#define __FDP_FILESYSTEM_HXX__

#include "H5Cpp.h"
#include "toml.hpp"
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <iostream>

#include "fdp/objects/data.hxx"
#include "fdp/objects/distributions.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/registry/data_object.hxx"
#include "fdp/utilities/hdf5.hxx"
#include "fdp/utilities/logging.hxx"

using namespace H5; // Use the HDF5 library

// Define constants relating to object names/keys in the HDF5 file
#define TABLE "table" /*!< Key of table object */
#define ARRAY "array" /*!< Key of the array object */
#define COLUMN_UNITS                                                           \
  "column_units"              /*!< Key of the data table column units object */
#define ROW_NAMES "row_names" /*!< Key of the row names object */
#define DIMENSION_PREFIX                                                       \
  "Dimension_" /*!< Prefix for objects relating to array dimensions */
#define DIM_UNITS_SUFFIX                                                       \
  "_units" /*!< Suffix for objects relating to units of measurement */
#define DIM_NAME_SUFFIX "_names" /*!< Suffix for objects relating to names */
#define DIM_TITLE_SUFFIX                                                       \
  "_title" /*!< Suffix for objects relating to titles                          \
            */

namespace FDP {
/*! **************************************************************************
 * @brief parse the configuration file
 * @author K. Zarebski (UKAEA)
 *
 * @return a YAML::Node object containing all of the extracted metadata
 *****************************************************************************/
YAML::Node parse_config_();
/*! ***************************************************************************
 * @class LocalFileSystem
 * @brief class for handling interaction with the local file system
 *
 * This class contains methods for interacting with the configuration file and
 * retrieving the information required to read/write data objects
 *****************************************************************************/
class LocalFileSystem {
private:
  const std::filesystem::path config_path_;
  const YAML::Node config_data_;
  YAML::Node meta_data_() const;

public:
  /*! *************************************************************************
   * @brief construct a LocalFileSystem instance using the given configuration
   * @author K. Zarebski (UKAEA)
   *
   * @param config_file_path FDP Pipeline configuration YAML file path
   ***************************************************************************/
  LocalFileSystem(std::filesystem::path config_file_path);

  /*! *************************************************************************
   * @brief retrieve the data store location from the configuration file
   * @author K. Zarebski (UKAEA)
   *
   * @return path of the local data store folder
   ***************************************************************************/
  std::filesystem::path get_data_store() const;

  /*! *************************************************************************
   * @brief read all data product entries listed in the configuration file
   * @author K. Zarebski (UKAEA)
   *
   * @return objects representing the metadata for the read data products
   ***************************************************************************/
  std::vector<ReadObject::DataProduct *> read_data_products() const;

  /*! *************************************************************************
   * @brief read all external object entries listed in the configuration file
   * @author K. Zarebski (UKAEA)
   *
   * @return objects representing the metadata for the read external objects
   ***************************************************************************/
  std::vector<ReadObject::ExternalObject *> read_external_objects() const;

  /*! *************************************************************************
   * @brief retrieve the default input namespace from the configuration file
   * @author K. Zarebski (UKAEA)
   *
   * @return the default input namespace
   ***************************************************************************/
  std::string get_default_input_namespace() const;

  /*! *************************************************************************
   * @brief retrieve the default output namespace from the configuration file
   * @author K. Zarebski (UKAEA)
   *
   * @return the default output namespace
   ***************************************************************************/
  std::string get_default_output_namespace() const;

  /*! *************************************************************************
   * @brief retrieve all configurations from the configuration file
   * @author K. Zarebski (UKAEA)
   *
   * @return a YAML::Node object containing all configuration entries
   ***************************************************************************/
  YAML::Node get_config_data() const { return config_data_; }
};

/*! ***************************************************************************
 * @brief retrieves the first key from the given TOML data
 * @author K. Zarebski (UKAEA)
 *
 * @param data_table data read from a TOML file
 * @return std::string the first key within the given data set
 *****************************************************************************/
std::string get_first_key_(const toml::value data_table);

/*! ***************************************************************************
 * @brief construct a Distribution from the given extracted TOML data
 * @author K. Zarebski (UKAEA)
 *
 * @param data_table data read from a TOML file
 * @return distribution object matching the specification
 *****************************************************************************/
Distribution *construct_dis_(const toml::value data_table);

/*! ***************************************************************************
 * @brief read the value of a point estimate from a given TOML file
 * @author K. Zarebski (UKAEA)
 *
 * @param var_address file path for the input TOML file
 * @return the extracted point estimate value
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestTOMLPERead
 *
 *    This unit test checks that a point estimate can be read from a TOML file
 *    successfully
 *    @snippet `test/test_filesystem.cxx TestTOMLPERead
 *
 *****************************************************************************/
double read_point_estimate_from_toml(const std::filesystem::path var_address);

/*! ***************************************************************************
 * @brief read parameters for a distribution from a given TOML file and
 * construct a Distribution object
 * @author K. Zarebski (UKAEA)
 *
 * @param var_address file path for the input TOML file
 * @return the distribution created from the specified parameters
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestTOMLDisRead
 *
 *    This unit test checks that a distribution can be read from a TOML file
 *    successfully
 *    @snippet `test/test_filesystem.cxx TestTOMLDisRead
 *
 *****************************************************************************/
Distribution *
read_distribution_from_toml(const std::filesystem::path var_address);

/*! ***************************************************************************
 * @brief Create a point estimate and save it to a TOML file
 * @author K. Zarebski (UKAEA)
 *
 * @tparam T type of the input value
 * @param value the value to save as a point estimate
 * @param data_product the address/label for the created data product
 * @param version_num the version associated with the value
 * @param file_system a local file system instance to use for writing
 * @return std::filesystem::path the output file location
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestWritePointEstimate
 *
 *    This unit test checks that a point estimate is successfully written
 *    to a version labelled TOML file
 *    @snippet `test/test_filesystem.cxx TestWritePointEstimate
 *
 *****************************************************************************/
template <typename T>
std::filesystem::path create_estimate(T &value,
                                      const std::filesystem::path &data_product,
                                      const Versioning::version &version_num,
                                      const LocalFileSystem *file_system) {
  const std::string param_name_ = data_product.stem().string();
  const std::string namespace_ = file_system->get_default_output_namespace();
  const std::filesystem::path data_store_ = file_system->get_data_store();
  const toml::value data_{
      {param_name_, {{"type", "point-estimate"}, {"value", value}}}};

  const std::filesystem::path output_filename_ =
      data_store_ / namespace_ / data_product.parent_path() /
      std::string(version_num.to_string() + ".toml");
  std::ofstream toml_out_;

  if (!std::filesystem::exists(output_filename_.parent_path())) {
    std::filesystem::create_directories(output_filename_.parent_path());
  }

  toml_out_.open(output_filename_);

  if (!toml_out_) {
    throw std::runtime_error("Failed to open TOML file for writing");
  }

  toml_out_ << toml::format(data_);

  toml_out_.close();

  APILogger->debug("FileSystem:CreateEstimate: Wrote point estimate to '{0}'",
                   output_filename_.string());

  return output_filename_;
}

/*! ***************************************************************************
 * @brief Save a constructed distribution to a TOML file
 * @author K. Zarebski (UKAEA)
 *
 * @param distribution the distribution object to convert
 * @param data_product the address/label for the created data product
 * @param version_num the version associated with the value
 * @param file_system a local file system instance to use for writing
 * @return std::filesystem::path the output file location
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestWriteDistribution
 *
 *    This unit test checks that a distribution is successfully written
 *    to a version labelled TOML file
 *    @snippet `test/test_filesystem.cxx TestWriteDistribution
 *
 *****************************************************************************/
std::filesystem::path create_distribution(
    const Distribution *distribution, const std::filesystem::path &data_product,
    const Versioning::version &version_num, const LocalFileSystem *file_system);

/*! ***************************************************************************
 * @brief Save a constructed data table to a HDF5 file
 * @author K. Zarebski (UKAEA)
 *
 * @param table the data table to store
 * @param data_product the address/label for the created data product
 * @param component the name of the component/key to save to within the file
 * @param file_system a local file system instance to use for writing
 * @return std::filesystem::path the output file location
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestWriteTable
 *
 *    This unit test checks that a data table is successfully written
 *    to a version labelled HDF5 file alongside the relevant metadata
 *    @snippet `test/test_filesystem.cxx TestWriteTable
 *
 *****************************************************************************/
std::filesystem::path create_table(const DataTable *table,
                                   const std::filesystem::path &data_product,
                                   const std::filesystem::path &component,
                                   const LocalFileSystem *file_system);

/*! ***************************************************************************
 * @brief Save a constructed array to a HDF5 file
 * @author K. Zarebski (UKAEA)
 *
 * @tparam T the type of the array member values
 * @param array values for saving to the file
 * @param data_product the address/label for the created data product
 * @param component the name of the component/key to save to within the file
 * @param file_system a local file system instance to use for writing
 * @return std::filesystem::path the output file location
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestWriteArray
 *
 *    This unit test checks that an array is successfully written
 *    to a version labelled HDF5 file alongside the relevant metadata
 *    @snippet `test/test_filesystem.cxx TestWriteArray
 *
 *****************************************************************************/
template <typename T>
std::filesystem::path create_array(const ArrayObject<T> *array,
                                   const std::filesystem::path &data_product,
                                   const std::filesystem::path &component,
                                   const LocalFileSystem *file_system) {
  const PredType *dtype_ = HDF5::get_hdf5_type<T>();

  const std::filesystem::path name_space_ =
      file_system->get_default_output_namespace();
  const std::filesystem::path data_store_ = file_system->get_data_store();

  const std::string output_file_name_ = current_time_stamp(true) + ".h5";
  const std::filesystem::path output_dir_ =
      data_store_ / name_space_ / data_product;
  const std::filesystem::path output_path_ = output_dir_ / output_file_name_;
  const std::string arr_name_ = std::string(ARRAY);

  if (output_path_.extension() != ".h5") {
    throw std::invalid_argument("Output file name for array must be HDF5");
  }

  if (!std::filesystem::exists(output_dir_))
    std::filesystem::create_directories(output_dir_);

  H5File *output_file_ = new H5File(output_path_.string().c_str(), H5F_ACC_TRUNC);

  APILogger->debug("FileSystem:CreateArray: Writing Group '{0}' to file",
                   component.string());

  Group *output_group_ =
      new Group(output_file_->createGroup(component.string().c_str()));

  const std::vector<int> dimensions_ = array->get_dimensions();

  hsize_t *dim_ = new hsize_t[dimensions_.size()];

  std::copy(dimensions_.begin(), dimensions_.end(), dim_);

  // ---------- WRITE ARRAY --------- //

  DataSpace *dspace_ = new DataSpace(dimensions_.size(), dim_);

  DataSet *dset_ = nullptr;

  if (*dtype_ == PredType::NATIVE_INT) {
    IntType out_type_(*dtype_);
    out_type_.setOrder(H5T_ORDER_LE);
    dset_ = new DataSet(
        output_group_->createDataSet(arr_name_.c_str(), out_type_, *dspace_));
  } else {
    FloatType out_type_(*dtype_);
    out_type_.setOrder(H5T_ORDER_LE);
    dset_ = new DataSet(
        output_group_->createDataSet(arr_name_.c_str(), out_type_, *dspace_));
  }

  int size_ = 1;

  for (const int &i : dimensions_)
    size_ *= i;

  T *data_ = new T[size_];

  for (int i{0}; i < size_; ++i) {
    data_[i] = array->get_values()[i];
  }

  dset_->write(data_, *dtype_);

  dspace_->close();
  dset_->close();

  delete[] data_;

  APILogger->debug("FileSystem:CreateArray: Wrote array object.");

  // ---------- WRITE TITLES ------------- //

  for (int i{0}; i < dimensions_.size(); ++i) {
    const hsize_t str_dim_[1] = {1};
    const int rank = 1;
    DataSpace *str_space_ = new DataSpace(rank, str_dim_);
    const std::string title_ = array->get_title(i);
    const std::string label_ = std::string(DIMENSION_PREFIX) +
                               std::to_string(i + 1) +
                               std::string(DIM_TITLE_SUFFIX);

    char* out_title_ = new char[title_.length() + 1];

    strcpy_s(out_title_, title_.size() + 1, title_.c_str());

    StrType stype_(H5T_C_S1, title_.length() + 1);

    DataSet *dset_str_ = new DataSet(
        output_group_->createDataSet(label_.c_str(), stype_, *str_space_));
    dset_str_->write(out_title_, stype_);

    dset_str_->close();
    str_space_->close();
  }

  APILogger->debug("FileSystem:CreateArray: Wrote dimension titles.");

  // ---------- WRITE NAMES --------- //

  for (int i{0}; i < dimensions_.size(); ++i) {
    const std::vector<std::string> names_ = array->get_dimension_names(i);

    std::vector<const char *> names_c_;

    for (const std::string &name : names_)
      names_c_.push_back(name.c_str());

    const hsize_t str_dim_[1] = {names_c_.size()};

    const int rank = 1;
    DataSpace *str_space_ = new DataSpace(rank, str_dim_);
    const std::string label_ = "Dimension_" + std::to_string(i + 1) + "_names";

    StrType stype_(H5T_C_S1, H5T_VARIABLE);

    DataSet *dset_str_ = new DataSet(
        output_group_->createDataSet(label_.c_str(), stype_, *str_space_));

    dset_str_->write(names_c_.data(), stype_);

    dset_str_->close();
    str_space_->close();
  }

  APILogger->debug("FileSystem:CreateArray: Wrote dimension names.");

  output_file_->close();

  APILogger->debug("FileSystem:CreateArray: Wrote file '{0}'",
                   output_path_.string());

  return output_path_;
}

/*! *****************************************************************
 * @brief read an array from a HDF5 file
 * @author K. Zarebski (UKAEA)
 *
 * @tparam T the type of the array values
 * @param var_address the address of the HDF5 file
 * @param key the key of the array entry within the file
 * @return an array object representing the array and its metadata
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestArrayRead
 *
 *    This unit test checks that an array and metadata are
 *    successfully read from a HDF5 file into an ArrayObject
 *    @snippet `test/test_filesystem.cxx TestArrayRead
 *
 *******************************************************************/
template <typename T>
ArrayObject<T> *read_array(const std::filesystem::path var_address,
                           const std::filesystem::path key) {
  APILogger->debug("FileSystem:ReadArray: Opening file '{0}'",
                   var_address.string());
  if (!std::filesystem::exists(var_address)) {
    throw std::runtime_error("Could not open HDF5 file '" +
                             var_address.string() + "', file does not exist.");
  }

  const H5File *file_ = new H5File(var_address.string().c_str(), H5F_ACC_RDONLY);

  // ------------- ARRAY RETRIEVAL -------------- //

  const std::string array_key_ = key.string() + "/" + std::string(ARRAY);
  APILogger->debug("FileSystem:ReadArray: Reading key '{0}'", array_key_);

  DataSet *data_set_ = new DataSet(file_->openDataSet(array_key_.c_str()));
  DataSpace *data_space_ = new DataSpace(data_set_->getSpace());

  int arr_dims_ = data_space_->getSimpleExtentNdims();

  hsize_t *dim_ = new hsize_t [arr_dims_];

  data_space_->getSimpleExtentDims(dim_, NULL);

  hsize_t mem_dim_ = 1;

  for (int i{0}; i < arr_dims_; ++i)
    mem_dim_ *= dim_[i];

  std::vector<T> data_out_(mem_dim_);

  const DataType &dtype_ = *HDF5::get_hdf5_type<T>();

  data_set_->read(data_out_.data(), dtype_, *data_space_, *data_space_);

  data_space_->close();
  data_set_->close();

  delete data_space_;
  delete data_set_;

  // -------------- TITLE RETRIEVAL ------------- //

  const int n_titles_ = arr_dims_;

  std::vector<std::string> titles_;

  for (int i{0}; i < n_titles_; ++i) {
    const std::string title_key_ =
        key.string() + "/" + std::string(DIMENSION_PREFIX) +
        std::to_string(i + 1) + std::string(DIM_TITLE_SUFFIX);
    titles_.push_back(
        HDF5::read_hdf5_object<std::string>(var_address, title_key_));
  }

  // -------------- NAMES RETRIEVAL ------------- //

  const int n_name_sets_ = arr_dims_;

  std::vector<std::vector<std::string>> names_;

  for (int i{0}; i < n_name_sets_; ++i) {
    const std::string names_key_ =
        key.string() + "/" + std::string(DIMENSION_PREFIX) +
        std::to_string(i + 1) + std::string(DIM_NAME_SUFFIX);
    const std::vector<std::string> names_i_str_ =
        HDF5::read_hdf5_as_str_vector(var_address, names_key_);

    names_.push_back(names_i_str_);
  }

  delete file_;

  std::vector<int> dimensions_;

  // for (auto &d : *dim_)
  //   dimensions_.push_back(d);

  for (int i{0}; i < arr_dims_; ++i)
    dimensions_.push_back(dim_[i]);

  APILogger->debug("FileSystem:ReadArray: Read Successful.");

  return new ArrayObject<T>{titles_, names_, dimensions_, data_out_};
}

/*! *****************************************************************
 * @brief read a column from a CompType/table within a HDF5 file
 * @author K. Zarebski (UKAEA)
 *
 * @tparam T the type of the column values
 * @param var_address the address of the HDF5 file
 * @param key the key of the array entry within the file
 * @param column the header of the requested column
 * @return an array object representing the array and its metadata
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestTableReadColumn
 *
 *    This unit test checks that a table column and metadata are
 *    successfully read from a HDF5 file into an DataTableColumn
 *    @snippet `test/test_filesystem.cxx TestTableReadColumn
 *
 *******************************************************************/
template <typename T>
DataTableColumn<T> *read_table_column(const std::filesystem::path var_address,
                                      const std::filesystem::path key,
                                      const std::string column) {
  APILogger->debug("FileSystem:ReadArray: Opening file '{0}'",
                   var_address.string());
  if (!std::filesystem::exists(var_address)) {
    throw std::runtime_error("Could not open HDF5 file '" +
                             var_address.string() + "', file does not exist.");
  }

  const H5File *file_ = new H5File(var_address.string().c_str(), H5F_ACC_RDONLY);

  // ------------ COLUMN RETRIEVAL ----------- //

  const std::string array_key_ = key.string() + "/" + std::string(TABLE);

  const std::vector<T> container_ =
      HDF5::read_hdf5_comp_type_member<T>(var_address, array_key_, column);

  // ------------ UNITS RETRIEVAL ------------ //

  const std::string units_key_ = key.string() + "/" + std::string(COLUMN_UNITS);

  const int col_index_ =
      HDF5::get_comptype(var_address, array_key_)->getMemberIndex(column);

  const std::vector<std::string> all_units_ =
      HDF5::read_hdf5_as_str_vector(var_address, units_key_);

  if (col_index_ >= all_units_.size()) {
    throw std::runtime_error(
        "Failed to retrieve units for column '" + column +
        "', retrieved index location outside memory allocation");
  }

  const std::string unit_ = all_units_[col_index_];

  // ------ ROW NAMES RETRIEVAL ---- //

  const std::string row_names_key_ =
      key.string() + "/" + std::string(ROW_NAMES);

  const std::vector<std::string> row_names_ =
      HDF5::read_hdf5_as_str_vector(var_address, row_names_key_);

  APILogger->debug("FileSystem:ReadTableColumn: Read Successful.");

  return new DataTableColumn<T>(column, unit_, row_names_, container_);
}
}; // namespace FDP

#endif