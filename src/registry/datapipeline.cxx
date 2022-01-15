#include "fdp/registry/datapipeline.hxx"

namespace FDP {

 DataPipelineImpl_::DataPipelineImpl_(const std::filesystem::path &config_file_path,
                    const std::filesystem::path &script_file_path,
                    const std::string token,
                    spdlog::level::level_enum log_level,
                    RESTAPI api_location)
       {
         std::unique_ptr<Config> configPtr(new Config(config_file_path, script_file_path, token, api_location));
         config_ = std::move(configPtr);

            const std::string api_root_ = config_->get_api_url();

            spdlog::set_default_logger(APILogger);
            APILogger->set_level(log_level);

            APILogger->info("\n[Configuration]\n\t- Config Path: {0}\n\t- API Root: "
                            "{1}\n\t- FDP API Token: {2}",
                            config_file_path.string(), api_root_,
                            token);
    }

  std::filesystem::path FDP::DataPipelineImpl_::link_read(std::string &data_product){
    return config_->link_read(data_product);
  }
  std::filesystem::path FDP::DataPipelineImpl_::link_write(std::string &data_product){
    return config_->link_write(data_product);
  }
  void FDP::DataPipelineImpl_::finalise(){
    config_->finalise();
  }

  std::string FDP::DataPipelineImpl_::get_code_run_uuid(){
    return config_->get_code_run_uuid();
  }



}; // namespace FDP