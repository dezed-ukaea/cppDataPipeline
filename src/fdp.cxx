#include "fdp/fdp.hxx"

namespace FDP {

DataPipeline::~DataPipeline() = default;
DataPipeline::DataPipeline(DataPipeline &&) noexcept = default;
DataPipeline& DataPipeline::operator=(DataPipeline &&) noexcept = default;

DataPipeline::DataPipeline(const DataPipeline& rhs)
    : pimpl_(std::make_shared<DataPipelineImpl_>(*rhs.pimpl_))
{}

DataPipeline& DataPipeline::operator=(const DataPipeline& rhs) {
    if (this != &rhs) 
        pimpl_.reset(new DataPipelineImpl_(*rhs.pimpl_));

    return *this;
}

std::filesystem::path FDP::DataPipeline::link_read(std::string &data_product){
    return Pimpl()->link_read(data_product);
  }
  std::filesystem::path FDP::DataPipeline::link_write(std::string &data_product){
    return Pimpl()->link_write(data_product);
  }
  void FDP::DataPipeline::finalise(){
    Pimpl()->finalise();
  }

}; // namespace FDP