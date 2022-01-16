/*! **************************************************************************
 * @file fdp/utilities/hdf5.hxx
 * @authors K. Zarebski (UKAEA), T. Middleweek (UKAEA)
 * @date 2021-05-07
 * @brief File contains methods relating to the reading/writing to HDF5
 *
 * This file contains methods which implement the HDF5 C++ API in order to
 * read and write arrays, and data tables as CompTypes. The procedures involved
 * require allocation of memory.
 ****************************************************************************/
#ifndef __FDP_HDF5__
#define __FDP_HDF5__

#include "H5Cpp.h"
#include <ghc/filesystem.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "fdp/utilities/logging.hxx"

namespace FDP {
namespace HDF5 {

/*! **************************************************************************
 * @struct CompTypeMember
 * @author T. Middleweek (UKAEA)
 *
 ****************************************************************************/
struct CompTypeMember {

  H5::PredType type;
  H5::StrType str_type; //(H5::PredType::C_S1, H5T_VARIABLE);

  int offset;
  CompTypeMember() : type(H5::PredType::NATIVE_UINT) {}

  H5::DataType *GetType() {
    if (type == H5::PredType::C_S1) {
      return &str_type;
    } else {
      return &type;
    }
  }
};

/*! **************************************************************************
 * @brief Get the hdf5 type from a given C++ type
 * @author K. Zarebski (UKAEA)
 *
 * @tparam T the C++ type
 * @return the HDF5 type for the given C++ type
 ****************************************************************************/
template <typename T> const H5::PredType *get_hdf5_type() {
  if (std::is_same<T, float>::value) {
    return &H5::PredType::NATIVE_FLOAT;
  }

  else if (std::is_same<T, double>::value) {
    return &H5::PredType::NATIVE_DOUBLE;
  }

  else if (std::is_same<T, long double>::value) {
    return &H5::PredType::NATIVE_LDOUBLE;
  }

  else if (std::is_same<T, char>::value) {
    return &H5::PredType::NATIVE_CHAR;
  }

  else if (std::is_same<T, char *>::value) {
    return &H5::PredType::C_S1;
  }

  else if (std::is_same<T, std::string>::value) {
    return &H5::PredType::C_S1;
  }

  else if (std::is_same<T, unsigned char>::value) {
    return &H5::PredType::NATIVE_UCHAR;
  }

  else if (std::is_same<T, short>::value) {
    return &H5::PredType::NATIVE_SHORT;
  }

  else if (std::is_same<T, unsigned short>::value) {
    return &H5::PredType::NATIVE_USHORT;
  }

  else if (std::is_same<T, int>::value) {
    return &H5::PredType::NATIVE_INT;
  }

  else if (std::is_same<T, unsigned int>::value) {
    return &H5::PredType::NATIVE_UINT;
  }

  else if (std::is_same<T, long>::value) {
    return &H5::PredType::NATIVE_LONG;
  }

  else if (std::is_same<T, unsigned long>::value) {
    return &H5::PredType::NATIVE_ULONG;
  }

  else if (std::is_same<T, long long>::value) {
    return &H5::PredType::NATIVE_LLONG;
  }

  else if (std::is_same<T, unsigned long long>::value) {
    return &H5::PredType::NATIVE_ULLONG;
  }

  throw std::runtime_error(
      "Unsupported C++ type, could not convert to HDF5 type");
}

/*! **************************************************************************
 * @struct CompType
 * @brief structure for holding multiple fields to be combined
 * as a CompType within the HDF5 file
 * @author T. Middleweek (UKAEA)
 *
 ****************************************************************************/
struct CompType {
  std::map<std::string, CompTypeMember> members;
  size_t size = 0;

  template <typename T> void AddMember(const std::string &member_name) {

    H5::PredType type = *HDF5::get_hdf5_type<T>();

    CompTypeMember member;
    member.offset = size;
    member.type = type;

    // In the case of a variable length string...(where type is char*)
    if (type == H5::PredType::C_S1) {
      member.str_type = H5::StrType(H5::PredType::C_S1, H5T_VARIABLE);
    }

    members[member_name] = member;
    size += sizeof(T);
  }

