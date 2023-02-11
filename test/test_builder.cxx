#ifndef TESTDIR
#define TESTDIR ""
#endif

#include "gtest/gtest.h"

#include "fdp/fdpnetcdf.hxx"

//using namespace FairDataPipeline;

class BuilderTest : public ::testing::Test {
    protected:
        void SetUp() override 
        {
        }

        std::string getHomeDirectory() 
        {
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

TEST_F(BuilderTest, TestCheckStatus) 
{
    ASSERT_TRUE( FDP_FILE_ISNOERR(0) );
    ASSERT_FALSE( FDP_FILE_ISNOERR(1) );
    ASSERT_TRUE( FDP_FILE_ISERR(1) );
    ASSERT_FALSE( FDP_FILE_ISERR(0) );
}

TEST_F(BuilderTest, TestStringSplit) 
{
    ASSERT_EQ( fdp::str_strip_right( "/grp1/grp2/", "/" ), "/grp1/grp2" );
    ASSERT_EQ( fdp::str_strip_right( "/grp1/grp2//", "/" ), "/grp1/grp2" );
    ASSERT_EQ( fdp::str_strip_left( "//grp1/grp2/", "/" ), "grp1/grp2/" );

    ASSERT_EQ( fdp::str_strip( "/grp1/grp2/", "/" ), "grp1/grp2" );
    ASSERT_EQ( fdp::str_strip( "//grp1/grp2//", "/" ), "grp1/grp2" );
}


TEST_F(BuilderTest, TestGroupItemType ) 
{
    fdp::PARENT_ITEM_TYPE parent_item = fdp::split_item_name( "/grp1/grp2/itm" );

    ASSERT_EQ( parent_item.first, "/grp1/grp2" );
    ASSERT_EQ( parent_item.second, "itm" );

    parent_item = fdp::split_item_name( "grp1/grp2/itm" );

    ASSERT_EQ( parent_item.first, "grp1/grp2" );
    ASSERT_EQ( parent_item.second, "itm" );

    parent_item = fdp::split_item_name( "/itm" );

    ASSERT_EQ( parent_item.first, "/" );
    ASSERT_EQ( parent_item.second, "itm" );

    parent_item = fdp::split_item_name( "itm" );

    ASSERT_EQ( parent_item.first, "" );
    ASSERT_EQ( parent_item.second, "itm" );

}

TEST_F(BuilderTest, TestInterface) 
{
    return;
#if 0
    fdp::IFile::sptr 
        ibuilder = fdp::FileFactory::create( "xbuildertest.nc", fdp::IFile::Mode::WRITE );


    ASSERT_TRUE( NULL != ibuilder );

    auto root_grp_ptr = ibuilder->requireGroup( "" );
    ASSERT_EQ( ibuilder, root_grp_ptr );

    auto grp_bob_ptr = ibuilder->requireGroup("bob");

    auto tmp_grp_ptr = grp_bob_ptr->requireGroup("");

    ASSERT_EQ( tmp_grp_ptr, grp_bob_ptr );

    // grp_bob_ptr->putAtt( "int", 123 );
    //grp_bob_ptr->putAtt( "float", 123.123f );
    char* psz_ = "a string";
    grp_bob_ptr->putAtt( "string", fdp::DataType::STRING, 1, &psz_ );

    int ivals[] = { 1, 2, 3 };
    float fvals[] = { 1.0, 2.0, 3.0 };
    double dvals[] = { 1.0, 2.0, 3.0 };

    const char* strings[3] = {"a", "test", "bob"};

    grp_bob_ptr->putAtt( "ints", fdp::DataType::INT, 3, &ivals[0] );
    grp_bob_ptr->putAtt( "floats", fdp::DataType::FLOAT, 3, fvals );
    grp_bob_ptr->putAtt( "doubles", fdp::DataType::DOUBLE, 3, dvals );
    grp_bob_ptr->putAtt2( "some_strings", 3, &strings[0] );

    ASSERT_TRUE( NULL != grp_bob_ptr );

    auto grp_bob_terry_ptr = grp_bob_ptr->requireGroup( "terry");

    ASSERT_TRUE( NULL != grp_bob_terry_ptr );

    auto grp_bad = grp_bob_ptr->getGroup( "bad" );

    ASSERT_TRUE( NULL == grp_bad );

    auto grp_ptr = ibuilder->getGroup("bob/terry");

    ASSERT_EQ( grp_ptr->requireGroup( "/" ), root_grp_ptr );
    ASSERT_EQ( grp_ptr->requireGroup( "/bob" ), grp_bob_ptr );


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
    ASSERT_EQ( grp_ptr->getName(), "terry" );
    ASSERT_EQ( grp_ptr->parent()->getName(), "bob" );

    auto parent_parent = grp_ptr->parent()->parent();
    std::string s = parent_parent->getName();

    ASSERT_TRUE( NULL != grp_ptr->parent()->parent() );
    ASSERT_EQ( "/", grp_ptr->parent()->parent()->getName() );

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

    fdp::IVar::sptr dim_var1_ptr = grp_bob_terry_ptr->addVar( "var1", fdp::DataType::INT, vdims ); 
    dim_var1_ptr->putVar( vals.data() );
    char* psz = "the_string";
    dim_var1_ptr->putAtt( "astring", fdp::DataType::STRING, 1, &psz );
    //dim_var1_ptr->putAtt( "ints", 3, (const int*)(&ivals[0]) );
    dim_var1_ptr->putAtt( "ints", fdp::DataType::INT, 3, &ivals[0] );

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
#endif
}

TEST_F(BuilderTest, TestInterfaceRemoteDim)
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";
    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    auto grp2_ptr = file_ptr->requireGroup( "/grp1/grp2" );
    auto grp1_ptr = grp2_ptr->parent();



    auto dim_ptr = grp1_ptr->addDim( "dim1", 3);

    std::vector< fdp::IDimension::sptr > vdims ={ dim_ptr };

    auto var_ptr = grp2_ptr->addVar( "var1", fdp::DataType::INT, vdims ); 

    ASSERT_TRUE( NULL != var_ptr );


} 

TEST_F(BuilderTest, TestInterface2) 
{

#if 0
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

    std::remove( "xtest.nc" );
    fdp::Builder builder2( "xtest.nc", fdp::IFile::Mode::WRITE );

    int status = FDP_FILE_STATUS_NOERR;

    status = builder2.writeDimension( "grp/dim1", dimdef1, dim1 );
    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );

