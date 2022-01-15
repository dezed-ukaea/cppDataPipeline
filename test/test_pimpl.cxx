#ifndef TESTDIR
#define TESTDIR ""
#endif
#include "fdp/fdp.hxx"
#include "gtest/gtest.h"

#include <iostream>
#include <fstream>

using namespace FDP;

class PimplTest : public ::testing::Test {
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

  std::string token =
      read_token(std::filesystem::path(getHomeDirectory()) /
                                    ".fair" / "registry" / "token");
  void TearDown() override {}
};

TEST_F(PimplTest, TestDataPipelineInit) {
  const std::filesystem::path config_path_ =
      std::filesystem::path(TESTDIR) / "data" / "write_csv.yaml";
  const std::filesystem::path script_path_ =
      std::filesystem::path(TESTDIR) / "test_script.sh";
  APILogger->set_level(spdlog::level::debug);
  DataPipeline dp = DataPipeline(config_path_, script_path_, token, spdlog::level::debug);

  std::string data_product = "test/csv";
  std::filesystem::path currentLink = std::filesystem::path(dp.link_write(data_product));
  EXPECT_GT(currentLink.string().size(), 1);

  std::ofstream testCSV;
  testCSV.open(currentLink.string());
  testCSV << "Test";
  testCSV.close();

  dp.finalise();

  std::filesystem::path config_path_read_ =
      std::filesystem::path(TESTDIR) / "data" / "read_csv.yaml";

  dp = DataPipeline(config_path_read_, script_path_, token, spdlog::level::debug);

  currentLink = dp.link_read(data_product);
  EXPECT_GT(currentLink.string().size(), 1);

  dp.finalise();

}