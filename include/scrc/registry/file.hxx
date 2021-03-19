#ifndef __SCRC_FILE_HXX__
#define __SCRC_FILE_HXX__

#include <string>
#include <filesystem>
#include <fstream>
#include "digestpp.hpp"


namespace SCRC
{
    std::string calculate_hash(std::filesystem::path);
}

#endif