    status = builder2.writeDimension( "grp/dim2", dimdef2, dim2 );
    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );

    status = builder2.writeArray( "grp/arr1", arrdef, data );
    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );

    // write already existing dim name
    status = builder2.writeDimension( "grp/dim1", dimdef1, dim1 );
    ASSERT_TRUE( FDP_FILE_STATUS_NOERR != status );

    status = builder2.writeArray( "grp/arr1", arrdef, data );
    ASSERT_TRUE( FDP_FILE_STATUS_NOERR != status );
    fdp::ArrayDefinition arrdef_bad;
    arrdef_bad.units="arrunits";
    arrdef_bad.description = "arrdescription";
    arrdef_bad.dataType = fdp::DataType::INT;

    arrdef_bad.dimension_names = { "baddim1", "baddim2" };

    status = builder2.writeArray( "grp/arr1", arrdef_bad, data );
    ASSERT_TRUE( FDP_FILE_STATUS_NOERR != status );

    fdp::ArrayDefinition arrdef_;

    status = builder2.readArray_metadata( "grp/arr1",  arrdef_ );

    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );
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

    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );
    ASSERT_EQ( 2, dimdef1_.size );

    ASSERT_EQ( "units1", dimdef1_.units );
    ASSERT_EQ( "description1", dimdef1_.description );

    int dim1_[2];

    status = builder2.readDim_data( "grp/dim1", dimdef1_, dim1_ );

    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );

    for( int j = 0; j < 2; ++j )
        ASSERT_EQ( dim1[j], dim1_[j] );

    fdp::DimensionDefinition dimdef2_;
    status = builder2.readDim_metadata( "grp/dim2", dimdef2_ );

    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );
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

    ASSERT_EQ( FDP_FILE_STATUS_NOERR, status );

    ASSERT_TRUE( NULL != att_ptr );

    ASSERT_EQ( fdp::DataType::INT, att_ptr->getAttType() );
    ASSERT_EQ( fdp::IAtt::ATTRIBUTE_VAR_TYPE, att_ptr->getClassType() );
    ASSERT_EQ( 3, att_ptr->getAttLength() );

    status = builder2.getAtt( "grp/dim3", "bad_att",  att_ptr );

    ASSERT_NE( FDP_FILE_STATUS_NOERR, status );

    ASSERT_TRUE( NULL == att_ptr );

