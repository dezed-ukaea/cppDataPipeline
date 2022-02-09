#include "fdp/registry/datapipeline.hxx"

namespace FDP {
class DataPipeline {
public:
  /**
   * @brief Construct a new Data Pipeline (PIMPL)
   * 
   * @param config_file_path 
   * @param script_file_path 
   * @param token 
   * @param log_level 
   */
  explicit DataPipeline(
      const std::string &config_file_path,
      const std::string &script_file_path,
      std::string token = "",
      spdlog::level::level_enum log_level = spdlog::level::info)
      : pimpl_(std::make_shared<DataPipelineImpl_>(ghc::filesystem::path(config_file_path), ghc::filesystem::path(script_file_path), token,
                                     log_level)) {
          
    APILogger->debug("DataPipeline: Initialising session '{0}'", Pimpl()->get_code_run_uuid());
  }

  /**
   * @brief Destroy the Data Pipeline object
   * 
   */
  ~DataPipeline();

  DataPipeline(DataPipeline &&rhs) noexcept;
  DataPipeline &operator=(DataPipeline &&rhs) noexcept;

  DataPipeline(const DataPipeline &rhs);
  DataPipeline &operator=(const DataPipeline &rhs);

  /**
   * @brief Return a path to a given data product
   * Whilst recording it's meta data for the code run
   * 
   * @param data_product 
   * @return ghc::filesystem::path 
   */
  ghc::filesystem::path link_read(std::string &data_product);

  /**
   * @brief Return a path to be used for a given data product
   * whilst recording it's meta data
   * 
   * @param data_product 
   * @return ghc::filesystem::path 
   */
  ghc::filesystem::path link_write(std::string &data_product);

  /**
   * @brief Finalise the pipeline
   * Record all data products and meta data to the registry
   * update the code run with all appropriate meta data
   * 
   */
  void finalise();

private:
  const DataPipelineImpl_* Pimpl() const {return pimpl_.get();}
  DataPipelineImpl_* Pimpl() {return pimpl_.get();}
  
  std::shared_ptr<DataPipelineImpl_> pimpl_;
};
}; // namespace FDP