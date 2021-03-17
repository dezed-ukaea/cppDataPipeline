#include "gtest/gtest.h"
#include "scrc/registry/datapipeline.hxx"

using namespace SCRC;

TEST(SCRCAPITest, TestAccessRegistry)
{
    DataPipeline* api = new DataPipeline;
    ASSERT_NO_THROW(api->fetch_all_objects());
}

TEST(SCRCAPITest, TestAccessObject)
{
    DataPipeline* api = new DataPipeline;
    ASSERT_NO_THROW(api->fetch_object_by_id(72974));
}