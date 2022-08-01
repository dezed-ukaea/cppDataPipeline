#ifndef TESTDIR
#define TESTDIR ""
#endif

#include "gtest/gtest.h"

#include "fdp/fdpnetcdf.hxx"

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

TEST_F(BuilderTest, TestInterface) 
{
    fdp::IFile::sptr 
        ibuilder = fdp::FileFactory::create( "xbuildertest.nc", fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != ibuilder );

    auto grp_bob_ptr = ibuilder->addGroup("bob");
    grp_bob_ptr->putAtt( "int", 123 );
    grp_bob_ptr->putAtt( "float", 123.123f );
    grp_bob_ptr->putAtt( "string", "a string" );

    int ivals[] = { 1, 2, 3 };
    float fvals[] = { 1.0, 2.0, 3.0 };
    double dvals[] = { 1.0, 2.0, 3.0 };

    const char* strings[3] = {"a", "test", "bob"};

    grp_bob_ptr->putAtt( "ints", 3, ivals );
    grp_bob_ptr->putAtt( "floats", 3, fvals );
    grp_bob_ptr->putAtt( "doubles", 3, dvals );
    grp_bob_ptr->putAtt( "some_strings", 3, &strings[0] );

    ASSERT_TRUE( NULL != grp_bob_ptr );

    auto grp_bob_terry_ptr = grp_bob_ptr->addGroup( "terry");

    ASSERT_TRUE( NULL != grp_bob_terry_ptr );

    auto grp_bad = grp_bob_ptr->getGroup( "bad" );

    ASSERT_TRUE( NULL == grp_bad );

    auto grp_ptr = ibuilder->getGroup("bob/terry");


    //std::string dimx = "grp1/grp2/grp3i/dimx";
    //auto dimx_ptr = ibuilder->addDim( dimx, 5 );
    //ASSERT_TRUE( dimx_ptr != NULL );



    grp_bob_terry_ptr->addDim( "dim1", 10);

    auto dim1_ptr = grp_bob_terry_ptr->getDim( "dim1" );

    ASSERT_EQ( dim1_ptr->getName(), "dim1" );
    ASSERT_EQ( dim1_ptr->getSize(), 10 );
    ASSERT_EQ( dim1_ptr->getParentGroup(), grp_bob_terry_ptr );

    //auto grp_with_dim_name = grp_bob_terry_ptr->addGroup( "dim1" );

    //ASSERT_TRUE( NULL != grp_with_dim_name );
    //return;


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

    //fdp::Array::Shape dim = fdp::Array::make_shape( shape, 3 );
    //ASSERT_EQ( dim.rank(), 3 );

    //fdp::Array::ArrayRef< int > a_ref( pa, dim );
    std::vector< int > vals = {0,1,2,3,4,5,6,7,8,9};

    //ASSERT_EQ( a_ref.data(), pa );


    std::vector< fdp::IDimension::sptr > vdims ={ dim1_ptr };

    auto dim_var1_ptr = grp_bob_terry_ptr->addVar( "var1", fdp::DataType::INT, vdims ); 
    dim_var1_ptr->putVar( vals.data() );
    dim_var1_ptr->putAtt( "astring", "the_string" );
    dim_var1_ptr->putAtt( "ints", 3, ivals );

    ASSERT_EQ( dim_var1_ptr->parent(), grp_bob_terry_ptr );

    auto var_ptr = grp_bob_terry_ptr->getVar( "var1" );

    ASSERT_EQ( dim_var1_ptr, var_ptr );

    int vals2[10];
    var_ptr->getVar( vals2 );
    auto dim_ptrs = var_ptr->getDims();

    ASSERT_EQ( dim_ptrs.size(), 1 );
    ASSERT_EQ( dim_ptrs[0]->getName(), "dim1" );

    for( int i = 0; i < 10; ++i )
    {
        ASSERT_EQ( vals2[i], vals[i] );

    }

}

