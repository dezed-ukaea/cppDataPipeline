#ifndef TESTDIR
#define TESTDIR ""
#endif

//#include "fdp/registry/api.hxx"
#include "fdp/fdpnetcdf.hxx"
//#include "fdp/objects/metadata.hxx"
#include "gtest/gtest.h"
//#include "fdp/utilities/json.hxx"
//#include "fdp/utilities/logging.hxx"

//using namespace FairDataPipeline;

class BuilderTest : public ::testing::Test {
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
  //ghc::filesystem::path read_csv_config = ghc::filesystem::path(TESTDIR) / "data" / "read_csv.yaml";
  //std::string api_url_ = "http://127.0.0.1:8000/api";

  //FairDataPipeline::API::sptr api_ = FairDataPipeline::API::construct(api_url_);

  //std::string token =
  //    read_token(ghc::filesystem::path(getHomeDirectory()) /
  //                                  ".fair" / "registry" / "token");
  void TearDown() override 
  {
  }
};

namespace fdp = FairDataPipeline;

//! [TestGetById]
TEST_F(BuilderTest, TestBuilder) 
{
	fdp::Builder2::sptr 
		builder = fdp::Builder2::create( "xbuildertest.nc", fdp::Builder2::Mode::WRITE );

	ASSERT_TRUE( NULL != builder );

	auto grp_bob = builder->addGroup("bob");

	ASSERT_TRUE( NULL != grp_bob );

	auto grp_bob_terry = grp_bob->addGroup( "terry");

	ASSERT_TRUE( NULL != grp_bob_terry );

	auto grp_bad = grp_bob->getGroup( "bad" );

	auto grp = builder->getGroup("bob/terry");

	ASSERT_TRUE( NULL != grp );
	ASSERT_EQ( grp->name(), "terry" );
	ASSERT_EQ( grp->parent()->name(), "bob" );

	auto parent_parent = grp->parent()->parent();
	std::string s = parent_parent->name();

	ASSERT_TRUE( NULL != grp->parent()->parent() );
	ASSERT_EQ( "/", grp->parent()->parent()->name() );

} //! [TestGetById]

