#include "fdp/fdp.hxx"

namespace FDP {

    class DataPipeline::impl {

  private:
      Config::uptr config_;

      ghc::filesystem::path config_file_path_() const {return config_->get_config_file_path();}
      ghc::filesystem::path script_file_path_() const {return config_->get_script_file_path();}
      std::string token_() const {return config_->get_token();}
      RESTAPI api_location_() {return config_->get_rest_api_location();}
  public:
  
  /*! *************************************************************************
   * @brief construct a DataPipelineImpl_ instance from configurations and setup
   * @author K. Zarebski (UKAEA)
   *
   * @param config_file_path location of the local configuration file
   * @param access_token_file API authorisation token where required
   * @param log_level level for the output logging statements
   * @param api_location whether to use local/remote RestAPI endpoint
   ***************************************************************************/
  impl(const ghc::filesystem::path &config_file_path,
          const ghc::filesystem::path &file_system_path,
          const std::string token,
          spdlog::level::level_enum log_level = spdlog::level::info,
          RESTAPI api_location = RESTAPI::LOCAL);

  impl(const impl &dp);

  /**
   * @brief Destroy the Data Pipeline Impl_ object
   * 
   */
  ~impl() = default;
  
  //DataPipelineImpl_& operator=(DataPipelineImpl_ dp);



  ghc::filesystem::path link_read(std::string &data_product);
  ghc::filesystem::path link_write(std::string &data_product);
  void finalise();

  std::string get_code_run_uuid();

};

DataPipeline::impl::impl(const impl &dp)
{
    impl( dp.config_file_path_()
            , dp.script_file_path_()
            , token_(), spdlog::level::info
            , api_location_()
        );
}


DataPipeline::impl::impl(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &script_file_path,
                    const std::string token,
                    spdlog::level::level_enum log_level,
                    RESTAPI api_location)
{
    Config::uptr configPtr(new Config(config_file_path, script_file_path, token, api_location));
    config_ = std::move(configPtr);

    const std::string api_root_ = config_->get_api_url();

    spdlog::set_default_logger(APILogger);
    APILogger->set_level(log_level);

    APILogger->info("\n[Configuration]\n\t- Config Path: {0}\n\t- API Root: "
            "{1}\n\t- FDP API Token: {2}",
            config_file_path.string(), api_root_,
            token);
}

ghc::filesystem::path FDP::DataPipeline::impl::link_read(std::string &data_product){
    return config_->link_read(data_product);
}
ghc::filesystem::path FDP::DataPipeline::impl::link_write(std::string &data_product){
    return config_->link_write(data_product);
}
void FDP::DataPipeline::impl::finalise(){
    config_->finalise();
}

std::string FDP::DataPipeline::impl::get_code_run_uuid(){
    return config_->get_code_run_uuid();
}





DataPipeline::~DataPipeline() = default;
DataPipeline::DataPipeline(DataPipeline &&) noexcept = default;
DataPipeline& DataPipeline::operator=(DataPipeline &&) noexcept = default;

DataPipeline::DataPipeline(const DataPipeline& rhs)
    : pimpl_(std::make_shared< DataPipeline::impl >(*rhs.pimpl_))
{}

DataPipeline::DataPipeline(
        const std::string &config_file_path,
        const std::string &script_file_path,
        std::string token,
        spdlog::level::level_enum log_level )
: pimpl_(std::make_shared< DataPipeline::impl >(ghc::filesystem::path(config_file_path), ghc::filesystem::path(script_file_path), token,
            log_level)) 
{
    APILogger->debug("DataPipeline: Initialising session '{0}'", pimpl_->get_code_run_uuid());
}


DataPipeline& DataPipeline::operator=(const DataPipeline& rhs) {
    if (this != &rhs) 
        pimpl_.reset(new impl(*rhs.pimpl_));

    return *this;
}

ghc::filesystem::path FDP::DataPipeline::link_read(std::string &data_product){
    return pimpl_->link_read(data_product);
}

ghc::filesystem::path FDP::DataPipeline::link_write(std::string &data_product){
    return pimpl_->link_write(data_product);
}

void FDP::DataPipeline::finalise(){
    pimpl_->finalise();
}





}; // namespace FDP
