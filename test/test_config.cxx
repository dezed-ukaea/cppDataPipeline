#ifndef TESTDIR
#define TESTDIR ""
#endif

#include <iostream>
#include <fstream>

#include "fdp/objects/config.hxx"
#include "fdp/registry/api.hxx"
//#include "fdp/registry/datapipeline.hxx"
#include "gtest/gtest.h"

#include "fdp/utilities/json.hxx"

#include "fdp/utilities/logging.hxx"

using namespace FairDataPipeline;

class ConfigTest : public ::testing::Test {
protected:
  void SetUp() override {}
  
  static std::string getHomeDirectory() {
    std::string HomeDirectory;
#ifdef _WIN32
    HomeDirectory = getenv("HOMEDRIVE");
    HomeDirectory += getenv("HOMEPATH");
#else
    HomeDirectory = getenv("HOME");
#endif
    return HomeDirectory;
  }

  static Config::sptr config(bool use_local = true, std::string config = "write_csv.yaml") {

      std::string home_dir = getHomeDirectory();
      std::string token =
          read_token (ghc::filesystem::path( home_dir) / ".fair" / "registry" / "token");

      const ghc::filesystem::path config_path_ =
          ghc::filesystem::path(TESTDIR) / "data" / config;
      const ghc::filesystem::path script_path_ =
          ghc::filesystem::path(TESTDIR) / "test_script.sh";
      logger::get_logger()->set_level( logging::LOG_LEVEL::DEBUG );

      return Config::construct(config_path_, script_path_, token,
              use_local ? RESTAPI::LOCAL : RESTAPI::REMOTE ); 
  }

  void TearDown() override {}
};

TEST_F(ConfigTest, TestMetaData) {

    auto cnf = config();
    auto meta = cnf->meta_data_();

    EXPECT_EQ( meta["description"].as<std::string>(), "Write csv file");
    EXPECT_EQ( meta["local_data_registry_url"].as<std::string>(), "http://127.0.0.1:8000/api/");
    EXPECT_EQ( meta["remote_data_registry_url"].as<std::string>(), "https://data.scrc.uk/api/");
    EXPECT_EQ( meta["default_input_namespace"].as<std::string>(), "testing");
    EXPECT_EQ( meta["default_output_namespace"].as<std::string>(), "testing");
    //EXPECT_EQ(cnf->meta_data_()["write_data_store"].as<std::string>(), "tmp");
    EXPECT_EQ( meta["local_repo"].as<std::string>(), "./");
    EXPECT_EQ( meta["public"].as<std::string>(), "true");
    EXPECT_EQ( meta["latest_commit"].as<std::string>(), "52008720d240693150e96021ea34ac6fffe05870");
    EXPECT_EQ( meta["remote_repo"].as<std::string>(), "https://github.com/FAIRDataPipeline/cppDataPipeline");
}

TEST_F(ConfigTest, TestLinkWrite){
    Config::sptr cnf = config();
  std::string data_product = "test/csv";
  ghc::filesystem::path currentLink = ghc::filesystem::path(cnf->link_write(data_product));
  EXPECT_GT(currentLink.string().size(), 1);

  std::ofstream testCSV;
  testCSV.open(currentLink.string());
  testCSV << "Test";
  testCSV.close();

  cnf->finalise();
  
}

TEST_F(ConfigTest, TestLinkRead){
    Config::sptr cnf = config(true, "read_csv.yaml");
  std::string data_product = "test/csv";
  ghc::filesystem::path currentLink = cnf->link_read(data_product);
  EXPECT_GT(currentLink.string().size(), 1);

  cnf->finalise();
}