#endif
} 

TEST_F(BuilderTest, TestGroupWithDim) 
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    fdp::IGroup::sptr grp_ptr = file_ptr->requireGroup( "aap/noot/mies" );

    ASSERT_TRUE( NULL != grp_ptr );

    if( grp_ptr )
    {
        fdp::IDimension::sptr dim_ptr = grp_ptr->addDim( "bla", 3 );
    }
}

TEST_F(BuilderTest, Test2GroupWithDim) 
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );


    fdp::IGroup::sptr grp1_ptr = file_ptr->requireGroup( "aap/noot/mies" );
    fdp::IGroup::sptr grp2_ptr = file_ptr->requireGroup( "aap/noot/mies/pis" );

    grp1_ptr->addDim( "miesdim", 3 );
    grp2_ptr->addDim( "pisdim", 2 );

    ASSERT_TRUE( NULL != grp1_ptr );
    ASSERT_TRUE( NULL != grp2_ptr );
}

#if 0
TEST_F(BuilderTest, TestPrepareCoordinateVar)
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );


    int dim_values1[] = {1,2,3};
    fdp::CoordinatVariableDefinition cvdef1; 
    cvdef1.name = "dim";
    cvdef1.values = &dim_values1;
    cvdef1.size = 3;
    cvdef1.datatype = fdp::DataType::INT;
    cvdef1.units = "";
    int status = file_ptr->prepare( cvdef1 );

    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );

    //	    new CoordinateVariableDefinition(
    //    "dim", new int[] {1, 2, 3}, "", "", "");
    fdp::CoordinatVariableDefinition cvdef2 ;
    //	    new CoordinateVariableDefinition(
    //			    new VariableName("dim", "one"), new int[] {1, 2, 3, 4}, "", "", "");
}

#endif
#if 0
TEST_F(BuilderTest, TestPrepareCoordinateVars)
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    int status = FDP_FILE_STATUS_NOERR;


    int dim_values1[] = {1,2,3};
    fdp::CoordinatVariableDefinition cvdef1; 
    cvdef1.name = "dim";
    cvdef1.values = &dim_values1;
    cvdef1.size = 3;
    cvdef1.datatype = fdp::DataType::INT;
    cvdef1.units = "";

    status = file_ptr->prepare( cvdef1 );

    int dim_values2[] = {1,2,3, 4};
    fdp::CoordinatVariableDefinition cvdef2; 
    cvdef2.name = "one/dim";
    cvdef2.values = &dim_values2;
    cvdef2.size = 4;
    cvdef2.datatype = fdp::DataType::INT;
    cvdef2.units = "";

    status = file_ptr->prepare( cvdef2 );

    fdp::DimensionalVariableDefinition dvd;
    dvd.name = "one/two/var";
    dvd.datatype = fdp::DataType::INT;
    dvd.dimensions = {"/dim"};




    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );

    status = file_ptr->prepare( dvd );

    return;
    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );

    //	    new CoordinateVariableDefinition(
    //    "dim", new int[] {1, 2, 3}, "", "", "");

}
#endif
#if 0
TEST_F(BuilderTest, TestLinkedFromMultiAttribute)
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    //std::string path = "test_linked_from_multi_attribute.nc";
    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    int status = FDP_FILE_STATUS_NOERR;

    fdp::LocalVAriableDefinition lvs[1];

    lvs[0].name = "column1";
    lvs[0].datatype = fdp::DataType::INT;

    fdp::TableDefinition td;
    td.name = "table";
    td.columns = { lvs[0] };


    fdp::ITable::sptr table_ptr;
    status = file_ptr->prepare( td, table_ptr );


    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );
    ASSERT_TRUE( table_ptr != NULL );

}
TEST_F(BuilderTest, TestPrepareDimensionalVar)
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    //std::string path = "test_prepare_dimensionalvar.nc";
    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    int status = FDP_FILE_STATUS_NOERR;

    int cvd_values[3] = {1,2,3};
    fdp::CoordinatVariableDefinition cvd;
    cvd.name = "dimension1";
    cvd.datatype = fdp::DataType::INT;
    cvd.values = cvd_values;

    status = file_ptr->prepare( cvd );

    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );


    int values[3] = { 1,2,3 };
    fdp::DimensionalVariableDefinition dvd;
    dvd.name = "dimensionalvariable";
    dvd.datatype = fdp::DataType::INT;
    dvd.dimensions = { "dimension1" };

    status = file_ptr->prepare( dvd );


    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );


}

