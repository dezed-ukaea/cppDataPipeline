#ifndef TESTDIR
#define TESTDIR ""
#endif
#include "fdp/fdp.hxx"
#include "fdp/objects/metadata.hxx"
#include "fdp/registry/data_io.hxx"
#include "gtest/gtest.h"
#include <ghc/filesystem.hpp>
#include <ostream>
#include <vector>

using namespace fdp;

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

  DataPipeline *init_pipeline(bool use_local = true) {

    const ghc::filesystem::path config_path_ =
        ghc::filesystem::path(TESTDIR) / "config.yaml";
    const ghc::filesystem::path script_path_ =
        ghc::filesystem::path(TESTDIR) / "test_script.sh";
    APILogger->set_level(spdlog::level::debug);

    return new DataPipeline(
        config_path_.string(),
        script_path_.string(),
        token,
        spdlog::level::debug );
  }

  std::string token =
      read_token(ghc::filesystem::path(getHomeDirectory()) /
                                    ".fair" / "registry" / "token");
  void TearDown() override {}
};

TEST_F(RegistryTest, TestDataPipelineInit) {
  const ghc::filesystem::path config_path_ =
      ghc::filesystem::path(TESTDIR) / "config.yaml";
  const ghc::filesystem::path script_path_ =
      ghc::filesystem::path(TESTDIR) / "test_script.sh";
  APILogger->set_level(spdlog::level::debug);
  DataPipeline(config_path_.string(), script_path_.string(), token, spdlog::level::debug);
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
      calculate_hash_from_file(ghc::filesystem::path(TESTDIR) / "config.yaml");
  std::cout << "HASH: " << file_hash_ << std::endl;
}
