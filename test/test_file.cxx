#ifndef TESTDIR
#define TESTDIR ""
#endif

#include "gtest/gtest.h"
#include<cstring>

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

    fdp::IGroup2::sptr grp_bob_ptr = ibuilder->requireGroup("grp1");
    grp_bob_ptr->putAtt( "int", 123 );
    

    fdp::IGroupAtt2::sptr grp_att_ptr;

    grp_att_ptr = grp_bob_ptr->putAtt( "float", 123.123f );

    ASSERT_TRUE( NULL != grp_att_ptr );
    ASSERT_TRUE( grp_att_ptr->name() == "float" );
    ASSERT_TRUE( grp_att_ptr->getAttType() == fdp::FLOAT );
    ASSERT_TRUE( grp_att_ptr->getAttLength() == 1 );


    grp_att_ptr = grp_bob_ptr->putAtt( "string", "a string" );
    //
#if 1

    int ivals[] = { 1, 2, 3 };
    float fvals[] = { 1.0, 2.0, 3.0 };
    double dvals[] = { 1.0, 2.0, 3.0 };

    const char* strings[3] = {"a", "test", "bob"};

    std::string s[3] = {"a","b","c"};

    grp_att_ptr = grp_bob_ptr->putAtt( "ints", 3, ivals );

    ASSERT_TRUE( grp_att_ptr->name() == "ints" );
    ASSERT_TRUE( grp_att_ptr->getAttType() == fdp::INT );
    ASSERT_TRUE( grp_att_ptr->getAttLength() == 3 );

    int ivals_[3] = {0,0,0};
    grp_att_ptr->getValues( ivals_ );


    ASSERT_TRUE( 0 == std::memcmp( ivals, ivals_, sizeof(int)*3 ) );

    //grp_att_ptr = grp_bob_ptr->putAtt( "more_strings", &s[0] );
    grp_att_ptr = grp_bob_ptr->putAtt( "floats", 3, fvals );
    grp_att_ptr = grp_bob_ptr->putAtt( "doubles", 3, dvals );
    grp_att_ptr = grp_bob_ptr->putAtt( "some_strings", 3, &strings[0] );  

    ASSERT_TRUE( NULL != grp_bob_ptr );

    auto grp_12_ptr = grp_bob_ptr->requireGroup( "grp1.1");

    ASSERT_TRUE( NULL != grp_12_ptr );

    auto grp_bad = grp_bob_ptr->getGroup( "bad" );

    ASSERT_TRUE( NULL == grp_bad );

    auto grp_ptr = ibuilder->getGroup("grp1/grp1.1");

    ASSERT_TRUE( NULL != grp_ptr );
    ASSERT_TRUE( grp_12_ptr == grp_ptr );


#endif
}

TEST_F(FileTest, TestBuilder) 
{
    fdp::CoordinatVariableDefinition cv;

    cv.name = "grp1/grp2/cv1";


    fdp::Builder2 builder( "test_builder.nc", fdp::IFile2::WRITE );

    builder.prepare( cv );

    cv.name = "cv1";

    builder.prepare( cv );


}