TEST_F(BuilderTest, TestPrepareDimensionalVarRemoteDim)
{
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    //std::string path = "test_prepare_dimensionalvar_remote_dim.nc";
    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    int status = FDP_FILE_STATUS_NOERR;

    int cvd_values[3] = {1,2,3};
    fdp::CoordinatVariableDefinition cvd;
    cvd.name = "mygroup/dimension1";
    cvd.datatype = fdp::DataType::INT;
    cvd.values = cvd_values;

    status = file_ptr->prepare( cvd );

    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );

    fdp::DimensionalVariableDefinition dvd;
    dvd.name = "mygroup/subgroup/dimensionalvariable";
    dvd.datatype = fdp::DataType::INT;
    dvd.dimensions = { cvd.name };

    status = file_ptr->prepare( dvd );

    ASSERT_TRUE( FDP_FILE_ISNOERR( status ) );


}

#endif
TEST_F(BuilderTest, TestTable )
{

    // Prepare some data
    size_t size=3;

    int id_data[ size ] = {1,2,3};
    int data[ size ][2][2];
    double val_data[ size ] = {1.1, 2.2, 3.3};
    const char* names[3] = { "a", "b", "c" };

    for( int i = 0; i < size; ++i )
    {
        id_data[i] = i;
        val_data[i] = (i+1) * 1.1;

        int (*row)[2] = &(data[i][0]);
        for( int j = 0;  j < 2; ++j )
        {
            row[j][0] = i;
            row[j][1] = i;
        }
    }

    //craete the netcdf file
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    //std::string path = "test_prepare_dimensionalvar_remote_dim.nc";
    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    int status = FDP_FILE_STATUS_NOERR;

    fdp::ITable::sptr table_ptr;

    status = file_ptr->addTable( "myTestTable", size, table_ptr );

    ASSERT_EQ( 0, status );
    ASSERT_TRUE( NULL != table_ptr );

    table_ptr->long_name( "My little simple initial test table");

    fdp::IVar::sptr col_id_ptr = table_ptr->addCol( "id", fdp::DataType::INT );

    col_id_ptr->putVar( id_data );

    auto col_name_ptr = table_ptr->addCol( "itemName", fdp::DataType::STRING );
    col_name_ptr->long_name( "" );
    col_name_ptr->putVar( names );
    auto col_val_ptr = table_ptr->addCol( "value", fdp::DataType::DOUBLE );
    col_val_ptr->putVar( val_data );
    col_val_ptr->long_name( "value of the item" );


    fdp::IDimension::sptr dim_ptr1= table_ptr->addDim( "x", 2 );
    fdp::IDimension::sptr dim_ptr2= table_ptr->addDim( "y", 2 );

    fdp::IVar::VDIMS vdims = { dim_ptr1, dim_ptr2 };

    auto col_val_array_ptr = table_ptr->addCol( "array", vdims,  fdp::DataType::INT );
    col_val_array_ptr->putVar( data );

    //cant add table with existing table name
    fdp::ITable::sptr bad_table_ptr;
    status = file_ptr->addTable( "myTestTable", size, bad_table_ptr );

    ASSERT_NE( 0, status );
    ASSERT_TRUE( NULL == bad_table_ptr );

    auto tables = file_ptr->getTables();
    ASSERT_EQ( 1, tables.size() );

    ASSERT_TRUE( tables[0] == table_ptr );
}
#if 0
typedef struct compund_t
{
    int id;
    double value;
    int array_data[2][2];
    std::string name;
} compund_t;

int compound_init( compund_t* self, int id, double value, int array_data[][2], const std::string& name )
{
    self->id = id;
    self->value = value;
    self->name = name;
    memcpy( self->array_data, array_data, sizeof( int ) * 4 ) );
}

