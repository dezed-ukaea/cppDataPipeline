#ifndef TESTDIR
#define TESTDIR ""
#endif

#include "gtest/gtest.h"

#include "fdp/fdpnetcdf.hxx"
#include "fdp/fdpnetcdf2.hxx"

//using namespace FairDataPipeline;

class FileTest : public ::testing::Test {
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

        void TearDown() override 
        {
        }
};

namespace fdp = FairDataPipeline;

TEST_F(FileTest, TestInterface) 
{
    fdp::FileFactory2 fac;

    fdp::IFile2::sptr 
        ibuilder = fac.create( "test_file.nc", fdp::IFile2::Mode::WRITE );

    ASSERT_TRUE( NULL != ibuilder );

    //fdp::IGroup2::sptr grp_root_ptr = std::dynamic_pointer_cast< fdp::IGroup2 >( ibuilder );

    fdp::IGroup2::sptr grp_bob_ptr = ibuilder->requireGroup("bob");
#if 0
    //grp_bob_ptr->putAtt( "int", 123 );
    //grp_bob_ptr->putAtt( "float", 123.123f );
    //grp_bob_ptr->putAtt( "string", "a string" );

    int ivals[] = { 1, 2, 3 };
    float fvals[] = { 1.0, 2.0, 3.0 };
    double dvals[] = { 1.0, 2.0, 3.0 };

    const char* strings[3] = {"a", "test", "bob"};

    grp_bob_ptr->putAtt( "ints", 3, ivals );
    grp_bob_ptr->putAtt( "floats", 3, fvals );
    grp_bob_ptr->putAtt( "doubles", 3, dvals );
    grp_bob_ptr->putAtt( "some_strings", 3, &strings[0] );

    ASSERT_TRUE( NULL != grp_bob_ptr );

    auto grp_bob_terry_ptr = grp_bob_ptr->requireGroup( "terry");

    ASSERT_TRUE( NULL != grp_bob_terry_ptr );

    auto grp_bad = grp_bob_ptr->getGroup( "bad" );

    ASSERT_TRUE( NULL == grp_bad );

    auto grp_ptr = ibuilder->getGroup("bob/terry");
#endif
}

