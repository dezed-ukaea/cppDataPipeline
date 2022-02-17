#ifndef TESTDIR
#define TESTDIR ""
#endif

#include "fdp/registry/api.hxx"
#include "fdp/fdp.hxx"
#include "fdp/objects/metadata.hxx"
#include "gtest/gtest.h"
#include "fdp/utilities/json.hxx"
#include "fdp/utilities/logging.hxx"

using namespace FairDataPipeline;

class ApiTest : public ::testing::Test {
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
  ghc::filesystem::path read_csv_config = ghc::filesystem::path(TESTDIR) / "data" / "read_csv.yaml";
  std::string api_url_ = "http://127.0.0.1:8000/api";
  FairDataPipeline::API *api_ = new FairDataPipeline::API(api_url_);
  std::string token =
      read_token(ghc::filesystem::path(getHomeDirectory()) /
                                    ".fair" / "registry" / "token");
  void TearDown() override {
    delete api_;
  }
};

//! [TestGetById]
TEST_F(ApiTest, TestGetById) {
  Json::Value author = api_->get_request(std::string("author/?name=Interface%20Test"))[0]["url"];
  APILogger->info("Author: {0}", author.asString());
  int author_id_ = ApiObject::get_id_from_string( author.asString() );
  ASSERT_EQ(api_->get_by_id(std::string("author"), author_id_)["name"].asString(), std::string("Interface Test"));
} //! [TestGetById]

//![TestGetEntry409]
TEST_F(ApiTest, TestGetEntry409) {
  Json::Value post_data;
  post_data["name"] = std::string("Interface Test");
  post_data["identifier"] = std::string("https://orcid.org/000-0000-0000-0000");
  Json::Value author = api_->post(std::string("author"), post_data, token);
  ASSERT_EQ(author["name"].asString(), std::string("Interface Test"));
} //![TestGetEntry409]

//![TestPostEntry]
TEST_F(ApiTest, TestPostEntry) {
  Json::Value post_data;
  post_data["root"] = "http://test.com";
  Json::Value storage_root = api_->post("storage_root", post_data, token);
  ASSERT_EQ(storage_root["root"], "http://test.com");
}