#endif
TEST_F(BuilderTest, TestTableUnlimited )
{

    // Prepare some data
    size_t size=3;

    int id_data[ size ] = {1,2,3};
    int data[ size ][2][2];
    double val_data[ size ] = {1.1, 2.2, 3.3};
    const char* names[3] = { "a", "b", "c" };

    for( int i = 0; i < size; ++i )
    {
        id_data[i] = i;
        val_data[i] = (i+1) * 1.1;

        int (*row)[2] = &(data[i][0]);
        for( int j = 0;  j < 2; ++j )
        {
            row[j][0] = i;
            row[j][1] = i;
        }
    }

    //craete the netcdf file
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    //std::string path = "test_prepare_dimensionalvar_remote_dim.nc";
    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    int status = FDP_FILE_STATUS_NOERR;

    fdp::ITable::sptr table_ptr;

    status = file_ptr->addTable( "myTestTable", FDP_UNLIMITED, table_ptr );

    ASSERT_EQ( 0, status );
    ASSERT_TRUE( NULL != table_ptr );

    table_ptr->long_name( "My little simple initial test table");

    fdp::IVar::sptr col_id_ptr = table_ptr->addCol( "id", fdp::DataType::INT );

    //col_id_ptr->putVar( id_data );

    auto col_name_ptr = table_ptr->addCol( "itemName", fdp::DataType::STRING );
    col_name_ptr->long_name( "" );
    //col_name_ptr->putVar( names );
    auto col_val_ptr = table_ptr->addCol( "value", fdp::DataType::DOUBLE );
    //col_val_ptr->putVar( val_data );
    col_val_ptr->long_name( "value of the item" );


    fdp::IDimension::sptr dim_ptr1= table_ptr->addDim( "x", 2 );
    fdp::IDimension::sptr dim_ptr2= table_ptr->addDim( "y", 2 );

    fdp::IVar::VDIMS vdims = { dim_ptr1, dim_ptr2 };

    auto col_val_array_ptr = table_ptr->addCol( "array", vdims,  fdp::DataType::INT );
    //col_val_array_ptr->putVar( data );
    //
    for( int i = 0; i  < size; ++i )
    {
        std::vector< size_t > index = { i };
        std::vector< size_t > index2 = { i, 0, 0 };
        const char* pszname = names[i];

        int (*row)[2] = &(data[i][0]);

        col_id_ptr->putVar( index, &i );

        col_val_ptr->putVar( index, &(val_data[i]) );
        col_name_ptr->putVar( index, &(names[i]) );

#if 0
        for( int j = 0; j < 2; j++)
        {
            for( int k = 0; k < 2; ++k )
            {
                std::vector< size_t > index2 = { i, j, k };
                col_val_array_ptr->putVar( index2, row );
            }
        }
#endif
        std::vector< size_t > count = { 1, 2, 2 };
        col_val_array_ptr->putVar( index2, count, row );
    }

    //cant add table with existing table name
    fdp::ITable::sptr bad_table_ptr;
    status = file_ptr->addTable( "myTestTable", size, bad_table_ptr );

    ASSERT_NE( 0, status );
    ASSERT_TRUE( NULL == bad_table_ptr );

    auto tables = file_ptr->getTables();
    ASSERT_EQ( 1, tables.size() );

    ASSERT_TRUE( tables[0] == table_ptr );
}

class table_t;
class row_t;
class cell_t;

class table_t
{
    public:
    table_t( std::vector< fdp::IVar::sptr >& vars );
    std::vector< fdp::IVar::sptr > vars;
    private:

};

class cell_t
{
    public:
        int id;
        row_t* row;    

        std::vector< size_t > shape();
    private:

};
class row_t
{
    public:
        row_t(){}
        row_t( table_t* t )
        {
            this->table = t;
        }
    int add( cell_t& cell );

    table_t* table;
    private:

    std::vector< cell_t > _cells;
};
table_t::table_t( std::vector< fdp::IVar::sptr >& vars )
    {
        this->vars = vars;

        row_t row;
        for( int i = 0; i < vars.size(); ++i )
        {
            cell_t c;
            row.add( c );
        }
    }



int row_t::add( cell_t& cell )
{
    cell.row = this;
    cell.id = this->_cells.size();
    this->_cells.push_back( cell );
    return 0;
}

