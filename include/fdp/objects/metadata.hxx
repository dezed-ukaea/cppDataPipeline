/*! **************************************************************************
 * @file FairDataPipeline/objects/metadata.hxx
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
#include <ghc/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <regex>

#include "digestpp.hpp"

#ifdef _WIN32
   #include <io.h> 
   #define access    _access_s
#else
   #include <unistd.h>
#endif

namespace FairDataPipeline {
/*! **************************************************************************
 * @brief calculates a hash from a given input file via SHA1
 *
 * @return the hash obtained from the file contents
 ****************************************************************************/
std::string calculate_hash_from_file(const ghc::filesystem::path &);

/*! **************************************************************************
 * @brief calculates a hash from a given string via SHA1
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

/*! *************************************************************************
 * @brief returns the current time as a timestamp
 * @author K. Zarebski (UKAEA)
 *
 * @param file_name if true, format the timestamp to be filename friendly
 * @return timestamp for the current time
 ***************************************************************************/
std::string current_time_stamp(bool file_name = false);

/**
 * @brief remove the file:// from a given root
 * 
 * @param root 
 * @return std::string 
 */
std::string remove_local_from_root(const std::string &root);

/**
 * @brief replace backslashes with forward slashes in a given path
 * 
 * @param path 
 * @return std::string 
 */
std::string remove_backslash_from_path(const std::string &path);

/**
 * @brief check whether a given file exists
 * 
 * @param Filename 
 * @return true file exists
 * @return false file does not exist
 */
bool file_exists( const std::string &Filename );

/**
 * @brief read a token from a given file
 * 
 * @param token_path 
 * @return std::string 
 */
std::string read_token(const ghc::filesystem::path &token_path);

}; // namespace FairDataPipeline

#endif