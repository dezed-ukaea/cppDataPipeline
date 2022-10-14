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
//
#define FDP_FILE_STATUS_NOERR 0
#define FDP_FILE_STATUS_ERR 1

#define FDP_FILE_ISNOERR(x) (0 == x)
#define FDP_FILE_ISERR(x) (!FDP_FILE_ISNOERR(x))

namespace FairDataPipeline
{
    std::string str_strip_right( std::string str, const std::string& chars );

    std::string str_strip_left( std::string str, const std::string& chars );

    std::string str_strip( std::string str, const std::string& chars );

    typedef std::pair< std::string, std::string > PARENT_ITEM_TYPE;

    PARENT_ITEM_TYPE split_item_name( std::string str );



    enum DataType
    {
        UNKNOWN,
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

	struct netCDFComponentDefinition
	{
        netCDFComponentDefinition()
        {
        }

		std::string description;
		std::string long_name;

		//std::map< std::string, std::vector<std::string> > optional_attribs;
	};

	struct VariableDefinition : netCDFComponentDefinition
	{
        VariableDefinition() : netCDFComponentDefinition(), _sz_missing(0)
        {
            datatype = DataType::UNKNOWN;
        }

		DataType datatype;
		std::string units;
        std::shared_ptr< void > _missing_ptr;
        size_t _sz_missing;

        template< typename T >
        int setFillValue( const T& pval )
        {
            int status = 0;

            std::shared_ptr<T> p( new T() );

            _missing_ptr = p;
            _sz_missing = sizeof( T );

            return status;
        }
	};

	struct LocalVAriableDefinition : VariableDefinition
	{
        LocalVAriableDefinition() : VariableDefinition()
        {
        }

        std::string name;
	};

	struct CoordinatVariableDefinition : VariableDefinition
	{
        CoordinatVariableDefinition() : VariableDefinition()
        {
            size=0;
            values = NULL;
        }

		int size;
        static int UNLIMITED;
		void* values;
		std::string name;//variable name in jave splits the group and patgh

        bool isUnlimited() const;
	};

    struct DimensionalVariableDefinition : VariableDefinition
    {
        DimensionalVariableDefinition() : VariableDefinition()
        {
        }

        std::vector< std::string > dimensions;
        std::string name;

        const std::vector< std::string >& getDimensions() const;
    };

    struct TableDefinition : netCDFComponentDefinition
    {
        TableDefinition() : netCDFComponentDefinition()
        {
            size = 0;
        }

        const std::vector< LocalVAriableDefinition >& getColumns() const
        {
            return this->columns;
        }


        std::vector< LocalVAriableDefinition > columns;
        std::string name;
        size_t size;

        size_t getSize() const;
        bool isUnlimited() const;
        const std::string& getName() const { return name;}
        std::string getVariableName( int i );
    };




    struct IGroup;
    typedef std::shared_ptr< IGroup > IGroupPtr;

    struct IDimension
    {
        typedef std::shared_ptr< IDimension > sptr;

        virtual size_t getSize() = 0;
        virtual bool isUnlimited() = 0;
        virtual std::string getName() = 0;
        virtual IGroupPtr getParentGroup() = 0;
    };

    struct IAtt
    {
        enum ATTRIBUTE_TYPE{ ATTRIBUTE_GROUP_TYPE, ATTRIBUTE_VAR_TYPE };

        typedef std::shared_ptr< IAtt > sptr;

        virtual DataType getAttType() = 0;
        virtual std::string getName() = 0;

        virtual int getClassType() = 0;
        virtual size_t getAttLength() = 0;

        virtual int getValues( int* values ) = 0;
        virtual int getValues( short* values ) = 0;
        virtual int getValues( long* values ) = 0;
        virtual int getValues( long long* values ) = 0;

        virtual int getValues( unsigned short* values ) = 0;
        virtual int getValues( unsigned int* values ) = 0;
        virtual int getValues( unsigned long* values ) = 0;

        virtual int getValues( float* values ) = 0;
        virtual int getValues( std::string& values ) = 0;
        virtual int getValues( unsigned long long* values ) = 0;

    };

    struct IGroupAtt : public IAtt
    {
        typedef std::shared_ptr< IGroupAtt > sptr;


    };

    struct IVarAtt : public IAtt
    {
        typedef std::shared_ptr< IVarAtt > sptr;
    };

    struct IVar
    {
        typedef std::vector< IDimension::sptr > VDIMS;

