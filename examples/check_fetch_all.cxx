#include "data_pipeline.hxx"
#include "jsoncpp/json/json.h"

#include <iostream>

using namespace SCRC;

int main()
{
    DataPipeline* pipeline = new DataPipeline;
    Json::Value json_all = pipeline->fetch_all_objects();
    std::cout << json_all << std::endl;
    return 0;
}