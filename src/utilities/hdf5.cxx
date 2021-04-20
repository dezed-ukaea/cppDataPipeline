#include "scrc/utilities/hdf5.hxx"

const std::vector<std::string>
SCRC::HDF5::read_hdf5_as_str_vector(const std::filesystem::path file_name,
                                    const std::string key) {
  APILogger->debug("Utilities:HDF5:ReadAsStrVector: Opening file '{0}'",
                   file_name.string());

  if (!std::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5File *file_ = new H5File(file_name.c_str(), H5F_ACC_RDONLY);

  DataSet *data_set_ = new DataSet(file_->openDataSet(key.c_str()));
  DataSpace *data_space_ = new DataSpace(data_set_->getSpace());
  const DataType dtype_ = data_set_->getDataType();

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

  return data_str_;
}

const CompType *SCRC::HDF5::get_comptype(const std::filesystem::path file_name,
                                         std::string key) {
  APILogger->debug("Utilities:HDF5:GetCompType: Opening file '{0}'",
                   file_name.string());

  if (!std::filesystem::exists(file_name)) {
    throw std::runtime_error("Could not open HDF5 file '" + file_name.string() +
                             "', file does not exist.");
  }

  const H5File *file_ = new H5File(file_name.c_str(), H5F_ACC_RDONLY);

  DataSet *data_set_ = new DataSet(file_->openDataSet(key.c_str()));

  return new CompType(data_set_->getCompType());
}
