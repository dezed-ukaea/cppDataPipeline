#include "gtest/gtest.h"
#include "scrc/registry/datapipeline.hxx"
#include "scrc/registry/file_system.hxx"

using namespace SCRC;

TEST(SCRCAPITest, TestAccessObjectRegistry)
{
    DataPipeline* api = new DataPipeline;
    ASSERT_NO_THROW(api->fetch_all_objects());
}

TEST(SCRCAPITest, TestAccessObject)
{
    DataPipeline* api = new DataPipeline;
    ASSERT_NO_THROW(api->fetch_object_by_id(72974));
}

TEST(SCRCAPITest, TestAccessDataRegistry)
{
    DataPipeline* api = new DataPipeline;
    ASSERT_NO_THROW(api->fetch_all_data_products());
}

TEST(SCRCAPITest, TestAccessData)
{
    DataPipeline* api = new DataPipeline;
    ASSERT_NO_THROW(api->fetch_object_by_id(3337));
}

TEST(SCRCAPITest, TestFileSystemSetup)
{
    LocalFileSystem* filesystem = new LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
    ASSERT_TRUE(std::filesystem::exists(std::filesystem::path(TESTDIR) / "data"));
}