/*! **************************************************************************
 * @file FairDataPipeline/registry/config.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File containing classes for interacting with the local file system
 *
 * The classes within this file contain methods for interacting with the
 * local file system. These include the reading of configurations, and
 * the reading/writing of data to locally.
 ****************************************************************************/
#ifndef __FDP_DATA_IO_HXX__
#define __FDP_DATA_IO_HXX__

#include "toml.hpp"
#include <cstdlib>
#include <ghc/filesystem.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <ghc/filesystem.hpp>

#include <iostream>

#include "fdp/objects/config.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/utilities/logging.hxx"
#include "fdp/utilities/semver.hxx"

namespace FairDataPipeline {

/*! ***************************************************************************
 * @brief read the value of a point estimate from a given TOML file
 * @author K. Zarebski (UKAEA)
 *
 * @param var_address file path for the input TOML file
 * @return the extracted point estimate value
 *
 * @paragraph testcases Test Case
 *    `test/test_filesystem.cxx`: TestTOMLPERead
 *
 *    This unit test checks that a point estimate can be read from a TOML file
 *    successfully
 *    @snippet `test/test_filesystem.cxx TestTOMLPERead
 *
 *****************************************************************************/
double read_point_estimate_from_toml(const ghc::filesystem::path var_address);

/**
 * @brief Create an estimate
 * 
 * @tparam T 
 * @param value 
 * @param data_product 
 * @param version_num 
 * @param config 
 * @return ghc::filesystem::path 
 */
template <typename T>
ghc::filesystem::path create_estimate(T &value,
                                      const ghc::filesystem::path &data_product,
                                      const Versioning::version &version_num,
                                      const Config *config
                                      ) {
  const std::string param_name_ = data_product.stem().string();
  const std::string namespace_ = config->get_default_output_namespace();
  const ghc::filesystem::path data_store_ = config->get_data_store();
  const toml::value data_{
      {param_name_, {{"type", "point-estimate"}, {"value", value}}}};

  const ghc::filesystem::path output_filename_ =
      data_store_ / namespace_ / data_product.parent_path() /
      std::string(version_num.to_string() + ".toml");
  std::ofstream toml_out_;

  if (!ghc::filesystem::exists(output_filename_.parent_path())) {
    ghc::filesystem::create_directories(output_filename_.parent_path());
  }

  toml_out_.open(output_filename_.string());

  if (!toml_out_) {
    throw std::runtime_error("Failed to open TOML file for writing");
  }

  toml_out_ << toml::format(data_);

  toml_out_.close();

 
  auto the_logger = logger::get_logger();
  the_logger->debug() 
      <<  "FileSystem:CreateEstimate: Wrote point estimate to '" << output_filename_.string() << "'";

  return output_filename_;
}

/**
 * @brief Get the first key of a given toml value
 * 
 * @param data_table 
 * @return std::string 
 */
std::string get_first_key_(const toml::value data_table);

}; // namespace FairDataPipeline

#endif
