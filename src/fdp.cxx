#include "scrc/fdp.hxx"

namespace SCRC {
DataPipeline::~DataPipeline() {}

void DataPipeline::add_to_register(std::string &item) {}

double
DataPipeline::read_point_estimate(const std::filesystem::path &data_product) {
  return read_point_estimate_from_toml(data_product);
}

}; // namespace SCRC