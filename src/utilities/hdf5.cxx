#include "fdp/utilities/hdf5.hxx"

const std::vector<std::string>
FDP::HDF5::read_hdf5_as_str_vector(const std::filesystem::path file_name,
                                   const std::string key) {
  APILogger->debug("Utilities:HDF5:ReadAsStrVector: Opening file '{0}'",
                   file_name.string());

  if (!std::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5::H5File *file_ = new H5::H5File(file_name.c_str(), H5F_ACC_RDONLY);

  H5::DataSet *data_set_ = new H5::DataSet(file_->openDataSet(key.c_str()));
  H5::DataSpace *data_space_ = new H5::DataSpace(data_set_->getSpace());
  const H5::DataType dtype_ = data_set_->getDataType();

  const int arr_dims_ = data_space_->getSimpleExtentNdims();

  hsize_t dim_[arr_dims_];

  data_space_->getSimpleExtentDims(dim_, NULL);

  std::vector<const char *> data_(dim_[0]);
  std::vector<std::string> data_str_;

  data_set_->read(data_.data(), dtype_, *data_space_, *data_space_);

  for (auto &c : data_)
    data_str_.push_back(std::string(c));

  data_space_->close();
  data_set_->close();

  delete data_space_;
  delete data_set_;
  delete file_;

  return data_str_;
}

const H5::CompType *
FDP::HDF5::get_comptype(const std::filesystem::path file_name,
                        std::string key) {
  APILogger->debug("Utilities:HDF5:GetCompType: Opening file '{0}'",
                   file_name.string());

  if (!std::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5::H5File *file_ = new H5::H5File(file_name.c_str(), H5F_ACC_RDONLY);

  H5::DataSet *data_set_ = new H5::DataSet(file_->openDataSet(key.c_str()));

  H5::CompType *ctype_ = new H5::CompType(data_set_->getCompType());

  data_set_->close();

  delete data_set_;
  delete file_;

  return ctype_;
}
