/*! **************************************************************************
 * @file fdp/objects/metadata.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-05
 * @brief File containing methods for handling of metadata
 *
 * The functions contained within this file process or create metadata
 * related to the versioning of objects and runs.
 ****************************************************************************/
#ifndef __FDP_METADATA_HXX__
#define __FDP_METADATA_HXX__

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>

#include "digestpp.hpp"

#ifdef _WIN32
   #include <io.h> 
   #define access    _access_s
#else
   #include <unistd.h>
#endif

namespace FDP {
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

/**
 * @brief Generate a random hash
 * 
 * @return std::string a random hash
 */
std::string generate_random_hash();

/*! **************************************************************************
 * @brief generates a run identifier from configurations given in an API
 * config file
 * @author K. Zarebski (UKAEA)
 *
 * @param config_file_path path to the FDP API configuration file
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

bool FileExists( const std::string &Filename );

}; // namespace FDP

#endif