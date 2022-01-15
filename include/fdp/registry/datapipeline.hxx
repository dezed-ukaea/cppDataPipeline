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
#include "fdp/registry/data_object.hxx"
#include "fdp/registry/data_io.hxx"
#include "fdp/utilities/logging.hxx"

namespace FDP {

/*! **************************************************************************
 * @class DataPipelineImpl_
 * @brief private pointer-to-implementation class containing all backend methods
 * @author K. Zarebski (UKAEA)
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

    std::filesystem::path config_file_path_() const {return config_->get_config_file_path();}
    std::filesystem::path script_file_path_() const {return config_->get_script_file_path();}
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
  DataPipelineImpl_(const std::filesystem::path &config_file_path,
                    const std::filesystem::path &file_system_path,
                    const std::string token,
                    spdlog::level::level_enum log_level = spdlog::level::info,
                    RESTAPI api_location = RESTAPI::LOCAL);

  DataPipelineImpl_(const DataPipelineImpl_ &dp){
    DataPipelineImpl_(dp.config_file_path_(), dp.script_file_path_(), token_(), spdlog::level::info, api_location_());}

  /**
   * @brief Destroy the Data Pipeline Impl_ object
   * 
   */
  ~DataPipelineImpl_() = default;
  
  //DataPipelineImpl_& operator=(DataPipelineImpl_ dp);



  std::filesystem::path link_read(std::string &data_product);
  std::filesystem::path link_write(std::string &data_product);
  void finalise();

  std::string get_code_run_uuid();

};
}; // namespace FDP

#endif