/*! **************************************************************************
 * @file fdp/registry/datapipeline.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File containing classes for interaction with the FDP data pipeline
 *
 * The classes within this file form the core objects for handling interaction
 * with the FDP FAIR Data pipeline. They contain methods which parse
 * configurations and control calls to the RestAPI itself.
 ****************************************************************************/
#ifndef __FDP_DATAPIPELINE_HXX__
#define __FDP_DATAPIPELINE_HXX__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "json/json.h"

#include "fdp/objects/config.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/registry/api.hxx"
#include "fdp/registry/data_io.hxx"
#include "fdp/utilities/logging.hxx"

namespace FDP {

/*! **************************************************************************
 * @class DataPipelineImpl_
 * @brief private pointer-to-implementation class containing all backend methods
 *
 * This class performs all behind the scenes operations acting as the backend to
 * the user called FDP::DataPipeline class.
 * This structure has been chosen to allow for unit tests to be created to test
 * all methods including private ones.
 *
 * @warning The class should not be used directly during implementation,
 * but rather via an FDP::DataPipeline instance.
 *
 *****************************************************************************/
class DataPipelineImpl_ {
  private:
    std::unique_ptr<Config> config_;

    ghc::filesystem::path config_file_path_() const {return config_->get_config_file_path();}
    ghc::filesystem::path script_file_path_() const {return config_->get_script_file_path();}
    std::string token_() const {return config_->get_token();}
    RESTAPI api_location_() {return config_->get_rest_api_location();}
public:
  
  /*! *************************************************************************
   * @brief construct a DataPipelineImpl_ instance from configurations and setup
   *
   * @param config_file_path location of the local configuration file
   * @param access_token_file API authorisation token where required
   * @param log_level level for the output logging statements
   * @param api_location whether to use local/remote RestAPI endpoint
   ***************************************************************************/
  DataPipelineImpl_(const ghc::filesystem::path &config_file_path,
                    const ghc::filesystem::path &file_system_path,
                    const std::string token,
                    spdlog::level::level_enum log_level = spdlog::level::info,
                    RESTAPI api_location = RESTAPI::LOCAL);

  DataPipelineImpl_(const DataPipelineImpl_ &dp){
    DataPipelineImpl_(dp.config_file_path_(), dp.script_file_path_(), token_(), spdlog::level::info, api_location_());}

  /**
   * @brief Default Destructor
   * 
   */
  ~DataPipelineImpl_() = default;

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
  std::string get_code_run_uuid();

};
}; // namespace FDP

#endif