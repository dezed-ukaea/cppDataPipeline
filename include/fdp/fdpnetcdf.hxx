#ifndef __FDPAPI_NETCDF_H__
#define __FDPAPI_NETCDF_H__

#include<memory>
#include<map>
#include<vector>
#include<string>
#include<type_traits>

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
                *p = pval;

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

#define FDP_UNLIMITED 0
    struct CoordinatVariableDefinition : VariableDefinition
    {
        CoordinatVariableDefinition() : VariableDefinition()
        {
            size=0;
            values = NULL;
        }

        int size;
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

    struct IAttComposite
    {
        typedef std::shared_ptr< IAttComposite > sptr;

        virtual IAtt::sptr putAttShort( const std::string& key, short value ) = 0;
        virtual IAtt::sptr putAttInt( const std::string& key, int value ) = 0;
        virtual IAtt::sptr putAttLong( const std::string& key, long value ) = 0;
        virtual IAtt::sptr putAttLongLong( const std::string& key, long long value ) = 0;

        virtual IAtt::sptr putAttUShort( const std::string& key, unsigned short value ) = 0;
        virtual IAtt::sptr putAttUInt( const std::string& key, unsigned int value ) = 0;
        //virtual IAtt::sptr putAttULong( const std::string& key, unsigned long value ) = 0;

        virtual IAtt::sptr putAttString( const std::string& key, const std::string& value ) = 0;
        virtual IAtt::sptr putAttFloat( const std::string& key, float value ) = 0;
        virtual IAtt::sptr putAttDouble( const std::string& key, double value ) = 0;


        virtual IAtt::sptr putAttShorts( const std::string& key, size_t nvals, const short* values ) = 0;
        virtual IAtt::sptr putAttInts( const std::string& key, int nvals, const int* values ) = 0;
        virtual IAtt::sptr putAttLongs( const std::string& key, size_t nvals, const long* values ) = 0;
        virtual IAtt::sptr putAttLongLongs( const std::string& key, size_t nvals, const long long* values ) = 0;

        virtual IAtt::sptr putAttUShorts( const std::string& key, size_t nvals, const unsigned short* values ) = 0;
        virtual IAtt::sptr putAttUInts( const std::string& key, size_t nvals, const unsigned int* values ) = 0;
        virtual IAtt::sptr putAttULongs( const std::string& key, size_t nvals, const unsigned long* values ) = 0;
        virtual IAtt::sptr putAttULongLongs( const std::string& key, size_t nvals, const unsigned long long* values ) = 0;

        virtual IAtt::sptr putAttFloats( const std::string& key, size_t nvals, const float* values ) = 0;
        virtual IAtt::sptr putAttDoubles( const std::string& key, size_t nvals, const double* values ) = 0;

        virtual IAtt::sptr putAttStrs( const std::string& key, size_t nvals, const char** values ) = 0;

        virtual  IAtt::sptr putAttVal( const std::string& key, DataType datatype, const void* pv ) = 0;
        virtual  IAtt::sptr putAttVals( const std::string& key, DataType datatype, size_t nvals, const void* pv ) = 0;


        virtual IAtt::sptr getAtt( const std::string& key ) = 0;
	virtual size_t getAttCount() = 0;
//	virtual std::vector< IAtt::sptr > getAtts() = 0;

    };


    struct IVar : public IAttComposite
    {
        typedef std::vector< IDimension::sptr > VDIMS;

        typedef std::shared_ptr< IVar > sptr;


        virtual IGroupPtr parent() = 0;

        virtual void putVar( const void* vals ) = 0;
        virtual void getVar( void* vals ) = 0;

        virtual std::vector< IDimension::sptr > getDims() = 0;

        virtual DataType getType() = 0;
    };

    struct ITable;
    typedef std::shared_ptr< ITable > ITableSptr;


    struct IGroup : public IAttComposite
    {
        typedef std::shared_ptr< IGroup > sptr;

        virtual IGroup::sptr parent() = 0;

        virtual IGroup::sptr requireGroup( const std::string& name ) = 0;

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

        virtual int prepare( const CoordinatVariableDefinition& cvd ) = 0;

        virtual int prepare( const DimensionalVariableDefinition& dvd ) = 0;

        virtual int prepare( const TableDefinition& td, ITableSptr& table_ptr ) = 0; 
    };

    struct ICol : public IVar
    {
	    typedef std::shared_ptr< ICol > sptr;

	    virtual const std::string name() = 0;
	    virtual DataType type() = 0;
    };

    struct ITable //: public IGroup
    {
	    typedef std::shared_ptr< ITable > sptr;
	    virtual ICol::sptr addCol( const std::string& name
			    , const std::string& long_name
			    , const std::string& units
			    , const std::string& description
			    , DataType dtype ) = 0;
	    virtual ICol::sptr getCol( const std::string& name ) = 0;

	    virtual IGroup::sptr getParentGroup() = 0;
    };

    struct IFile : public IGroup
    {
        enum Mode{ READ, WRITE };
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
}
#endif
