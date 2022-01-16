#include "fdp/registry/datapipeline.hxx"

namespace FDP {
class DataPipeline {
public:
  // 'initialise' method for the API
  explicit DataPipeline(
      const std::string &config_file_path,
      const std::string &script_file_path,
      std::string token = "",
      spdlog::level::level_enum log_level = spdlog::level::info)
      : pimpl_(std::make_shared<DataPipelineImpl_>(ghc::filesystem::path(config_file_path), ghc::filesystem::path(script_file_path), token,
                                     log_level)) {
          
    APILogger->debug("DataPipeline: Initialising session '{0}'", Pimpl()->get_code_run_uuid());
  }

  // 'finalise' method for the API
  ~DataPipeline();

  DataPipeline(DataPipeline &&rhs) noexcept;
  DataPipeline &operator=(DataPipeline &&rhs) noexcept;

  DataPipeline(const DataPipeline &rhs);
  DataPipeline &operator=(const DataPipeline &rhs);

  ghc::filesystem::path link_read(std::string &data_product);
  ghc::filesystem::path link_write(std::string &data_product);
  void finalise();

private:
  const DataPipelineImpl_* Pimpl() const {return pimpl_.get();}
  DataPipelineImpl_* Pimpl() {return pimpl_.get();}
  
  std::shared_ptr<DataPipelineImpl_> pimpl_;
};
}; // namespace FDP