std::vector< size_t > cell_t::shape()
{
    std::vector< size_t > dim_shapes;

    table_t* table = this->row->table; 

    fdp::IVar::sptr var_ptr = table->vars[this->id];

    fdp::IVar::VDIMS vdims = var_ptr->getDims();

    for( int i = 1; i < vdims.size(); ++i )
    {
        fdp::IDimension::sptr dim_ptr = vdims[i];
        dim_shapes.push_back( dim_ptr->getSize() );
    }

    return dim_shapes;
}



TEST_F(BuilderTest, TestRow )

{

    // Prepare some data
    size_t size=3;

    int id_data[ size ] = {1,2,3};
    int data[ size ][2][2];
    double val_data[ size ] = {1.1, 2.2, 3.3};
    const char* names[3] = { "a", "b", "c" };

    for( int i = 0; i < size; ++i )
    {
        id_data[i] = i;
        val_data[i] = (i+1) * 1.1;

        int (*row)[2] = &(data[i][0]);
        for( int j = 0;  j < 2; ++j )
        {
            row[j][0] = i;
            row[j][1] = i;
        }
    }

    //craete the netcdf file
    const testing::TestInfo* const test_info =
        testing::UnitTest::GetInstance()->current_test_info();

    std::string path = std::string( test_info->name() ) + ".nc";

    //std::string path = "test_prepare_dimensionalvar_remote_dim.nc";
    std::remove( path.c_str() );

    fdp::IFile::sptr file_ptr = fdp::FileFactory::create( path, fdp::IFile::Mode::WRITE );

    ASSERT_TRUE( NULL != file_ptr );

    int status = FDP_FILE_STATUS_NOERR;

    fdp::ITable::sptr table_ptr;

    status = file_ptr->addTable( "myTestTable", FDP_UNLIMITED, table_ptr );

    ASSERT_EQ( 0, status );
    ASSERT_TRUE( NULL != table_ptr );

    table_ptr->long_name( "My little simple initial test table");

    fdp::IVar::sptr col_id_ptr = table_ptr->addCol( "id", fdp::DataType::INT );

    //col_id_ptr->putVar( id_data );

    auto col_name_ptr = table_ptr->addCol( "itemName", fdp::DataType::STRING );
    col_name_ptr->long_name( "" );
    //col_name_ptr->putVar( names );
    auto col_val_ptr = table_ptr->addCol( "value", fdp::DataType::DOUBLE );
    //col_val_ptr->putVar( val_data );
    col_val_ptr->long_name( "value of the item" );


    fdp::IDimension::sptr dim_ptr1= table_ptr->addDim( "x", 2 );
    fdp::IDimension::sptr dim_ptr2= table_ptr->addDim( "y", 2 );

    fdp::IVar::VDIMS vdims = { dim_ptr1, dim_ptr2 };

    auto col_val_array_ptr = table_ptr->addCol( "array", vdims,  fdp::DataType::INT );
    //col_val_array_ptr->putVar( data );
    //
    for( int i = 0; i  < size; ++i )
    {
        void* row[3] = { &i, &names[i], &data[i][0] };

#if 1

#else
        std::vector< size_t > index = { i };
        std::vector< size_t > index2 = { i, 0, 0 };
        const char* pszname = names[i];

        int (*row)[2] = &(data[i][0]);

        col_id_ptr->putVar( index, &i );

        col_val_ptr->putVar( index, &(val_data[i]) );
        col_name_ptr->putVar( index, &(names[i]) );

#if 0
        for( int j = 0; j < 2; j++)
        {
            for( int k = 0; k < 2; ++k )
            {
                std::vector< size_t > index2 = { i, j, k };
                col_val_array_ptr->putVar( index2, row );
            }
        }
#endif
        std::vector< size_t > count = { 1, 2, 2 };
        col_val_array_ptr->putVar( index2, count, row );
#endif
    }

    //cant add table with existing table name
    fdp::ITable::sptr bad_table_ptr;
    status = file_ptr->addTable( "myTestTable", size, bad_table_ptr );

    ASSERT_NE( 0, status );
    ASSERT_TRUE( NULL == bad_table_ptr );

    auto tables = file_ptr->getTables();
    ASSERT_EQ( 1, tables.size() );

    ASSERT_TRUE( tables[0] == table_ptr );

}
