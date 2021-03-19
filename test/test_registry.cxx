#include "gtest/gtest.h"
#include <filesystem>
#include "scrc/registry/datapipeline.hxx"
#include "scrc/registry/file_system.hxx"

using namespace SCRC;

DataPipeline* init_pipeline()
{
    const std::filesystem::path config_path_ = std::filesystem::path(TESTDIR) / "config.yaml";
    return new DataPipeline(config_path_);
}

TEST(SCRCAPITest, TestAccessObjectRegistry)
{ 
    ASSERT_NO_THROW(init_pipeline()->fetch_all_objects());
}

TEST(SCRCAPITest, TestAccessObject)
{
    ASSERT_NO_THROW(init_pipeline()->fetch_object_by_id(72974));
}

TEST(SCRCAPITest, TestAccessDataRegistry)
{
    ASSERT_NO_THROW(init_pipeline()->fetch_all_data_products());
}

TEST(SCRCAPITest, TestAccessData)
{
    ASSERT_NO_THROW(init_pipeline()->fetch_object_by_id(3337));
}

TEST(SCRCAPITest, TestFileSystemSetup)
{
    LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
    ASSERT_TRUE(std::filesystem::exists(std::filesystem::path(TESTDIR) / "data"));
}
