#ifndef __FDPAPI_NETCDF_H__
#define __FDPAPI_NETCDF_H__

//#include<stdio.h>
#include<memory>
#include<map>
#include<vector>
#include<string>
#include<type_traits>
//#include<netcdf>

//#include "fdp/fdparrayref.hxx"

namespace FairDataPipeline
{
    void split_str( const std::string& s, const char delim, std::vector< std::string >& splits );

    enum DataType
    {
        BYTE,
        CHAR,
        SHORT,
        INT,
        FLOAT,
        DOUBLE,
        UBYTE,
        USHORT,
        UINT,
        INT64,
        UINT64,
        STRING,
        //VLEN,
        //OPAQUE,
        //ENUM,
        //COMPOUND
    };

    struct IGroup;
    typedef std::shared_ptr< IGroup > IGroupPtr;

    struct IDimension
    {
        typedef std::shared_ptr< IDimension > sptr;

        virtual size_t getSize() = 0;
        virtual std::string getName() = 0;
        //virtual void rename(const std::string& new_name) = 0;
        virtual IGroupPtr getParentGroup() = 0;
    };
    struct IGroupAtt
    {
        typedef std::shared_ptr< IGroupAtt > sptr;

        virtual int getValues( int* values ) = 0;
        virtual int getValues( float* values ) = 0;
        virtual int getValues( std::string& values ) = 0;
    };



    struct IVarAtt
    {
        typedef std::shared_ptr< IVarAtt > sptr;

	virtual int getValues( int* values ) = 0;
        virtual int getValues( float* values ) = 0;
        virtual int getValues( std::string& values ) = 0;
    };

    struct IVar
    {
        typedef std::vector< IDimension::sptr > VDIMS;

        typedef std::shared_ptr< IVar > sptr;

        typedef std::map< std::string, IVarAtt::sptr > NAME_VARATT_MAP;

        virtual IGroupPtr parent() = 0;

        virtual void putVar( const void* vals ) = 0;
        virtual void getVar( void* vals ) = 0;

        virtual std::vector< std::string > getDims() = 0;

        virtual DataType getType() = 0;

        virtual IVarAtt::sptr putAtt( const std::string& key, const std::string& value ) = 0;
        virtual IVarAtt::sptr getAtt( const std::string& key ) = 0;
    };

    struct IGroup
    {
        typedef std::shared_ptr< IGroup > sptr;

        typedef std::map< std::string, IGroup::sptr > NAME_GROUP_MAP;
        typedef std::map< std::string, IDimension::sptr > NAME_DIM_MAP;
        typedef std::map< std::string, IVar::sptr > NAME_VAR_MAP;
        typedef std::map< std::string, IGroupAtt::sptr > NAME_ATT_MAP;

        virtual IGroup::sptr parent() = 0;

        virtual IGroup::sptr requireGroup( const std::string& name ) = 0;

        virtual IGroup::sptr addGroup( const std::string& s ) = 0;

        virtual IGroup::sptr getGroup( const std::string& name ) = 0;

        virtual IDimension::sptr addDim( const std::string& name, size_t sz ) = 0;
        
        virtual IDimension::sptr getDim( const std::string& name ) = 0;

        virtual IVar::sptr addVar( const std::string& name
                , DataType dtype
                , std::vector< IDimension::sptr >& vdims ) = 0;

        virtual IVar::sptr getVar( const std::string& name ) = 0;


        virtual int getGroupCount() = 0;

        virtual std::string name() = 0;

        virtual IGroupAtt::sptr putAtt( const std::string& key, const std::string& value ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, int value ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, float value ) = 0;

        virtual IGroupAtt::sptr getAtt( const std::string& key ) = 0;

    };

    struct IBuilder : public IGroup
    {
        enum Mode{ READ, WRITE};
    };

    struct DimensionDefinition
    {
        size_t size;
        std::string name;
        std::string units;
        std::string description;
        enum DataType dataType;
    };

    struct ArrayDefinition
    {
        std::string name;
        std::string units;
        enum DataType dataType;
        std::string description;
        std::vector< std::string > dimension_names;
        std::vector< size_t > shape;
    };


    class BuilderFactory
    {
        public:
            typedef IBuilder::Mode Mode;

            BuilderFactory() = delete;
            BuilderFactory (const BuilderFactory& rhs ) = delete;

            static IBuilder::sptr create( const std::string& path, Mode mode );
        private:

    };

    class Builder
    {
        public:
            Builder() = delete;
            Builder( const Builder& rhs ) = delete;

            Builder( const std::string& path, IBuilder::Mode mode );

            int writeArray( /*const std::string& group_name
                    , */const ArrayDefinition& arraydef, const void* data );

            int writeDimension( /*const std::string& group_name,*/ const DimensionDefinition& dimdef, const void* data );

            int readDim_metadata( /*const std::string& grp_name
                    , */const std::string dim_name
                    , DimensionDefinition& dimdef );

            int readDim_data( /*const std::string& grp_name
                    ,*/ const DimensionDefinition& dimdef, void* data );


            int readArray_metadata( /*const std::string& grp_name, */std::string array_name
                    ,  ArrayDefinition& arraydef );

            int readArray_data( /*const std::string& grp_name,*/  const ArrayDefinition& arraydef, void *data );
        private:
            IBuilder::sptr _builder;
    };
#if 0
    template< size_t...N >
        struct DDim
        {
            DDim()
            {
                int n = sizeof...(N);
                //printf( "DDIM size:%d\n", n  );

                std::array< size_t, sizeof...(N) > a = {N...};

                //printf("DDIM:");
                //for( int i = 0;i < n; ++i )
                //{
                //printf("%d,", a[i]);
                //}
                //printf( "\n");
            }
            size_t rank(){ return sizeof...(N);}
            std::vector<size_t> shape()
            {
                size_t n = this->rank();

                std::vector<size_t> v;
                v.reserve(n);

                //int a[ sizeof...(N) ]= {N...};

                std::array< size_t, sizeof...(N) > a = {N...};
                for( int i = 0;i < n; ++i )
                {
                    // printf("%d,", a[i]);
                    v.push_back( a[i] );
                }
                //printf( "\n");
                return v;

            }

            //std::array< long, N > Dim;
        };
#endif


}
#endif
