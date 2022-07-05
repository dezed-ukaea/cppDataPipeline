#ifndef TESTDIR
#define TESTDIR ""
#endif

#include "gtest/gtest.h"

#include "fdp/fdpnetcdf.hxx"
#include "fdp/fdparrayref.hxx"

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
	fdp::IBuilder::sptr 
		builder = fdp::Builder::create( "xbuildertest.nc", fdp::IBuilder::Mode::WRITE );

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
	
	int a[1][2][3];
	int* pa = &a[0][0][0];
	int shape[3] = {1,2,3};

	fdp::Array::Shape dim = fdp::Array::make_shape<int>( shape, 3);
	ASSERT_EQ( dim.rank(), 3 );
	ASSERT_EQ( dim.shape()[0], 1 );
	ASSERT_EQ( dim.shape()[1], 2 );
	ASSERT_EQ( dim.shape()[2], 3 );

	fdp::Array::ArrayRef< int > a_ref( pa, dim );

	ASSERT_EQ( a_ref.data(), pa );

} //! [TestGetById]

