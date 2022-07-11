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

        void TearDown() override 
        {
        }
};

namespace fdp = FairDataPipeline;

TEST_F(BuilderTest, TestBuilder) 
{
    fdp::IBuilder::sptr 
        ibuilder = fdp::BuilderFactory::create( "xbuildertest.nc", fdp::IBuilder::Mode::WRITE );

    ASSERT_TRUE( NULL != ibuilder );

    auto grp_bob_ptr = ibuilder->addGroup("bob");

    ASSERT_TRUE( NULL != grp_bob_ptr );

    auto grp_bob_terry_ptr = grp_bob_ptr->addGroup( "terry");

    ASSERT_TRUE( NULL != grp_bob_terry_ptr );

    auto grp_bad = grp_bob_ptr->getGroup( "bad" );

    auto grp_ptr = ibuilder->getGroup("bob/terry");

    grp_bob_terry_ptr->addDim( "dim1", 10);
    auto dim1_ptr = grp_bob_terry_ptr->getDim( "dim1" );

    ASSERT_EQ( dim1_ptr->getName(), "dim1" );
    ASSERT_EQ( dim1_ptr->getSize(), 10 );
    ASSERT_EQ( dim1_ptr->getParentGroup(), grp_bob_terry_ptr );




    ASSERT_TRUE( NULL != grp_ptr );
    ASSERT_EQ( grp_ptr->name(), "terry" );
    ASSERT_EQ( grp_ptr->parent()->name(), "bob" );

    auto parent_parent = grp_ptr->parent()->parent();
    std::string s = parent_parent->name();

    ASSERT_TRUE( NULL != grp_ptr->parent()->parent() );
    ASSERT_EQ( "/", grp_ptr->parent()->parent()->name() );

    int a[1][2][3];
    int* pa = &a[0][0][0];
    int (*pa2)[1][2][3];
    int shape[3] = {1,2,3};

    pa2 = &a;

    fdp::Array::Shape dim = fdp::Array::make_shape( shape, 3 );
    ASSERT_EQ( dim.rank(), 3 );
    ASSERT_EQ( dim.shape()[0], 1 );
    ASSERT_EQ( dim.shape()[1], 2 );
    ASSERT_EQ( dim.shape()[2], 3 );

    fdp::Array::ArrayRef< int > a_ref( pa, dim );
    std::vector< int > vals = {0,1,2,3,4,5,6,7,8,9};

    ASSERT_EQ( a_ref.data(), pa );


    std::vector< fdp::IDimension::sptr > vdims ={ dim1_ptr };

    auto dim_var1_ptr = grp_bob_terry_ptr->addVar( "var1", fdp::DataType::INT, vdims ); 
    dim_var1_ptr->putVar( vals.data() );

    ASSERT_EQ( dim_var1_ptr->parent(), grp_bob_terry_ptr );

    auto var_ptr = grp_bob_terry_ptr->getVar( "var1" );

    ASSERT_EQ( dim_var1_ptr, var_ptr );

    int vals2[10];
    var_ptr->getVar( vals2 );
    auto dim_names = var_ptr->getDims();

    ASSERT_EQ( dim_names.size(), 1 );
    ASSERT_EQ( dim_names[0], "dim1" );

    for( int i = 0; i < 10; ++i )
    {
        ASSERT_EQ( vals2[i], vals[i] );

    }


    {
        int data[2][3];
        int dim1[2] = {1,2};
        int dim2[3] ={3,4,5};
        int val=0;

        for( int i = 0; i < 2; i++)
        {
            for( int j = 0; j < 3; ++j )
                data[i][j] = val++;
        }

        fdp::DimensionDefinition dimdef1, dimdef2;

        dimdef1.name = "dim1";
        dimdef1.units = "units1";
        dimdef1.description = "description1";
        dimdef1.dataType = fdp::DataType::INT;
        dimdef1.size = 2;

        dimdef2.name = "dim2";
        dimdef2.units = "units2";
        dimdef2.description = "description2";
        dimdef2.dataType = fdp::DataType::INT;
        dimdef2.size = 3;

        fdp::ArrayDefinition arrdef;
        arrdef.name = "arr1";
        arrdef.units="arrunits";
        arrdef.description = "arrdescription";
        arrdef.dataType = fdp::DataType::INT;
        arrdef.dimension_names = { dimdef1.name, dimdef2.name };
        arrdef.shape = { 2,3 };

        fdp::Builder builder2( "xtest.nc", fdp::IBuilder::Mode::WRITE );

        int status = 0;

        status = builder2.writeDimension( "grp", dimdef1, dim1 );
        ASSERT_EQ( 0, status );

        status = builder2.writeDimension( "grp", dimdef2, dim2 );
        ASSERT_EQ( 0, status );

        status = builder2.writeArray( "grp", arrdef, data );
        ASSERT_EQ( 0, status );

        //write already existing dim name
        status = builder2.writeDimension( "grp", dimdef1, dim1 );
        ASSERT_TRUE( 0 != status );

        status = builder2.writeArray( "grp", arrdef, data );
        ASSERT_TRUE( 0 != status );

        fdp::ArrayDefinition arrdef_bad;
        arrdef_bad.name = "arrbad";
        arrdef_bad.units="arrunits";
        arrdef_bad.description = "arrdescription";
        arrdef_bad.dataType = fdp::DataType::INT;

        arrdef_bad.dimension_names = { "baddim1", "baddim2" };

        status = builder2.writeArray( "grp", arrdef_bad, data );
        ASSERT_TRUE( 0 != status );

        fdp::ArrayDefinition arrdef_;

        status = builder2.readArray_metadata( "grp", "arr1",  arrdef_ );

        ASSERT_EQ( 0, status );
        ASSERT_EQ( "arr1", arrdef_.name );
        ASSERT_EQ( "arrunits", arrdef_.units );
        ASSERT_EQ( "arrdescription", arrdef_.description );
        ASSERT_EQ( arrdef_.shape, arrdef.shape );
        ASSERT_EQ( arrdef_.dimension_names, arrdef.dimension_names );
        ASSERT_EQ( arrdef_.dataType, fdp::DataType::INT );

        int data_[2][3];
        status = builder2.readArray_data( "grp", arrdef_, data_ );

        for( int i = 0; i < 2; ++i )
            for( int j = 0; j < 3; ++j )
                ASSERT_EQ( data[i][j], data_[i][j] );

        fdp::DimensionDefinition dimdef1_;
        status = builder2.readDim_metadata( "grp", "dim1",  dimdef1_ );

        ASSERT_EQ( 0, status );
        ASSERT_EQ( "dim1", dimdef1_.name );
        ASSERT_EQ( 2, dimdef1_.size );

        ASSERT_EQ( "units1", dimdef1_.units );
        ASSERT_EQ( "description1", dimdef1_.description );

        int dim1_[2];
        status = builder2.readDim_data( "grp", dimdef1_, dim1_ );

        for( int j = 0; j < 2; ++j )
            ASSERT_EQ( dim1[j], dim1_[j] );

        fdp::DimensionDefinition dimdef2_;
        status = builder2.readDim_metadata( "grp", "dim2", dimdef2_ );

        ASSERT_EQ( 0, status );
        ASSERT_EQ( "dim2", dimdef2_.name );
        ASSERT_EQ( 3, dimdef2_.size );

        ASSERT_EQ( "units2", dimdef2_.units );
        ASSERT_EQ( "description2", dimdef2_.description );

        int dim2_[3];
        status = builder2.readDim_data( "grp", dimdef2_, dim2_ );

        for( int j = 0; j < 3; ++j )
            ASSERT_EQ( dim2[j], dim2_[j] );



    }



} //! [TestGetById]