TEST_F(BuilderTest, TestInterface2) 
{
        double data[2][3];
        int dim1[2] = {1,2};
        float dim2[3] ={3.1,4.2,5.3};
        int val=0;

        for( int i = 0; i < 2; i++)
        {
            for( int j = 0; j < 3; ++j )
                data[i][j] = val++ +.1;
        }

        fdp::DimensionDefinition dimdef1, dimdef2;

        dimdef1.units = "units1";
        dimdef1.description = "description1";
        dimdef1.dataType = fdp::DataType::INT;
        dimdef1.size = 2;

        dimdef2.units = "units2";
        dimdef2.description = "description2";
        dimdef2.dataType = fdp::DataType::FLOAT;
        dimdef2.size = 3;

        fdp::ArrayDefinition arrdef;
        arrdef.units="arrunits";
        arrdef.description = "arrdescription";
        arrdef.dataType = fdp::DataType::DOUBLE;
        arrdef.dimension_names = { "grp/dim1", "grp/dim2" };
        //arrdef.shape = { 2,3 };

        fdp::Builder builder2( "xtest.nc", fdp::IFile::Mode::WRITE );

        int status = 0;

        status = builder2.writeDimension( "grp/dim1", dimdef1, dim1 );
        ASSERT_EQ( 0, status );

        status = builder2.writeDimension( "grp/dim2", dimdef2, dim2 );
        ASSERT_EQ( 0, status );

        status = builder2.writeArray( "grp/arr1", arrdef, data );
        ASSERT_EQ( 0, status );

        //write already existing dim name
        status = builder2.writeDimension( "grp/dim1", dimdef1, dim1 );
        ASSERT_TRUE( 0 != status );

        status = builder2.writeArray( "grp/arr1", arrdef, data );
        ASSERT_TRUE( 0 != status );

        fdp::ArrayDefinition arrdef_bad;
        arrdef_bad.units="arrunits";
        arrdef_bad.description = "arrdescription";
        arrdef_bad.dataType = fdp::DataType::INT;

        arrdef_bad.dimension_names = { "baddim1", "baddim2" };

        status = builder2.writeArray( "grp/arr1", arrdef_bad, data );
        ASSERT_TRUE( 0 != status );

        fdp::ArrayDefinition arrdef_;

        status = builder2.readArray_metadata( "grp/arr1",  arrdef_ );

        ASSERT_EQ( 0, status );
        ASSERT_EQ( "arrunits", arrdef_.units );
        ASSERT_EQ( "arrdescription", arrdef_.description );
        //ASSERT_EQ( arrdef_.shape, arrdef.shape );
        ASSERT_EQ( arrdef_.dimension_names, arrdef.dimension_names );
        ASSERT_EQ( arrdef_.dataType, fdp::DataType::DOUBLE );

        double data_[2][3];

        status = builder2.readArray_data( "grp/arr1", arrdef_, data_ );

        for( int i = 0; i < 2; ++i )
            for( int j = 0; j < 3; ++j )
                ASSERT_EQ( data[i][j], data_[i][j] );

        fdp::DimensionDefinition dimdef1_;
        status = builder2.readDim_metadata( "grp/dim1",  dimdef1_ );

        ASSERT_EQ( 0, status );
        ASSERT_EQ( 2, dimdef1_.size );

        ASSERT_EQ( "units1", dimdef1_.units );
        ASSERT_EQ( "description1", dimdef1_.description );

        int dim1_[2];

        status = builder2.readDim_data( "grp/dim1", dimdef1_, dim1_ );

        ASSERT_EQ( 0, status );

        for( int j = 0; j < 2; ++j )
            ASSERT_EQ( dim1[j], dim1_[j] );

        fdp::DimensionDefinition dimdef2_;
        status = builder2.readDim_metadata( "grp/dim2", dimdef2_ );

        ASSERT_EQ( 0, status );
        ASSERT_EQ( 3, dimdef2_.size );

        ASSERT_EQ( "units2", dimdef2_.units );
        ASSERT_EQ( "description2", dimdef2_.description );

        float dim2_[3];

        status = builder2.readDim_data( "grp/dim2", dimdef2_, dim2_ );

        for( int j = 0; j < 3; ++j )
            ASSERT_EQ( dim2[j], dim2_[j] );

        fdp::DimensionDefinition dimdef3;
        dimdef3.units = "";
        dimdef3.description = "";
        dimdef3.dataType = fdp::DataType::STRING;
        dimdef3.size = 3;

        const char* dim3data[3] = {"a", "test", "bob"};


        status = builder2.writeDimension("grp/dim3", dimdef3, dim3data );

        int iatts[] = { 1,2,3 };
        builder2.putAtt( "grp/dim3", "dim3_att", 3, iatts );

        fdp::IAtt::sptr att_ptr;
        status = builder2.getAtt( "grp/dim3", "dim3_att",  att_ptr );

        ASSERT_EQ( 0, status );

        ASSERT_TRUE( NULL != att_ptr );

        ASSERT_EQ( fdp::DataType::INT, att_ptr->getAttType() );
        ASSERT_EQ( fdp::IAtt::ATTRIBUTE_VAR_TYPE, att_ptr->getClassType() );
        ASSERT_EQ( 3, att_ptr->getAttLength() );

        status = builder2.getAtt( "grp/dim3", "bad_att",  att_ptr );

        ASSERT_NE( 0, status );

        ASSERT_TRUE( NULL == att_ptr );

} 
