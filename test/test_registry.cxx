#include "gtest/gtest.h"
#include <filesystem>
#include <ostream>
#include <vector>
#include "scrc/registry/datapipeline.hxx"
#include "scrc/registry/file_system.hxx"
#include "scrc/registry/file.hxx"

using namespace SCRC;


DataPipeline* init_pipeline()
{
    const std::filesystem::path config_path_ = std::filesystem::path(TESTDIR) / "config.yaml";
    APILogger->set_level(spdlog::level::debug);
    return new DataPipeline(config_path_, REMOTE_API_ROOT, spdlog::level::debug);
}

TEST(SCRCAPITest, TestLogLevelSet)
{
    init_pipeline();
    ASSERT_EQ(spdlog::get_level(), spdlog::level::debug);
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
    ASSERT_TRUE(std::filesystem::exists(std::filesystem::path(TESTDIR) / "datastore"));
}

TEST(SCRCAPITest, TestGetConfigDataProducts)
{
    LocalFileSystem* fs_ = new LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
    const std::vector<ReadObject::DataProduct*> data_products_ = fs_->read_data_products();
    const semver::version version_ = data_products_[0]->get_version();
    const std::filesystem::path path_ = data_products_[0]->get_path();
    ASSERT_EQ(version_, semver::version({0, 1, 0}));
    ASSERT_EQ(path_, "fixed-parameters/T_lat");
}

TEST(SCRCAPITest, TestHashFile)
{
    const std::string file_hash_ = calculate_hash(std::filesystem::path(TESTDIR) / "config.yaml");
    std::cout << "HASH: " << file_hash_ << std::endl;
}
