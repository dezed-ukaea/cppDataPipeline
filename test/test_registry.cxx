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

using namespace FairDataPipeline;

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

  DataPipeline::sptr init_pipeline(bool use_local = true) {

    const ghc::filesystem::path config_path_ =
        ghc::filesystem::path(TESTDIR) / "config.yaml";
    const ghc::filesystem::path script_path_ =
        ghc::filesystem::path(TESTDIR) / "test_script.sh";

    logger::get_logger()->set_level( logging::LOG_LEVEL::DEBUG );

    return DataPipeline::construct(
        config_path_.string(),
        script_path_.string(),
        token );
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

  logger::get_logger()->set_level( logging::LOG_LEVEL::DEBUG );
  
  auto dp = DataPipeline::construct( config_path_.string()
          , script_path_.string()
          , token );
}

TEST_F(RegistryTest, TestLogLevelSet) {
  init_pipeline();
  ASSERT_EQ( logger::get_logger()->get_level(), logging::LOG_LEVEL::DEBUG );
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

TEST_F(RegistryTest, TestLogger) {
    logging::OStreamSink::sptr sink = logging::OStreamSink::create( logging::INFO, std::cout );

    logging::Logger::sptr logger = logging::Logger::create( logging::INFO, sink );
    logger->trace() << " " << 10 << " " << "TRACE";
    logger->info() << " " << 50 << " " << "INFO";
    
    logger->warn() << " " << 100 << " " << "WARN";

    logger::get_logger()->info() << " WARNINF";
}
