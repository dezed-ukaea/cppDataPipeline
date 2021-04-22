#ifndef __SCRC_FILESYSTEM_HXX__
#define __SCRC_FILESYSTEM_HXX__

#include "H5Cpp.h"
#include "toml.hpp"
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#include <iostream>

#include "scrc/objects/data.hxx"
#include "scrc/objects/distributions.hxx"
#include "scrc/objects/metadata.hxx"
#include "scrc/registry/data_object.hxx"
#include "scrc/utilities/hdf5.hxx"
#include "scrc/utilities/logging.hxx"

using namespace H5;

#define TABLE "table"
#define ARRAY "array"
#define COLUMN_UNITS "column_units"
#define ROW_NAMES "row_names"
#define DIMENSION_PREFIX "Dimension_"
#define DIM_UNITS_SUFFIX "_units"
#define DIM_NAME_SUFFIX "_names"
#define DIM_TITLE_SUFFIX "_title"

namespace SCRC {
YAML::Node parse_config_();
class LocalFileSystem {
private:
  const std::filesystem::path config_path_;
  const YAML::Node config_data_;
  YAML::Node meta_data_() const;

public:
  LocalFileSystem(std::filesystem::path config_file_path);
  std::filesystem::path get_data_store() const;
  std::vector<ReadObject::DataProduct *> read_data_products() const;
  std::vector<ReadObject::ExternalObject *> read_external_objects() const;
  std::string get_default_input_namespace() const;
  std::string get_default_output_namespace() const;
  YAML::Node get_config_data() const { return config_data_; }
};

std::string get_first_key_(const toml::value data_table);
Distribution *construct_dis_(const toml::value data_table);

double read_point_estimate_from_toml(const std::filesystem::path var_address);
Distribution *
read_distribution_from_toml(const std::filesystem::path var_address);

template <typename T>
std::filesystem::path write_array(const ArrayObject<T> *array,
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

  H5File *output_file_ = new H5File(output_path_, H5F_ACC_TRUNC);

  APILogger->debug("Writing Group '{0}' to file", component.string());

  Group *output_group_ =
      new Group(output_file_->createGroup(component.c_str()));

  const std::vector<int> dimensions_ = array->get_dimensions();

  hsize_t dim_[dimensions_.size()];

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

  APILogger->debug("FileSystem:WriteArray: Wrote array object.");

  // ---------- WRITE TITLES ------------- //

  for (int i{0}; i < dimensions_.size(); ++i) {
    const hsize_t str_dim_[1] = {1};
    const int rank = 1;
    DataSpace *str_space_ = new DataSpace(rank, str_dim_);
    const std::string title_ = array->get_title(i);
    const std::string label_ = "Dimension_" + std::to_string(i + 1) + "_title";

    char out_title_[title_.length() + 1];

    strcpy(out_title_, title_.c_str());

    StrType stype_(H5T_C_S1, title_.length() + 1);

    DataSet *dset_str_ = new DataSet(
        output_group_->createDataSet(label_.c_str(), stype_, *str_space_));
    dset_str_->write(out_title_, stype_);

    dset_str_->close();
    str_space_->close();
  }

  APILogger->debug("FileSystem:WriteArray: Wrote dimension titles.");

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

  APILogger->debug("FileSystem:WriteArray: Wrote dimension names.");

  output_file_->close();

  APILogger->debug("FileSystem:WriteArray: Wrote file '{0}'",
                   output_path_.string());

  return output_path_;
}

template <typename T>
ArrayObject<T> *read_array(const std::filesystem::path var_address,
                           const std::filesystem::path key) {
  APILogger->debug("FileSystem:ReadArray: Opening file '{0}'",
                   var_address.string());
  if (!std::filesystem::exists(var_address)) {
    throw std::runtime_error("Could not open HDF5 file '" +
                             var_address.string() + "', file does not exist.");
  }

  const H5File *file_ = new H5File(var_address.c_str(), H5F_ACC_RDONLY);

  // ------------- ARRAY RETRIEVAL -------------- //

  const std::string array_key_ = key.string() + "/" + std::string(ARRAY);
  APILogger->debug("FileSystem:ReadArray: Reading key '{0}'", array_key_);

  DataSet *data_set_ = new DataSet(file_->openDataSet(array_key_.c_str()));
  DataSpace *data_space_ = new DataSpace(data_set_->getSpace());

  const int arr_dims_ = data_space_->getSimpleExtentNdims();

  hsize_t dim_[arr_dims_];

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

  for (auto &d : dim_)
    dimensions_.push_back(d);

  APILogger->debug("FileSystem:ReadArray: Read Successful.");

  return new ArrayObject<T>{titles_, names_, dimensions_, data_out_};
}

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

  const H5File *file_ = new H5File(var_address.c_str(), H5F_ACC_RDONLY);

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
}; // namespace SCRC

#endif