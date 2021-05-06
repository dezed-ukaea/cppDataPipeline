/*! **************************************************************************
 * @file scrc/objects/metadata.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-05
 * @brief File containing methods for handling of metadata
 *
 * The functions contained within this file process or create metadata
 * related to the versioning of objects and runs.
 ****************************************************************************/
#ifndef __SCRC_METADATA_HXX__
#define __SCRC_METADATA_HXX__

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "digestpp.hpp"

namespace SCRC {
/*! **************************************************************************
 * @brief calculates a hash from a given input file via SHA1
 * @author K. Zarebski (UKAEA)
 *
 * @return the hash obtained from the file contents
 ****************************************************************************/
std::string calculate_hash_from_file(const std::filesystem::path &);

/*! **************************************************************************
 * @brief calculates a hash from a given string via SHA1
 * @author K. Zarebski (UKAEA)
 *
 * @param input string to be hashed
 * @return the hash obtained from the string contents
 ****************************************************************************/
std::string calculate_hash_from_string(const std::string &input);

/*! **************************************************************************
 * @brief generates a run identifier from configurations given in an API
 * config file
 * @author K. Zarebski (UKAEA)
 *
 * @param config_file_path path to the SCRC API configuration file
 * @return identifier generated for the code run
 ***************************************************************************/
std::string generate_run_id(std::filesystem::path config_file_path);

/*! *************************************************************************
 * @brief returns the current time as a timestamp
 * @author K. Zarebski (UKAEA)
 *
 * @param file_name if true, format the timestamp to be filename friendly
 * @return timestamp for the current time
 ***************************************************************************/
std::string current_time_stamp(bool file_name = false);

}; // namespace SCRC

#endif