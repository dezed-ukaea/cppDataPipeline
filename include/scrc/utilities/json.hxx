#ifndef __SCRC_JSON_HXX__
#define __SCRC_JSON_HXX__

#include <json/writer.h>

namespace SCRC
{
    std::string json_to_string(Json::Value& json_data);
};

#endif