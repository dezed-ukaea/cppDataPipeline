#include "scrc/registry/datapipeline.hxx"

namespace SCRC
{
    Json::Value DataPipeline::fetch_all_objects()
    {
        return api_->request("object/");
    }

    Json::Value DataPipeline::fetch_object_by_id(int identifier)
    {
        return api_->request("object/"+std::to_string(identifier)+"/");
    }
};