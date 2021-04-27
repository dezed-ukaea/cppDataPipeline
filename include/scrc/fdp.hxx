#include "scrc/registry/datapipeline.hxx"

namespace SCRC
{
class DataPipeline {
public:
  // 'initialise' method for the API
  explicit DataPipeline(
      spdlog::level::level_enum log_level = spdlog::level::info,
      std::filesystem::path apiroot = LOCAL_API_ROOT,
      std::filesystem::path config_file_path = std::filesystem::path(std::getenv("FDP_CONFIG_DIR")) / "config.yaml")
      : pimpl_(new DataPipelineImpl_(config_file_path, apiroot, log_level)),
        session_id_(generate_run_id(config_file_path)) {
    if(!std::filesystem::exists(config_file_path))
    {
        APILogger->error("DataPipeline: Failed to retrieve configuration file location");
        throw config_parsing_error("No configuration directory was identified");
    }
    APILogger->debug("DataPipeline: Initialising session '{0}'", session_id_);
  }

  // 'finalise' method for the API
  ~DataPipeline();

  void add_to_register(std::string &item);
  double read_point_estimate(const std::filesystem::path &data_product);

  DataPipeline(DataPipeline &&rhs) noexcept;
  DataPipeline &operator=(DataPipeline &&rhs) noexcept;

  DataPipeline(const DataPipeline &rhs);
  DataPipeline &operator=(const DataPipeline &rhs);

private:
  std::unique_ptr<DataPipelineImpl_> pimpl_;
  const std::string session_id_;
};
};