#include "scrc/objects/metadata.hxx"
#include "scrc/registry/datapipeline.hxx"
#include "scrc/registry/file_system.hxx"
#include "gtest/gtest.h"
#include <filesystem>
#include <ostream>
#include <vector>

using namespace SCRC;

DataPipelineImpl_ *init_pipeline(bool use_local = false) {
  if (std::filesystem::exists(std::filesystem::path(TESTDIR) /
                              std::filesystem::path("datastore"))) {
    std::filesystem::remove_all(std::filesystem::path(TESTDIR) /
                                std::filesystem::path("datastore"));
  }

  const std::filesystem::path config_path_ =
      std::filesystem::path(TESTDIR) / "config.yaml";
  APILogger->set_level(spdlog::level::debug);

  return new DataPipelineImpl_(config_path_,
                               (use_local) ? LOCAL_API_ROOT : REMOTE_API_ROOT,
                               spdlog::level::debug);
}

TEST(SCRCAPITest, TestDataPipelineInit) {
  if (std::filesystem::exists(std::filesystem::path(TESTDIR) /
                              std::filesystem::path("datastore"))) {
    std::filesystem::remove_all(std::filesystem::path(TESTDIR) /
                                std::filesystem::path("datastore"));
  }

  const std::filesystem::path config_path_ =
      std::filesystem::path(TESTDIR) / "config.yaml";
  APILogger->set_level(spdlog::level::debug);
  DataPipeline(config_path_, REMOTE_API_ROOT, spdlog::level::debug);
}

TEST(SCRCAPITest, TestLogLevelSet) {
  init_pipeline();
  ASSERT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST(SCRCAPITest, TestURLEncode) {
  const std::string start_string_ = "fixed-parameters/T_lat";
  const std::string expected_ = "fixed-parameters%2FT_lat";

  ASSERT_EQ(url_encode(start_string_), expected_);
}

TEST(SCRCAPITest, TestAccessObjectRegistry) {
  ASSERT_NO_THROW(init_pipeline()->fetch_all_objects());
}

TEST(SCRCAPITest, TestAccessObject) {
  ASSERT_NO_THROW(init_pipeline()->fetch_object_by_id(72974));
}

TEST(SCRCAPITest, TestAccessDataRegistry) {
  ASSERT_NO_THROW(init_pipeline()->fetch_all_data_products());
}

TEST(SCRCAPITest, TestAccessData) {
  ASSERT_NO_THROW(init_pipeline()->fetch_object_by_id(3337));
}

TEST(SCRCAPITest, TestFileSystemSetup) {
  LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
  ASSERT_TRUE(
      std::filesystem::exists(std::filesystem::path(TESTDIR) / "datastore"));
}

TEST(SCRCAPITest, TestGetConfigDataTOML) {
  LocalFileSystem *fs_ =
      new LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
  const std::vector<ReadObject::DataProduct *> data_products_ =
      fs_->read_data_products();
  ASSERT_TRUE(data_products_[0]);
  const version version_ = data_products_[0]->get_version();
  const std::filesystem::path path_ = data_products_[0]->get_path();
  ASSERT_EQ(version_, version(0, 1, 0));
  ASSERT_EQ(path_, "fixed-parameters/T_lat");
}

TEST(SCRCAPITest, TestGetConfigDataHDF5) {
  LocalFileSystem *fs_ =
      new LocalFileSystem(std::filesystem::path(TESTDIR) / "config.yaml");
  const std::vector<ReadObject::DataProduct *> data_products_ =
      fs_->read_data_products();
  ASSERT_TRUE(data_products_[1]);
  const version version_ = data_products_[1]->get_version();
  const std::filesystem::path path_ = data_products_[1]->get_path();
  ASSERT_EQ(version_, version(0, 20200813, 0));
  ASSERT_EQ(path_, "prob_hosp_and_cfr/data_for_scotland");
}

TEST(SCRCAPITest, TestHashFile) {
  const std::string file_hash_ =
      calculate_hash_from_file(std::filesystem::path(TESTDIR) / "config.yaml");
  std::cout << "HASH: " << file_hash_ << std::endl;
}

TEST(SCRCAPITest, TestDownloadTOMLFile) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline();
  const std::vector<ReadObject::DataProduct *> data_products_ =
      data_pipeline_->file_system->read_data_products();
  ASSERT_TRUE(data_products_[0]);
  ASSERT_NO_THROW(data_pipeline_->download_data_product(data_products_[0]));
}

TEST(SCRCAPITest, TestDownloadHDF5File) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline();
  const std::vector<ReadObject::DataProduct *> data_products_ =
      data_pipeline_->file_system->read_data_products();
  ASSERT_NO_THROW(data_pipeline_->download_data_product(data_products_[1]));
}

TEST(SCRCAPITest, DISABLED_TestAddToRegister) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline(true);
  data_pipeline_->add_to_register("raw-mortality-data");
}

TEST(SCRCAPITest, TestDownloadExternalObject) {
  DataPipelineImpl_ *data_pipeline_ = init_pipeline();
  const std::vector<ReadObject::ExternalObject *> external_objs =
      data_pipeline_->file_system->read_external_objects();

  ASSERT_TRUE(external_objs[0]);
  ASSERT_NO_THROW(data_pipeline_->download_external_object(external_objs[0]));
}