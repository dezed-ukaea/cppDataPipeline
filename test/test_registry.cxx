#ifndef TESTDIR
#define TESTDIR ""
#endif
#include "fdp/fdp.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/registry/datapipeline.hxx"
#include "fdp/registry/data_io.hxx"
#include "gtest/gtest.h"
#include <filesystem>
#include <ostream>
#include <vector>

using namespace FDP;

class RegistryTest : public ::testing::Test {
protected:
  void SetUp() override {}
  std::string getHomeDirectory() {
    std::string HomeDirectory;
#ifdef _WIN32
    HomeDirectory = getenv("HOMEDRIVE");
    HomeDirectory += getenv("HOMEPATH");
#else
    HomeDirectory = getenv("HOME");
#endif
    return HomeDirectory;
  }

  DataPipelineImpl_ *init_pipeline(bool use_local = true) {

    const std::filesystem::path config_path_ =
        std::filesystem::path(TESTDIR) / "config.yaml";
    const std::filesystem::path script_path_ =
        std::filesystem::path(TESTDIR) / "test_script.sh";
    APILogger->set_level(spdlog::level::debug);

    return new DataPipelineImpl_(config_path_, script_path_, token, spdlog::level::debug,
                                 static_cast<RESTAPI>(use_local));
  }

  std::string token =
      read_token(std::filesystem::path(getHomeDirectory()) /
                                    ".fair" / "registry" / "token");
  void TearDown() override {}
};

TEST_F(RegistryTest, TestDataPipelineInit) {
  const std::filesystem::path config_path_ =
      std::filesystem::path(TESTDIR) / "config.yaml";
  const std::filesystem::path script_path_ =
      std::filesystem::path(TESTDIR) / "test_script.sh";
  APILogger->set_level(spdlog::level::debug);
  DataPipeline(config_path_, script_path_, token, spdlog::level::debug);
}

TEST_F(RegistryTest, TestLogLevelSet) {
  init_pipeline();
  ASSERT_EQ(spdlog::get_level(), spdlog::level::debug);
}

TEST_F(RegistryTest, TestURLEncode) {
  const std::string start_string_ = "fixed-parameters/T_lat";
  const std::string expected_ = "fixed-parameters%2FT_lat";

  ASSERT_EQ(url_encode(start_string_), expected_);
}

TEST_F(RegistryTest, TestHashFile) {
  const std::string file_hash_ =
      calculate_hash_from_file(std::filesystem::path(TESTDIR) / "config.yaml");
  std::cout << "HASH: " << file_hash_ << std::endl;
}