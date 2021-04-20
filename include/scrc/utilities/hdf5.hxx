#ifndef __SCRC_HDF5__
#define __SCRC_HDF5__

#include "H5Cpp.h"
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include "scrc/utilities/logging.hxx"

using namespace H5;

namespace SCRC {
namespace HDF5 {
template <typename T> const DataType *get_hdf5_type() {
  if (std::is_same<T, float>::value) {
    return &PredType::NATIVE_FLOAT;
  }

  else if (std::is_same<T, double>::value) {
    return &PredType::NATIVE_DOUBLE;
  }

  else if (std::is_same<T, long double>::value) {
    return &PredType::NATIVE_LDOUBLE;
  }

  else if (std::is_same<T, char>::value ||
           std::is_same<T, std::string>::value) {
    return &PredType::NATIVE_CHAR;
  }

  else if (std::is_same<T, unsigned char>::value) {
    return &PredType::NATIVE_UCHAR;
  }

  else if (std::is_same<T, short>::value) {
    return &PredType::NATIVE_SHORT;
  }

  else if (std::is_same<T, unsigned short>::value) {
    return &PredType::NATIVE_USHORT;
  }

  else if (std::is_same<T, int>::value) {
    return &PredType::NATIVE_INT;
  }

  else if (std::is_same<T, unsigned int>::value) {
    return &PredType::NATIVE_UINT;
  }

  else if (std::is_same<T, long>::value) {
    return &PredType::NATIVE_LONG;
  }

  else if (std::is_same<T, unsigned long>::value) {
    return &PredType::NATIVE_ULONG;
  }

  else if (std::is_same<T, long long>::value) {
    return &PredType::NATIVE_LLONG;
  }

  else if (std::is_same<T, unsigned long long>::value) {
    return &PredType::NATIVE_ULLONG;
  }

  throw std::runtime_error(
      "Unsupported C++ type, could not convert to HDF5 type");
}

const std::vector<std::string>
read_hdf5_as_str_vector(const std::filesystem::path file_name,
                        const std::string key);

template <typename T>
const T read_hdf5_object(const std::filesystem::path file_name,
                         std::string key) {
  APILogger->debug("Utilities:HDF5:ReadObject: Opening file '{0}'",
                   file_name.string());

  if (!std::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5File *file_ = new H5File(file_name.c_str(), H5F_ACC_RDONLY);
  DataSet *obj_set_ = new DataSet(file_->openDataSet(key.c_str()));
  DataSpace *obj_space_ = new DataSpace(obj_set_->getSpace());
  const DataType dtype_ = obj_set_->getDataType();

  if (dtype_.getClass() != H5T_STRING) {
    throw std::runtime_error("Type for object '" + key + "' is not string");
  }

  T obj_;

  obj_set_->read(obj_, dtype_);

  obj_set_->close();
  obj_space_->close();

  return obj_;
}

template <typename T>
const std::vector<T>
read_hdf5_comp_type_member(const std::filesystem::path file_name,
                           std::string key, std::string member) {
  APILogger->debug("Utilities:HDF5:ReadCompTypeMember: Opening file '{0}'",
                   file_name.string());

  if (!std::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5File *file_ = new H5File(file_name.c_str(), H5F_ACC_RDONLY);

  DataSet *data_set_ = new DataSet(file_->openDataSet(key.c_str()));
  DataSpace *data_space_ = new DataSpace(data_set_->getSpace());

  const int arr_dims_ = data_space_->getSimpleExtentNdims();
  const CompType ctype_ = data_set_->getCompType();
  const int col_index_ = ctype_.getMemberIndex(member);
  const int n_cols_ = ctype_.getNmembers();

  hsize_t dim_[arr_dims_];

  data_space_->getSimpleExtentDims(dim_, NULL);

  CompType ttype_(sizeof(T));

  const DataType dtype_ = *HDF5::get_hdf5_type<T>();

  ttype_.insertMember(member, 0, dtype_);
  APILogger->debug("Utilities:HDF5:ReadCompTypeMember: Extracting member '" +
                   member + "' from Compound Type");

  std::vector<T> container_(dim_[0]);

  data_set_->read(container_.data(), ttype_);

  data_space_->close();
  data_set_->close();

  return container_;
}

const CompType *get_comptype(const std::filesystem::path file_name,
                             std::string key);
}; // namespace HDF5
}; // namespace SCRC

#endif