        typedef std::shared_ptr< IVar > sptr;

        typedef std::map< std::string, IVarAtt::sptr > NAME_VARATT_MAP;

        virtual IGroupPtr parent() = 0;

        virtual void putVar( const void* vals ) = 0;
        virtual void getVar( void* vals ) = 0;

        virtual std::vector< IDimension::sptr > getDims() = 0;

        virtual DataType getType() = 0;

        virtual IVarAtt::sptr putAtt( const std::string& key, const std::string& value ) = 0;

        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const short* values ) = 0;
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const int* values ) = 0;
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const long* values ) = 0;
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const long long* values ) = 0;

        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned short* values ) = 0;
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned int* values ) = 0;
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long* values ) = 0;
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long long* values ) = 0;

        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const float* values ) = 0;
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const double* values ) = 0;
        
        virtual IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const char** values ) = 0;
       virtual  IVarAtt::sptr putAtt( const std::string& key
                , DataType datatype, size_t nvals, void* pv ) = 0;


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

        //virtual IGroup::sptr addGroup( const std::string& s ) = 0;

        virtual IGroup::sptr getGroup( const std::string& name ) = 0;

        virtual IDimension::sptr addDim( const std::string& name, size_t sz ) = 0;
        virtual IDimension::sptr addUnlimitedDim( const std::string& name ) = 0;

        virtual IDimension::sptr getDim( const std::string& name ) = 0;

        virtual IVar::sptr addVar( const std::string& name
                , DataType dtype
                , std::vector< IDimension::sptr >& vdims ) = 0;

        virtual IVar::sptr getVar( const std::string& name ) = 0;
        virtual std::vector< std::string > getVars() = 0;

        virtual std::vector< std::string > getGroups() = 0;


        virtual int getGroupCount() = 0;

        virtual std::string getName() = 0;

        virtual IGroupAtt::sptr putAtt( const std::string& key, const std::string& value ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, int value ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, float value ) = 0;

        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const short* values ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const int* values ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const long* values ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const long long* values ) = 0;

        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned short* values ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned int* values ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long* values ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long long* values ) = 0;

        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const float* values ) = 0;
        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const double* values ) = 0;

        virtual IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const char** values ) = 0;


        virtual IGroupAtt::sptr getAtt( const std::string& key ) = 0;
        virtual std::vector< std::string > getAtts() = 0;




	virtual int prepare( const CoordinatVariableDefinition& cvd ) = 0;

    virtual int prepare( const DimensionalVariableDefinition& dvd ) = 0;

    };

    struct IFile : public IGroup
    {
        enum Mode{ READ, WRITE};
    };

    struct DimensionDefinition
    {
        size_t size;
        std::string units;
        std::string description;
        enum DataType dataType;
    };

    struct ArrayDefinition
    {
        std::string units;
        enum DataType dataType;
        std::string description;
        std::vector< std::string > dimension_names;
    };


    class FileFactory
    {
        public:
            typedef IFile::Mode Mode;

            FileFactory() = delete;
            FileFactory (const FileFactory& rhs ) = delete;

            static IFile::sptr create( const std::string& path, Mode mode );
        private:

    };

    class Builder
    {
        public:
            Builder() = delete;

            Builder( const Builder& rhs ) = delete;

            Builder( const std::string& path, IFile::Mode mode );

            //int prepare( const CoordinatVariableDefinition& cvd );

            int writeArray( const std::string& name
                    , const ArrayDefinition& arraydef, const void* data );

            int writeDimension( const std::string& name
                    , const DimensionDefinition& dimdef, const void* data );

            int readDim_metadata( const std::string& name, DimensionDefinition& dimdef );

            int readDim_data( const std::string& name
                    , const DimensionDefinition& dimdef, void* data );

            int readArray_metadata( const std::string& name,  ArrayDefinition& arraydef );

            int readArray_data( const std::string& name
                    , const ArrayDefinition& arraydef, void *data );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const short* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const int* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const long* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const long long* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const unsigned short* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const unsigned int* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const unsigned long* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const unsigned long long* values );
            
            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const float* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const double* values );

            int putAtt( const std::string& name
                    , const std::string& key, size_t nvals,  const char** values );


            int getAtt( const std::string& name, const std::string& key, IAtt::sptr& att_ptr );
        private:
            IFile::sptr _file;
    };
}
#endif
