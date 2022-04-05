#include "fdp/fdp.hxx"

#include "fdp/objects/config.hxx"
#include "fdp/utilities/logging.hxx"

namespace FairDataPipeline {
    /*! **************************************************************************
     * @class DataPipelineImpl_
     * @brief private pointer-to-implementation class containing all backend methods
     *
     * This class performs all behind the scenes operations acting as the backend to
     * the user called FairDataPipeline::DataPipeline class.
     * This structure has been chosen to allow for unit tests to be created to test
     * all methods including private ones.
     *
     * @warning The class should not be used directly during implementation,
     * but rather via an FairDataPipeline::DataPipeline instance.
     *
     *****************************************************************************/
    class DataPipeline::impl {

  private:
      Config::sptr config_;

      ghc::filesystem::path config_file_path_() const {return config_->get_config_file_path();}
      ghc::filesystem::path script_file_path_() const {return config_->get_script_file_path();}
      std::string token_() const {return config_->get_token();}
      RESTAPI api_location_() const {return config_->get_rest_api_location();}

      impl(const ghc::filesystem::path &config_file_path,
          const ghc::filesystem::path &file_system_path,
          const std::string& token,
          RESTAPI api_location = RESTAPI::LOCAL);

      impl(const impl &dp) = delete;
      impl& operator=(const impl& ) = delete;

  public:
      typedef std::shared_ptr< impl > sptr;
  
  /*! *************************************************************************
   * @brief construct a DataPipelineImpl_ instance from configurations and setup
   *
   * @param config_file_path location of the local configuration file
   * @param access_token_file API authorisation token where required
   * @param log_level level for the output logging statements
   * @param api_location whether to use local/remote RestAPI endpoint
   ***************************************************************************/
  static sptr construct(const ghc::filesystem::path &config_file_path,
          const ghc::filesystem::path &file_system_path,
          const std::string& token,
          RESTAPI api_location = RESTAPI::LOCAL);



  /**
   * @brief Default Destructor
   * 
   */

  ~impl() = default;
  
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

  /**
   * @brief Get the code run uuid
   * 
   * @return std::string 
   */
  std::string get_code_run_uuid() const;

};
DataPipeline::impl::sptr DataPipeline::impl::construct(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &script_file_path,
                    const std::string& token,
                    RESTAPI api_location)
{

    sptr pobj = impl::sptr( new impl( config_file_path,
                    script_file_path,
                    token,
                    api_location ) );
    return pobj;
}

DataPipeline::impl::impl(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &script_file_path,
                    const std::string& token,
                    RESTAPI api_location)
{
    this->config_  = Config::construct(config_file_path, script_file_path, token, api_location);

    const std::string api_root_ = config_->get_api_url();

    auto the_logger = logger::get_logger();

    the_logger->info() << "\n[Configuration]\n\t- Config Path:" << config_file_path.string() 
        << "\n\t- API Root: "
        << api_root_ 
        << "\n\t- FDP API Token: " << token;
}

ghc::filesystem::path FairDataPipeline::DataPipeline::impl::link_read(std::string &data_product){
    return config_->link_read(data_product);
}
ghc::filesystem::path FairDataPipeline::DataPipeline::impl::link_write(std::string &data_product){
    return config_->link_write(data_product);
}
void FairDataPipeline::DataPipeline::impl::finalise(){
    config_->finalise();
}

std::string FairDataPipeline::DataPipeline::impl::get_code_run_uuid() const { 
    return config_->get_code_run_uuid();
}

DataPipeline::~DataPipeline() = default;

DataPipeline::sptr DataPipeline::construct(
        const std::string &config_file_path,
        const std::string &script_file_path,
        std::string token )
{
   return DataPipeline::sptr( new DataPipeline(
    config_file_path,
    script_file_path,
    token ) );
}


DataPipeline::DataPipeline(
        const std::string &config_file_path,
        const std::string &script_file_path,
        std::string token )
: pimpl_( DataPipeline::impl::construct(ghc::filesystem::path(config_file_path), ghc::filesystem::path(script_file_path), token )) 
{
    logger::get_logger()->debug() << "DataPipeline: Initialising session '" 
        << pimpl_->get_code_run_uuid() << "'";
}

std::string FairDataPipeline::DataPipeline::link_read(std::string &data_product){
    return pimpl_->link_read(data_product);
}

std::string FairDataPipeline::DataPipeline::link_write(std::string &data_product){
    return pimpl_->link_write(data_product);
}

void FairDataPipeline::DataPipeline::finalise(){
    pimpl_->finalise();
}





}; // namespace FairDataPipeline