  CompTypeMember GetMember(const std::string &member_name) const {
    auto it = members.find(member_name);
    if (it == members.end()) {
      APILogger->error("Utilities:HDF5:CompType: Cannot add value to field "
                       "'{0}', field does not exist",
                       member_name);
      throw std::invalid_argument("Failed to add value to HDF5 container");
    }
    return it->second;
  }
};

/*! **************************************************************************
 * @author T. Middleweek (UKAEA)
 *
 * @note It is your responsiblilty to clean up complicated data types.
 * For instance, if the compound type has a member that is a pointer,
 * you will need to clear this up before cleaning this array.
 * That is why there is an explict step to clean up this array to remind
 * you to do that!
 ****************************************************************************/
struct CompValueArray {
  CompType type;
  char *buffer = nullptr;
  size_t size;

  CompValueArray(const CompType &type_in, size_t size_in)
      : type(type_in), size(size_in) {
    buffer = new char[type.size * size];
  }

  ~CompValueArray() {
    // This will have a go at you if you have not cleaned up this array.
    assert(buffer == nullptr);
  }

  template <typename T>
  void SetValue(size_t index, const std::string &member_name, T value) {

    CompTypeMember member = type.GetMember(member_name);
    // Type check
    H5::PredType pred_type = *HDF5::get_hdf5_type<T>();

    if (member.type != pred_type) {
      APILogger->error("Utilities:HDF5:CompTypeArray: Type mismatch between "
                       "field and input value "
                       "when adding value '{0}', to field '{1}': {2} != {3}",
                       value, member_name, member.type.getId(),
                       pred_type.getId());
      throw std::invalid_argument("Cannot insert value into HDF5 container");
    }

    size_t offset = (type.size * index) + member.offset;

    memcpy(&buffer[offset], &value, sizeof(T));
  }

  template <typename T>
  T GetValue(size_t index, const std::string &member_name) {

    CompTypeMember member = type.GetMember(member_name);
    H5::PredType pred_type = *HDF5::get_hdf5_type<T>();
    // Do a type check
    if (member.type != pred_type) {
      APILogger->error("Utilities:HDF5:CompTypeArray: Type mismatch between "
                       "field and input value "
                       "for field '{0}': {1} != {2}",
                       member_name, member.type.getId(), pred_type.getId());
      throw std::invalid_argument("Cannot insert value into HDF5 container");
    }

    // Calculate the offset in teh buffer
    size_t offset = (type.size * index) + member.offset;

    T *value_ptr = (T *)&buffer[offset];

    return *value_ptr;
  }

  char *Data() { return buffer; }

  void Cleanup() {
    delete[] buffer;
    buffer = nullptr;
  }
};

const std::vector<std::string>
read_hdf5_as_str_vector(const ghc::filesystem::path file_name,
                        const std::string key);

template <typename T>
const T read_hdf5_object(const ghc::filesystem::path file_name,
                         std::string key) {
  APILogger->debug("Utilities:HDF5:ReadObject: Opening file '{0}'",
                   file_name.string());

  if (!ghc::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5::H5File *file_ = new H5::H5File(file_name.string().c_str(), H5F_ACC_RDONLY);
  H5::DataSet *obj_set_ = new H5::DataSet(file_->openDataSet(key.c_str()));
  H5::DataSpace *obj_space_ = new H5::DataSpace(obj_set_->getSpace());
  const H5::DataType dtype_ = obj_set_->getDataType();

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
read_hdf5_comp_type_member(const ghc::filesystem::path file_name,
                           std::string key, std::string member) {
  APILogger->debug("Utilities:HDF5:ReadCompTypeMember: Opening file '{0}'",
                   file_name.string());

  if (!ghc::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5::H5File *file_ = new H5::H5File(file_name.string().c_str(), H5F_ACC_RDONLY);

  H5::DataSet *data_set_ = new H5::DataSet(file_->openDataSet(key.c_str()));
  H5::DataSpace *data_space_ = new H5::DataSpace(data_set_->getSpace());

  int arr_dims_ = data_space_->getSimpleExtentNdims();
  const H5::CompType ctype_ = data_set_->getCompType();
  hsize_t *dim_ = new hsize_t[arr_dims_];

  data_space_->getSimpleExtentDims(dim_, NULL);

  H5::CompType ttype_(sizeof(T));

  const H5::DataType dtype_ = *HDF5::get_hdf5_type<T>();

  ttype_.insertMember(member, 0, dtype_);

  APILogger->debug("Utilities:HDF5:ReadCompTypeMember: Extracting member '" +
                   member + "' from Compound Type");

  std::vector<T> container_(dim_[0]);

  data_set_->read(container_.data(), ttype_);

  data_space_->close();
  data_set_->close();

  return container_;
}

const H5::CompType *get_comptype(const ghc::filesystem::path file_name,
                                 std::string key);
}; // namespace HDF5
}; // namespace FDP

#endif