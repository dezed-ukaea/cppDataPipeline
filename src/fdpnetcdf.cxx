#include "fdp/fdpnetcdf.hxx"
#include "fdp/utilities/logging.hxx"  

//#include<sstream>

#include<netcdf>

typedef netCDF::exceptions::NcException NcException;
typedef netCDF::NcGroup NcGroup;
typedef netCDF::NcFile NcFile;

typedef std::shared_ptr< NcGroup > NcGroupPtr;
typedef std::shared_ptr< NcFile > NcFilePtr;

#define ATTRIB_KEY_DESC "description"
#define ATTRIB_KEY_LNAME "long_name"
#define ATTRIB_KEY_FILLVALUE "_FillValue"
#define ATTRIB_KEY_UNITS "units"
#define ATTRIB_KEY_GROUP_TYPE "group_type"


template< typename T >
struct NcTraits;

template<>
struct NcTraits< short >
{
    typedef netCDF::NcShort MyNcType;
};

template<>
struct NcTraits< int >
{
    typedef netCDF::NcInt MyNcType;
};

template<>
struct NcTraits< unsigned short >
{
    typedef netCDF::NcUshort MyNcType;
};

template<>
struct NcTraits< unsigned int >
{
    typedef netCDF::NcUint MyNcType;
};


template<>
struct NcTraits< float >
{
    typedef netCDF::NcFloat MyNcType;
};

template<>
struct NcTraits< double >
{
    typedef netCDF::NcDouble MyNcType;
};

template<>
struct NcTraits< long >
{
    typedef netCDF::NcInt64 MyNcType;
};
template<>
struct NcTraits< long long >
{
    typedef netCDF::NcInt64 MyNcType;
};

template<>
struct NcTraits< unsigned long >
{
    typedef netCDF::NcUint64 MyNcType;
};
template<>
struct NcTraits< unsigned long long >
{
    typedef netCDF::NcUint64 MyNcType;
};




namespace FairDataPipeline
{
    template< int > struct  DataTypeTraits;

    template<>
        struct DataTypeTraits< DataType::CHAR >
        {
            typedef char native_type;
        };

    template<>
        struct DataTypeTraits< DataType::SHORT >
        {
            typedef short native_type;
        };

    template<>
        struct DataTypeTraits< DataType::INT >
        {
            typedef int native_type;
        };

    template<>
        struct DataTypeTraits< DataType::INT64 >
        {
            typedef long native_type;
        };

    //template<>
    //struct DataTypeTraits< DataType::UCHAR >
    //{
    //    typedef unsigned char native_type;
    //};

    template<>
        struct DataTypeTraits< DataType::USHORT >
        {
            typedef unsigned short native_type;
        };

    template<>
        struct DataTypeTraits< DataType::UINT >
        {
            typedef unsigned int native_type;
        };

    template<>
        struct DataTypeTraits< DataType::UINT64 >
        {
            typedef long native_type;
        };




    class GroupImpl;
    typedef std::shared_ptr< GroupImpl > GroupImplSptr;

    static netCDF::NcType::ncType DataType2NcType( DataType dtype )
    {
        netCDF::NcType::ncType nctype;

        switch( dtype )
        {
            case BYTE:
                nctype = netCDF::NcType::nc_BYTE;
                break;
            case CHAR:
                nctype = netCDF::NcType::nc_CHAR;
                break;
            case SHORT:
                nctype = netCDF::NcType::nc_SHORT;
                break;
            case INT:
                nctype = netCDF::NcType::nc_INT;
                break;
            case INT64:
                nctype = netCDF::NcType::nc_INT64;
                break;

            case USHORT:
                nctype  =netCDF::NcType::nc_USHORT;
                break;
            case UINT:
                nctype = netCDF::NcType::nc_UINT;
                break;
            case UINT64:
                nctype = netCDF::NcType::nc_UINT64;
                break;
            case UBYTE:
                nctype = netCDF::NcType::nc_UBYTE;
                break;


            case FLOAT:
                nctype = netCDF::NcType::nc_FLOAT;
                break;
            case DOUBLE:
                nctype = netCDF::NcType::nc_DOUBLE;
                break;

            case STRING:
                nctype = netCDF::NcType::nc_STRING;
                break;

            default:
                break;
        }
        return nctype;
    }


    static DataType NcType2DataType( netCDF::NcType::ncType nctype )
    {
        DataType dtype;

        switch( nctype )
        {
            case netCDF::NcType::nc_BYTE:
                dtype = BYTE;
                break;
            case netCDF::NcType::nc_CHAR:
                dtype = CHAR;
                break;
            case netCDF::NcType::nc_SHORT:
                dtype = SHORT;
                break;
            case netCDF::NcType::nc_INT:
                dtype = INT;
                break;
            case netCDF::NcType::nc_INT64:
                dtype = INT64;
                break;

            case netCDF::NcType::nc_USHORT:
                dtype = USHORT;
                break;
            case netCDF::NcType::nc_UINT:
                dtype = UINT;
                break;
            case netCDF::NcType::nc_UINT64:
                dtype = UINT64;
                break;
            case netCDF::NcType::nc_UBYTE:
                dtype = UBYTE;
                break;


            case netCDF::NcType::nc_FLOAT:
                dtype = FLOAT;
                break;
            case netCDF::NcType::nc_DOUBLE:
                dtype = DOUBLE;
                break;
            default:
                break;

        }
        return dtype;
    }



    void split_str( const std::string& s, const char delim, std::vector< std::string >& splits )
    {
        std::stringstream ss( s );

        std::string s_;
        while( std::getline( ss, s_, delim ))
        {
            splits.push_back( s_ );
        }
    }

    void split_name( const std::string& name, std::vector< std::string >& splits )
    {
        if( '/' == name[0] )
            splits.push_back( "/" );

        split_str( name, '/', splits );
    }

    class VarImpl : public IVar
    {
        public:
            typedef std::shared_ptr< VarImpl > sptr;

        typedef std::map< std::string, IVarAtt::sptr > NAME_VARATT_MAP;

            static VarImpl::sptr create( IGroup::sptr parent
                    , const std::string& name
                    , const netCDF::NcVar& nc_var );


            void putVar( const void* vals );

            void putVar( const std::vector< size_t >& index, int value ) const;
            void putVar( const std::vector< size_t >& index
                    , const std::vector< size_t >& countp
                    , int* values ) const;

            void getVar( void* vals );

            DataType getType();

            std::vector< IDimension::sptr > getDims();

            IGroup::sptr parent();


            IAtt::sptr putAttShort( const std::string& key, short value );
            IAtt::sptr putAttInt( const std::string& key, int value );
            IAtt::sptr putAttLong( const std::string& key, long value );
            IAtt::sptr putAttLongLong( const std::string& key, long long value );

            IAtt::sptr putAttUShort( const std::string& key, unsigned short value );
            IAtt::sptr putAttUInt( const std::string& key, unsigned int value );
            
            IAtt::sptr putAttString( const std::string& key, const std::string& value);
            IAtt::sptr putAttFloat( const std::string& key, float value );
            IAtt::sptr putAttDouble( const std::string& key, double value );

            IAtt::sptr putAttShorts( const std::string& key, size_t nvals, const short* values );
            IAtt::sptr putAttInts( const std::string& key, int nvals, const int* values );
            IAtt::sptr putAttLongs( const std::string& key, size_t nvals, const long* values );
            IAtt::sptr putAttLongLongs( const std::string& key, size_t nvals, const long long* values );

            IAtt::sptr putAttUShorts( const std::string& key, size_t nvals, const unsigned short* values );
            IAtt::sptr putAttUInts( const std::string& key, size_t nvals, const unsigned int* values );
            IAtt::sptr putAttULongs( const std::string& key, size_t nvals, const unsigned long* values );
            IAtt::sptr putAttULongLongs( const std::string& key, size_t nvals, const unsigned long long* values );

            IAtt::sptr putAttFloats( const std::string& key, size_t nvals, const float* values );
            IAtt::sptr putAttDoubles( const std::string& key, size_t nvals, const double* values );

            IAtt::sptr putAttStrs( const std::string& key, size_t nvals, const char** values );

    
	    IAtt::sptr putAttVal( const std::string& key, DataType datatype, const void* pv );
            IAtt::sptr putAttVals( const std::string& key, DataType datatype, size_t nvals, const void* pv );

	    size_t getAttCount(){ return _name_varatt_map.size();}
	    std::vector< IAtt::sptr > getAtts();


            IAtt::sptr getAtt( const std::string& key );

        private:

            VarImpl( IGroup::sptr parent, const std::string& name, const netCDF::NcVar& nc_var ) ;

            template< typename T >
                IVarAtt::sptr putAttImpls( const std::string& key
                        , size_t nvals, const T* values );

            template< typename T >
                IVarAtt::sptr putAttImpl( const std::string& key, const T& value );

            template< typename T >  
                void putVarImpl( const std::vector< size_t >& index, T* values ) const;

            template< typename T >  
                void putVarImpl( const std::vector< size_t >& index
                        , const std::vector< size_t >& countp
                        , T* values ) const;

            std::weak_ptr< IGroup > _parent;

            netCDF::NcVar _nc_var;

            NAME_VARATT_MAP _name_varatt_map;
    };

    std::vector< IAtt::sptr > VarImpl::getAtts()
    {
	    std::vector< IAtt::sptr > v_atts;

	    auto nc_name_att_map = _nc_var.getAtts();
	    auto nc_its = nc_name_att_map.begin();
	    auto nc_ite = nc_name_att_map.end();

	    for(auto it = nc_its; it != nc_ite; it++)
	    {
		    const std::string& name = it->first;
		    IAtt::sptr att_ptr = this->getAtt( name );
		    if( att_ptr )
			    v_atts.push_back( att_ptr );
	    }

	    return v_atts;
    }

    IAtt::sptr VarImpl::putAttVal( const std::string& key, DataType datatype, const void* pv )
    {
        IVarAtt::sptr att_ptr;
        switch( datatype )
        {
            case DataType::SHORT:
                this->putAttShort( key, *(const short*)(pv) );
                break;
            case DataType::INT:
                this->putAttInt( key, *(const int*)(pv) );
                break;
            case DataType::INT64:
                this->putAttLong( key, *(const long*)(pv) );
                break;
            case DataType::USHORT:
                this->putAttUShort( key, *(const unsigned short*)(pv) );
                break;
            case DataType::UINT:
                this->putAttUInt( key, *(const unsigned int*)(pv) );
                break;
	    //case DataType::UINT64:
                //this->putAttULong( key, *(const unsigned long*)(pv) );
                //break;
            case DataType::FLOAT:
                this->putAttFloat( key, *(const float*)(pv) );
                break;
            case DataType::DOUBLE:
                this->putAttDouble( key, *(const double*)(pv) );
                break;

            default:
                logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": Unhandled datatype";
                break;
        }
        return att_ptr;
    }

    IAtt::sptr VarImpl::putAttVals( const std::string& key, DataType datatype, size_t nvals, const void* pv )
    {
        IVarAtt::sptr att_ptr;
        switch( datatype )
        {
            case DataType::SHORT:
                this->putAttShorts( key, nvals, (const short*)(pv) );
                break;
            case DataType::INT:
                this->putAttInts( key, nvals, (const int*)(pv) );
                break;
            case DataType::INT64:
                this->putAttLongs( key, nvals, (const long*)(pv) );
                break;
            case DataType::USHORT:
                this->putAttUShorts( key, nvals, (const unsigned short*)(pv) );
                break;
            case DataType::UINT:
                this->putAttUInts( key, nvals, (const unsigned int*)(pv) );
                break;
            case DataType::UINT64:
                this->putAttULongs( key, nvals, (const unsigned long*)(pv) );
                break;
            case DataType::FLOAT:
                this->putAttFloats( key, nvals, (const float*)(pv) );
                break;
            case DataType::DOUBLE:
                this->putAttDoubles( key, nvals, (const double*)(pv) );
                break;

            default:
                logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": Unhandled datatype";
                break;
        }
        return att_ptr;
    }

    IGroup::sptr VarImpl::parent( )
    {
        return _parent.lock();
    }

    template< typename T >  
        void VarImpl::putVarImpl( const std::vector< size_t >& index, T* values ) const
        {
            try
            {
                _nc_var.putVar( index, values );
            }
            catch (NcException& ex )
            {
                logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            }
        }

    template< typename T >  
        void VarImpl::putVarImpl( const std::vector< size_t >& index
                , const std::vector< size_t >& countp
                , T* values ) const
        {
            try
            {
                _nc_var.putVar( index, countp, values );
            }
            catch (NcException& ex )
            {
                logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            }
        }


    void VarImpl::putVar( const std::vector< size_t >& index, int value ) const
    {
        try
        {
            _nc_var.putVar( index, value );
        }
        catch( NcException& ex )
        {
            logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }
    }

    void VarImpl::putVar( const std::vector< size_t >& index
            , const std::vector< size_t >& countp
            , int* values ) const
    {
        putVarImpl( index, countp, values );
    }


    class GroupAtt : public IGroupAtt
    {
        public:
            typedef std::shared_ptr< GroupAtt > sptr;

            GroupAtt() = delete;
            static GroupAtt::sptr create( GroupImplSptr grp_ptr, const netCDF::NcGroupAtt& ncatt );

            DataType getAttType();
            int getClassType(){ return ATTRIBUTE_GROUP_TYPE; }
            size_t getAttLength() { return _ncatt.getAttLength();}

            int getValues( std::string& values );

            int getValues( short* values );
            int getValues( int* values );
            int getValues( long* values );
            int getValues( long long* values );
            int getValues( unsigned short* values );
            int getValues( unsigned int* values );
            int getValues( unsigned long* values );
            int getValues( unsigned long long* values );

            int getValues( float* values );
            int getValues( double* values );

            IGroupPtr getParentGroup();
            std::string getName();

        private:
            template< typename T>
                int getValuesImpl( T* values );

            GroupAtt( GroupImplSptr grp_ptr, const netCDF::NcGroupAtt& ncatt );

            netCDF::NcGroupAtt _ncatt;

            std::weak_ptr< GroupImpl > _parent_group_ptr;
    };

    GroupAtt::GroupAtt( GroupImplSptr grp_ptr, const netCDF::NcGroupAtt& ncatt ) : _parent_group_ptr( grp_ptr),  _ncatt( ncatt ){}

    IGroupPtr GroupAtt::getParentGroup()
    {
        GroupImplSptr grp_ptr(_parent_group_ptr);
        //IGroup::sptr iptr( grp_ptr);


        return std::static_pointer_cast< IGroup >( grp_ptr );
    }


    std::string GroupAtt::getName()
    {
        return _ncatt.getName();
    }



    GroupAtt::sptr GroupAtt::create( GroupImplSptr grp_ptr, const netCDF::NcGroupAtt& ncatt ) 
    {
        return GroupAtt::sptr( new GroupAtt( grp_ptr, ncatt ) );
    }



    template< typename T > 
        int GroupAtt::getValuesImpl( T* values )
        {
            int status =FDP_FILE_STATUS_NOERR;

            try
            {
                _ncatt.getValues( values );
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();

                status = FDP_FILE_STATUS_ERR;
            }

            return status;
        }

    int GroupAtt::getValues( short* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( int* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( long* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( long long* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( unsigned short* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( unsigned int* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( unsigned long* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( unsigned long long* values )
    {
        return this->getValuesImpl( values );
    }


    int GroupAtt::getValues( float* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( double* values )
    {
        return this->getValuesImpl( values );
    }

    int GroupAtt::getValues( std::string& values )
    {
        int status = FDP_FILE_STATUS_NOERR;

        try
        {
            _ncatt.getValues( values );
        }
        catch( NcException& ex )
        {
            logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            status = FDP_FILE_STATUS_ERR;
        }

        return status;
    }


    class VarAtt : public IVarAtt
    {
        public:
            VarAtt() = delete;
            static VarAtt::sptr create( const netCDF::NcVarAtt& ncvaratt ) { return VarAtt::sptr( new VarAtt( ncvaratt ) );}


            DataType getAttType();
            int getClassType(){ return ATTRIBUTE_VAR_TYPE; }

            size_t getAttLength() { return _ncvaratt.getAttLength();}
            std::string getName(){ return _ncvaratt.getName(); }

            int getValues( short* values );
            int getValues( int* values );
            int getValues( long* values );
            int getValues( long long* values );
            int getValues( unsigned short* values );
            int getValues( unsigned int* values );
            int getValues( unsigned long* values );
            int getValues( unsigned long long* values );

            int getValues( float* values );
            int getValues( double* values );

            int getValues( std::string& values );
        private:

            template< typename T>
                int getValuesImpl( T* values );

            VarAtt( const netCDF::NcVarAtt& ncvaratt ) : _ncvaratt( ncvaratt ){}
            netCDF::NcVarAtt _ncvaratt;

    };

    template< typename T > 
        int VarAtt::getValuesImpl( T* values )
        {
            int status = FDP_FILE_STATUS_NOERR;

            try
            {
                _ncvaratt.getValues( values );
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() <<  __PRETTY_FUNCTION__ << ": " << ex.what();

                status = FDP_FILE_STATUS_ERR;
            }

            return status;
        }

    int VarAtt::getValues( short* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( int* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( long* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( long long* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( unsigned short* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( unsigned int* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( unsigned long* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( unsigned long long* values )
    {
        return this->getValuesImpl( values );
    }


    int VarAtt::getValues( float* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( double* values )
    {
        return this->getValuesImpl( values );
    }

    int VarAtt::getValues( std::string& values )
    {
        int status = FDP_FILE_STATUS_NOERR;

        try
        {
            _ncvaratt.getValues( values );
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            status = FDP_FILE_STATUS_ERR;
        }

        return status;
    }

    IAtt::sptr VarImpl::putAttShort( const std::string& key, short value )
    {
        VarAtt::sptr att_ptr;
        try
        {
            netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, netCDF::NcShort(), value );
            att_ptr = VarAtt::create( ncatt );
            _name_varatt_map[ key ] = att_ptr;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() <<  __PRETTY_FUNCTION__ << ": " << ex.what();
        }
        return att_ptr;
    }

    IAtt::sptr VarImpl::putAttInt( const std::string& key, int value )
    {
	    return this->putAttImpl<int>( key, value );
    }

    IAtt::sptr VarImpl::putAttLong( const std::string& key, long value )
    {
	    return this->putAttImpl<long>( key, value );
    }

    IAtt::sptr VarImpl::putAttLongLong( const std::string& key, long long value )
    {
	    return this->putAttImpl<long long>( key, value );
    }

    IAtt::sptr VarImpl::putAttUShort( const std::string& key, unsigned short value )
    {
	    return this->putAttImpl< unsigned short>( key, value );
    }

    IAtt::sptr VarImpl::putAttUInt( const std::string& key, unsigned int value )
    {
	    return this->putAttImpl< unsigned int >( key, value );
    }

    IAtt::sptr VarImpl::putAttFloat( const std::string& key, float value )
    {
	    return this->putAttImpl< float >( key, value );
    }

    IAtt::sptr VarImpl::putAttDouble( const std::string& key, double value )
    {
	    return this->putAttImpl< double >( key, value );
    }


    template< typename T >
	    IVarAtt::sptr VarImpl::putAttImpl( const std::string& key, const T& value )
	    {
		    IVarAtt::sptr var_att;
		    try
		    {
			    typedef typename NcTraits< T >::MyNcType MyNcType;

			    MyNcType theNcType = MyNcType();

			    netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, theNcType, value );
			    var_att = VarAtt::create( ncatt );

			    _name_varatt_map[ key ] = var_att;
		    }
		    catch( NcException& ex )
		    {
			    logger::get_logger()->error() <<  __PRETTY_FUNCTION__ << ": " << ex.what();
		    }

		    return var_att;

	    }


    template< typename T >
	    IVarAtt::sptr VarImpl::putAttImpls( const std::string& key, size_t nvals, const T* values )
        {
            IVarAtt::sptr var_att;
            try
            {
                typedef typename NcTraits< T >::MyNcType MyNcType;

                MyNcType theNcType = MyNcType();

                netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, theNcType, nvals, values );
                var_att = VarAtt::create( ncatt );

                _name_varatt_map[ key ] = var_att;
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() <<  __PRETTY_FUNCTION__ << ": " << ex.what();
            }

            return var_att;
        }

    IAtt::sptr VarImpl::putAttShorts( const std::string& key, size_t nvals, const short* values )
    {
        return this->putAttImpls< short >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttInts( const std::string& key, int nvals, const int* values )
    {
        return this->putAttImpls< int >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttLongs( const std::string& key, size_t nvals, const long* values )
    {
        return this->putAttImpls< long >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttLongLongs( const std::string& key, size_t nvals, const long long* values )
    {
        return this->putAttImpls< long long >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttUShorts( const std::string& key, size_t nvals, const unsigned short* values )
    {
        return this->putAttImpls< unsigned short >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttUInts( const std::string& key, size_t nvals, const unsigned int* values )
    {
        return this->putAttImpls< unsigned int>( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttULongs( const std::string& key, size_t nvals, const unsigned long* values )
    {
        return this->putAttImpls< unsigned long >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttULongLongs( const std::string& key, size_t nvals, const unsigned long long* values )
    {
        return this->putAttImpls< unsigned long long >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttFloats( const std::string& key, size_t nvals, const float* values )
    {
        return this->putAttImpls< float >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttDoubles( const std::string& key, size_t nvals, const double* values )
    {
        return this->putAttImpls< double >( key, nvals, values );
    }

    IAtt::sptr VarImpl::putAttStrs( const std::string& key, size_t nvals, const char** values )
    {
        IVarAtt::sptr var_att;
        try
        {
            netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, netCDF::NcString(), nvals, values );
            var_att = VarAtt::create( ncatt );

            _name_varatt_map[ key ] = var_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() <<  __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return var_att;
    }

    IAtt::sptr VarImpl::putAttString( const std::string& key, const std::string& value )
    {
        VarAtt::sptr att_ptr;
        try
        {
            netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, value );
            att_ptr = VarAtt::create( ncatt );
            _name_varatt_map[ key ] = att_ptr;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }
        return att_ptr;
    }
    
    IAtt::sptr VarImpl::getAtt( const std::string& key )
    {
        IVarAtt::sptr att_ptr;
        auto it = _name_varatt_map.find( key );
        if( it != _name_varatt_map.end() )
        {
            att_ptr = it->second;
        }
        else
        {
            try
            {
                netCDF::NcVarAtt ncatt = _nc_var.getAtt( key );
                att_ptr = VarAtt::create( ncatt );
                _name_varatt_map[ key ] = att_ptr;
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            }

        }
        return att_ptr;
    }


    DataType VarImpl::getType()
    {
        auto nctype = _nc_var.getType();

        DataType dtype = NcType2DataType( nctype.getTypeClass() );
        return dtype;
    }

    std::vector< IDimension::sptr > VarImpl::getDims()
    {
        std::vector< IDimension::sptr > dim_ptrs;

        std::vector< netCDF::NcDim > nc_dims = _nc_var.getDims();

        auto parent_grp_ptr = this->parent();

        for( auto it = nc_dims.begin(); it != nc_dims.end(); ++it )
        {
            const std::string& dim_name = it->getName();

            auto dim_ptr = parent_grp_ptr->getDim( dim_name );
            dim_ptrs.push_back( dim_ptr );
        }

        return dim_ptrs;
    }

    void VarImpl::putVar( const void* vals )
    {
        _nc_var.putVar( vals );
    }

    void VarImpl::getVar( void* vals )
    {
        _nc_var.getVar( vals );
    }

    VarImpl::VarImpl( IGroup::sptr parent, const std::string& name, const netCDF::NcVar& nc_var ) 
        : _parent(parent), _nc_var( nc_var ) 
    {
    }

    VarImpl::sptr VarImpl::create( IGroup::sptr parent, const std::string& name, const netCDF::NcVar& nc_var )
    {
        return VarImpl::sptr( new VarImpl( parent, name, nc_var ) );
    }

    class DimensionImpl : public IDimension
    {
        public:
            typedef std::shared_ptr< DimensionImpl > sptr;
            friend class GroupImpl;

            DimensionImpl() = delete;

            static sptr create( IGroup::sptr parent, const netCDF::NcDim& nc_dim )
            {
                return DimensionImpl::sptr( new DimensionImpl( parent, nc_dim ) );
            }

            size_t getSize(){ return _nc_dim.getSize();}
            std::string getName(){return _nc_dim.getName();}
            IGroup::sptr getParentGroup(){ return _parent.lock();}

            bool isUnlimited() { return _nc_dim.isUnlimited(); }


        private:
            DimensionImpl( IGroup::sptr parent, const netCDF::NcDim& nc_dim ) : _parent(parent), _nc_dim( nc_dim ) 
        {
        }
            netCDF::NcDim _nc_dim;
            std::weak_ptr< IGroup > _parent;
    };


    class GroupImpl
        : public std::enable_shared_from_this< GroupImpl>
          , public IGroup
    {
        public:
            virtual ~GroupImpl(){}

            typedef std::shared_ptr< GroupImpl > sptr;

            std::string getName();

            int getGroupCount();
            IGroup::sptr parent();

            IGroup::sptr rootGroup();

            IGroup::sptr getGroup( const std::string& name );


            std::vector< std::string > getGroups();

            IGroup::sptr requireGroup( const std::string& name );


            IDimension::sptr addDim( const std::string& name, size_t sz );
            IDimension::sptr addUnlimitedDim( const std::string& name );

            IDimension::sptr getDim( const std::string& name );

            IVar::sptr addVar( const std::string& name, DataType dataType
                    , std::vector< IDimension::sptr >& vdims );

            IVar::sptr getVar( const std::string& name );
            std::vector< std::string > getVars();


	    size_t getAttCount(){ return _name_att_map.size();}

            IAtt::sptr getAtt( const std::string& key );

            std::vector< IAtt::sptr > getAtts();

            IAtt::sptr putAttShort( const std::string& key, short value );
            IAtt::sptr putAttInt( const std::string& key, int value );
            IAtt::sptr putAttLong( const std::string& key, long value );
            IAtt::sptr putAttLongLong( const std::string& key, long long value );

            IAtt::sptr putAttUShort( const std::string& key, unsigned short value );
            IAtt::sptr putAttUInt( const std::string& key, unsigned int value );

            IAtt::sptr putAttString( const std::string& key, const std::string& value );
            IAtt::sptr putAttFloat( const std::string& key, float value );
            IAtt::sptr putAttDouble( const std::string& key, double value );

            IAtt::sptr putAttShorts( const std::string& key, size_t nvals, const short* values );
            IAtt::sptr putAttInts( const std::string& key, int nvals, const int* values );
            IAtt::sptr putAttLongs( const std::string& key, size_t nvals, const long* values );
            IAtt::sptr putAttLongLongs( const std::string& key, size_t nvals, const long long* values );

            IAtt::sptr putAttUShorts( const std::string& key, size_t nvals, const unsigned short* values );
            IAtt::sptr putAttUInts( const std::string& key, size_t nvals, const unsigned int* values );
            IAtt::sptr putAttULongs( const std::string& key, size_t nvals, const unsigned long* values );
            IAtt::sptr putAttULongLongs( const std::string& key, size_t nvals, const unsigned long long* values );

            IAtt::sptr putAttFloats( const std::string& key, size_t nvals, const float* values );
            IAtt::sptr putAttDoubles( const std::string& key, size_t nvals, const double* values );

            IAtt::sptr putAttStrs( const std::string& key, size_t nvals, const char** values );

	    IAtt::sptr putAttVal( const std::string& key, DataType datatyp, const void* pv );
            IAtt::sptr putAttVals( const std::string& key
                    , DataType datatype, size_t nvals, const void* pv );

            int prepare( const CoordinatVariableDefinition& cvd );
            int prepare( const TableDefinition& td );
            int prepare( const DimensionalVariableDefinition& dvd );

        protected:

        typedef std::map< std::string, IGroup::sptr > NAME_GROUP_MAP;
        typedef std::map< std::string, IDimension::sptr > NAME_DIM_MAP;
        typedef std::map< std::string, IVar::sptr > NAME_VAR_MAP;
        typedef std::map< std::string, IGroupAtt::sptr > NAME_ATT_MAP;


            static GroupImpl::sptr create( GroupImpl::sptr p, const std::string& name );

            NcGroupPtr _nc;	

            GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc );

            template< typename T >
                IGroupAtt::sptr putAttImpls( const std::string& key
                        , size_t nvals, const T* values );

        private:
            std::string _name;

            IGroup::sptr _addGroup( const std::string& name );

            IGroup::sptr  _getGroup( const std::string name );

            std::weak_ptr<GroupImpl> _parent;

            NAME_GROUP_MAP _name_grp_map;
            NAME_DIM_MAP _name_dim_map;
            NAME_VAR_MAP _name_var_map;

            NAME_ATT_MAP _name_att_map;

    };

    std::vector< IAtt::sptr > GroupImpl::getAtts()
    {
	    std::vector< IAtt::sptr > v_atts;

	    auto nc_name_att_map = _nc->getAtts();
	    auto nc_its = nc_name_att_map.begin();
	    auto nc_ite = nc_name_att_map.end();

	    for(auto it = nc_its; it != nc_ite; it++)
	    {
		    const std::string& name = it->first;
		    IAtt::sptr att_ptr = this->getAtt( name );
		    if( att_ptr )
			    v_atts.push_back( att_ptr );
	    }

	    return v_atts;
    }

    IAtt::sptr GroupImpl::putAttVal( const std::string& key, DataType datatype, const void* pv )
    {
        IVarAtt::sptr att_ptr;
        switch( datatype )
        {
            case DataType::SHORT:
                this->putAttShort( key, *(const short*)(pv) );
                break;
            case DataType::INT:
                this->putAttInt( key, *(const int*)(pv) );
                break;
            case DataType::INT64:
                this->putAttLong( key, *(const long*)(pv) );
                break;
            case DataType::USHORT:
                this->putAttUShort( key, *(const unsigned short*)(pv) );
                break;
            case DataType::UINT:
                this->putAttUInt( key, *(const unsigned int*)(pv) );
                break;
	    //case DataType::UINT64:
                //this->putAttULong( key, *(const unsigned long*)(pv) );
                //break;
            case DataType::FLOAT:
                this->putAttFloat( key, *(const float*)(pv) );
                break;
            case DataType::DOUBLE:
                this->putAttDouble( key, *(const double*)(pv) );
                break;

            default:
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": unhanded datatype";
                break;

        }

        return att_ptr;
    }

    IAtt::sptr GroupImpl::putAttVals( const std::string& key
            , DataType datatype, size_t nvals, const void* pv )
    {
        IVarAtt::sptr att_ptr;
        switch( datatype )
        {
            case DataType::SHORT:
                this->putAttShorts( key, nvals, (const short*)(pv) );
                break;
            case DataType::INT:
                this->putAttInts( key, nvals, (const int*)(pv) );
                break;
            case DataType::INT64:
                this->putAttLongs( key, nvals, (const long*)(pv) );
                break;
            case DataType::USHORT:
                this->putAttUShorts( key, nvals, (const unsigned short*)(pv) );
                break;
            case DataType::UINT:
                this->putAttUInts( key, nvals, (const unsigned int*)(pv) );
                break;
            case DataType::UINT64:
                this->putAttULongs( key, nvals, (const unsigned long*)(pv) );
                break;
            case DataType::FLOAT:
                this->putAttFloats( key, nvals, (const float*)(pv) );
                break;
            case DataType::DOUBLE:
                this->putAttDoubles( key, nvals, (const double*)(pv) );
                break;

            default:
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": unhanded datatype";
                break;

        }
        return att_ptr;
    }



    template< typename T >
        IGroupAtt::sptr GroupImpl::putAttImpls( const std::string& key, size_t nvals, const T* values )
        {
            IGroupAtt::sptr grp_att;
            try
            {
                typedef typename NcTraits< T >::MyNcType MyNcType;

                MyNcType theNcType = MyNcType();

                netCDF::NcGroupAtt ncatt = _nc->putAtt( key, theNcType, nvals, values );
                grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

                _name_att_map[ key ] = grp_att;
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            }


            return grp_att;
        }


    IAtt::sptr GroupImpl::putAttStrs( const std::string& key, size_t nvals, const char** values )
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcString(), nvals, values );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }


        return grp_att;
    }


    IAtt::sptr GroupImpl::putAttDoubles( const std::string& key, size_t nvals, const double* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls< double >( key, nvals, values );
        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttFloats( const std::string& key, size_t nvals, const float* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls< float >( key, nvals, values );
        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttShorts( const std::string& key, size_t nvals, const short* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls< short >( key, nvals, values );
        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttInts( const std::string& key, int nvals, const int* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls< int >( key, nvals, values );
        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttLongs( const std::string& key, size_t nvals, const long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls< long >( key, nvals, values );
        return grp_att;
    }


    IAtt::sptr GroupImpl::putAttLongLongs( const std::string& key, size_t nvals, const long long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls< long long >( key, nvals, values );
        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttUShorts( const std::string& key, size_t nvals, const unsigned short* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls( key, nvals, values );

        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttUInts( const std::string& key, size_t nvals, const unsigned int* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls( key, nvals, values );
        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttULongs( const std::string& key, size_t nvals, const unsigned long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls( key, nvals, values );
        return grp_att;
    }


    IAtt::sptr GroupImpl::putAttULongLongs( const std::string& key, size_t nvals, const unsigned long long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls( key, nvals, values );
        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttFloat( const std::string& key, float value )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls<float>( key, 1, &value );
        return grp_att;
    }
    IAtt::sptr GroupImpl::putAttDouble( const std::string& key, double value )
    {
        IGroupAtt::sptr grp_att = this->putAttImpls<double>( key, 1, &value );
        return grp_att;
    }



    IAtt::sptr GroupImpl::putAttString(const std::string& key, const std::string& value )
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_att;
    }
    IAtt::sptr GroupImpl::putAttShort(const std::string& key, short value)
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcShort(), value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttInt(const std::string& key, int value)
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcInt(), value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_att;
    }
    IAtt::sptr GroupImpl::putAttLong(const std::string& key, long value)
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcInt64(), value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_att;
    }
    IAtt::sptr GroupImpl::putAttLongLong(const std::string& key, long long value)
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcInt64(), value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttUShort(const std::string& key, unsigned short value)
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcUshort(), value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_att;
    }

    IAtt::sptr GroupImpl::putAttUInt(const std::string& key, unsigned int value)
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcUint(), value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_att;
    }


    IAtt::sptr GroupImpl::getAtt( const std::string& key )
    {
        IGroupAtt::sptr att_ptr;
        auto it = _name_att_map.find( key );
        if( it != _name_att_map.end() )
        {
            att_ptr = it->second;
        }
        else
        {
            try
            {
                netCDF::NcGroupAtt ncatt = _nc->getAtt( key );
                att_ptr = GroupAtt::create( this->shared_from_this(), ncatt );
                _name_att_map[ key ] = att_ptr;
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            }

        }
        return att_ptr;

    }

    IVar::sptr GroupImpl::addVar( const std::string& name, DataType dataType, IVar::VDIMS& vdims )
    {
        VarImpl::sptr var_ptr;

        PARENT_ITEM_TYPE grp_item = split_item_name( name );

        const std::string& grp_name = grp_item.first;
        const std::string itm_name = grp_item.second;

        IGroup::sptr grp_ptr = this->requireGroup( grp_name );
        auto grp_impl_ptr = std::static_pointer_cast< GroupImpl > ( grp_ptr );

        std::vector< netCDF::NcDim > ncdims;
        for( auto it = vdims.begin(); it != vdims.end(); ++it )
        {
            auto dim_ptr = std::dynamic_pointer_cast< DimensionImpl >(*it);
            ncdims.push_back( dim_ptr->_nc_dim );
        }

        try
        {
            netCDF::NcType::ncType nctype = DataType2NcType( dataType );
            netCDF::NcVar ncvar = grp_impl_ptr->_nc->addVar( itm_name, nctype, ncdims );
            var_ptr = VarImpl::create( grp_impl_ptr, name,  ncvar );
            grp_impl_ptr->_name_var_map[ itm_name ] = var_ptr;
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return var_ptr;
    }

    std::vector< std::string > GroupImpl::getVars()
    {
        std::vector< std::string > var_names;
        try
        {
            if( !_nc->isNull() )
            {
                std::multimap< std::string, netCDF::NcVar > ncvars = _nc->getVars();
                for( auto it = ncvars.begin(); it != ncvars.end(); ++it )
                {
                    var_names.push_back( it->first );
                }
            }
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }
        return var_names;
    }

    IVar::sptr GroupImpl::getVar( const std::string& name )
    {
        IVar::sptr var_ptr;
        auto it = _name_var_map.find( name );
        if( it != _name_var_map.end() )
        {
            var_ptr = it->second;
        }
        else
        {
            try
            {
                netCDF::NcVar ncvar = _nc->getVar( name );

                if( !ncvar.isNull() )
                {
                    var_ptr = VarImpl::create( this->shared_from_this(), name, ncvar );
                    _name_var_map[ name ] = var_ptr;
                }
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            }
        }

        return var_ptr;
    }

    IDimension::sptr GroupImpl::getDim( const std::string& name )
    {
        IDimension::sptr dim_ptr;
        IGroup::sptr grp_ptr = this->shared_from_this();

        PARENT_ITEM_TYPE parent_item = split_item_name( name );
        const std::string& grp_name = parent_item.first;
        const std::string& dim_name = parent_item.second;

        if( !(grp_name.empty()) )
            grp_ptr = grp_ptr->getGroup( grp_name );

        if( grp_ptr )
        {
            GroupImpl::sptr grp_impl_ptr = std::static_pointer_cast< GroupImpl > ( grp_ptr );

            auto it = grp_impl_ptr->_name_dim_map.find( dim_name );
            if( it != grp_impl_ptr->_name_dim_map.end() )
            {
                dim_ptr = it->second;
            }
            else
            {
                try
                {
                    netCDF::NcDim nc_dim = grp_impl_ptr->_nc->getDim( dim_name );

                    if( !nc_dim.isNull() )
                    {
                        dim_ptr = DimensionImpl::create( grp_ptr, nc_dim );
                        grp_impl_ptr->_name_dim_map[ dim_name ] = dim_ptr;
                    }
                }
                catch( NcException& ex )
                {
		    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
                }
            }
        }
        return dim_ptr;

    }

    IDimension::sptr GroupImpl::addUnlimitedDim( const std::string& name )
    {
        DimensionImpl::sptr dim_ptr;

        try
        {
            netCDF::NcDim nc_dim = _nc->addDim( name );

            if( !nc_dim.isNull() )
            {
                dim_ptr = DimensionImpl::create( this->shared_from_this(), nc_dim );

                _name_dim_map[ name ] = dim_ptr;
            }
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return dim_ptr;
    }

    IDimension::sptr GroupImpl::addDim( const std::string& name, size_t sz )
    {
        DimensionImpl::sptr dim_ptr;

        try
        {
            netCDF::NcDim nc_dim = _nc->addDim( name, sz );

            if( !nc_dim.isNull() )
            {
                dim_ptr = DimensionImpl::create( this->shared_from_this(), nc_dim );

                _name_dim_map[ name ] = dim_ptr;
            }
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return dim_ptr;
    }

    int GroupImpl::getGroupCount()
    {
        return this->_nc->getGroupCount();
    }

    IGroup::sptr GroupImpl::getGroup( const std::string& name )
    {
        IGroup::sptr parent_grp_ptr = this->shared_from_this();
        IGroup::sptr grp_ptr;

        if( name.empty() )
            return parent_grp_ptr;
        else if( '/' == name[0] )
            parent_grp_ptr = this->rootGroup();

        std::vector< std::string > splits;
        split_str( name, '/', splits );

        auto its = splits.begin();
        auto ite = splits.end();

        for( auto it = its; parent_grp_ptr && it != ite; ++it )
        {
            grp_ptr = std::static_pointer_cast< GroupImpl >( parent_grp_ptr)->_getGroup( *it );
            parent_grp_ptr = grp_ptr;
        }

        return grp_ptr;
    }

    IGroup::sptr  GroupImpl::_getGroup( const std::string name )
    {
        if( name.empty() )
            return this->shared_from_this();
        IGroup::sptr grp_ptr;

        auto it = _name_grp_map.find( name );
        if( it != _name_grp_map.end() )
        {
            grp_ptr = it->second;
        }
        else
        {
            try
            {
                NcGroup new_ncgrp = this->_nc->getGroup( name );
                if( ! new_ncgrp.isNull() )
                {
                    GroupImpl::sptr new_grp_ptr = GroupImpl::create( this->shared_from_this(), name );
                    new_grp_ptr->_nc = std::make_shared<NcGroup>(new_ncgrp);
                    grp_ptr = new_grp_ptr;
                    _name_grp_map[ name ] = grp_ptr;
                }
            }
            catch( NcException& ex )
            {
		logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
            }
        }
        return grp_ptr;
    }

    std::vector< std::string > GroupImpl::getGroups()
    {
        std::vector< std::string > grp_names;

        try
        {
            auto nc_grps = this->_nc->getGroups();
            for( auto it_nc_grp = nc_grps.begin(); it_nc_grp != nc_grps.end(); ++it_nc_grp )
            {
                std::string name = it_nc_grp->first;
                grp_names.push_back( name );
            }
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return grp_names;
    }

    IGroup::sptr GroupImpl::rootGroup()
    {
        IGroup::sptr parent_grp_ptr = this->shared_from_this();
        IGroup::sptr root_grp_ptr = parent_grp_ptr;

        while( parent_grp_ptr )
        {
            parent_grp_ptr = parent_grp_ptr->parent();
            if( parent_grp_ptr )
                root_grp_ptr = parent_grp_ptr;
        }
        return root_grp_ptr; 
    }

    IGroup::sptr GroupImpl::requireGroup( const std::string& name )
    {

        logging::ScopeLogger( *(logger::get_logger()), __PRETTY_FUNCTION__ );

        IGroup::sptr grp_ptr;
        IGroup::sptr parent_grp_ptr = this->shared_from_this();

        if( name.empty() )
            return parent_grp_ptr;
        else if( '/' == name[0] )
            parent_grp_ptr = this->rootGroup();

        std::vector< std::string > splits;
        split_str( name, '/', splits );

        for( auto it = splits.begin(); it != splits.end(); ++it )
        {
            if( parent_grp_ptr )
            {
                grp_ptr = std::static_pointer_cast< GroupImpl >(parent_grp_ptr)->_getGroup( *it );

                if( !grp_ptr )
                    grp_ptr = std::static_pointer_cast<GroupImpl>(parent_grp_ptr)->_addGroup( *it );

                parent_grp_ptr = grp_ptr;
            }
        }

        return grp_ptr;
    }

    IGroup::sptr GroupImpl::_addGroup( const std::string& name )
    {
        GroupImpl::sptr grp_ptr;

        NcGroupPtr nc = std::dynamic_pointer_cast< NcGroup >(_nc);

        try
        {
            NcGroup new_ncgrp( nc->addGroup( name ) );

            if( !new_ncgrp.isNull() )
            {
                auto self_ptr = this->shared_from_this();

                GroupImpl::sptr new_grp_ptr = GroupImpl::create( self_ptr, name );

                new_grp_ptr->_nc =std::make_shared< NcGroup >( new_ncgrp) ;

                grp_ptr = new_grp_ptr;

                _name_grp_map[ name ] = grp_ptr;
            }
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }

        return  grp_ptr;

    }

    GroupImpl::sptr GroupImpl::create( GroupImpl::sptr p, const std::string& name )
    {
        auto _nc = std::make_shared< NcGroup >();
        auto pgrp = GroupImpl::sptr( new GroupImpl(p, _nc));
        pgrp->_name = name;
        return pgrp;
    }


    GroupImpl::GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc ) : _parent( grp ), _nc(nc)
    {
    }

    IGroup::sptr GroupImpl::parent()
    {
        return _parent.lock(); 
    }

    std::string GroupImpl::getName()
    {
        std::string s = this->_nc->getName();
        return s;
    }


    class FileImpl 
        : public GroupImpl
    {
        public:
            typedef std::shared_ptr< FileImpl > sptr;

            static sptr create(const std::string& path, IFile::Mode mode );

            void open( const std::string& path , IFile::Mode mode );
            void close();

        private:
            NcFilePtr ncfile(){ return std::dynamic_pointer_cast< NcFile >( _nc ); }

            FileImpl( const std::string& path, IFile::Mode mode ) ;

    };



    FileImpl::sptr FileImpl::create(const std::string& path, IFile::Mode mode )
    {
        auto p = FileImpl::sptr( new FileImpl(path, mode ));
        return p;
    }

    void FileImpl::open( const std::string& path, IFile::Mode mode )
    {
        auto ncmode = NcFile::FileMode::read;

        switch( mode )
        {
            case IFile::Mode::READ:
                {
                    try
                    {
                        ncfile()->open( path, ncmode );
                    }
                    catch( NcException& ex )
                    {
			logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
                    }
                }
                break;
            case IFile::Mode::WRITE:
                {
                    try
                    {
                        //open existing file
                        ncmode = NcFile::FileMode::write;
                        ncfile()->open( path, ncmode );
                    }
                    catch( NcException& e )
                    {
                        //if does not exist then create a new one
                        ncmode = NcFile::FileMode::newFile;

                        ncfile()->open( path, ncmode );
                    }
                }
                break;
            default:
                break;

        }
    }

    void FileImpl::close()
    {
        try
        {
            ncfile()->close();
        }
        catch( NcException& ex )
        {
	    logger::get_logger()->error() << __PRETTY_FUNCTION__ << ": " << ex.what();
        }
    }


    FileImpl::FileImpl( const std::string& path, IFile::Mode mode ) 
        : GroupImpl( NULL, std::make_shared<NcFile>() ) 
    {
        this->open( path, mode );
    }


    IFile::sptr FileFactory::create( const std::string& path, Mode mode ) 
    {
        auto builder = FileImpl::create( path, mode );
        return builder;
    }

    std::string str_strip_right( std::string str, const std::string& chars )
    {
        str.erase( str.find_last_not_of( chars ) + 1 );
        return str;
    }

    std::string str_strip_left( std::string str, const std::string& chars )
    {
        str.erase( 0, str.find_first_not_of( chars ) );
        return str;
    }

    std::string str_strip( std::string str, const std::string& chars )
    {
        str = str_strip_right( str, chars );
        str = str_strip_left( str, chars );
        return str;
    }

    PARENT_ITEM_TYPE split_item_name( std::string str )
    {
        bool bAbsPath = (str[0] == '/');

        str = str_strip( str, "/" );

        size_t pos = str.rfind( "/" );

        std::string str_grp;
        std::string str_itm;

        if( std::string::npos == pos )
        {
            //not found
            str_grp = "";
            str_itm = str;
        }
        else
        {
            str_grp = std::string( str.begin(), str.begin() + pos );
            str_itm = std::string( str.begin() + pos +1 , str.end() );
        }

        if(bAbsPath)
            str_grp.insert( 0, 1, '/' );

        return  std::make_pair( str_grp, str_itm );
    }

    //    int CoordinatVariableDefinition::UNLIMITED = 0;

    bool CoordinatVariableDefinition::isUnlimited() const
    {
        return this->size == FDP_UNLIMITED;
    }

    size_t TableDefinition::getSize() const
    {
        return this->size;
    }

    bool TableDefinition::isUnlimited() const
    {
        return this->getSize() == FDP_UNLIMITED;
    }

    std::string TableDefinition::getVariableName( int i )
    {
        PARENT_ITEM_TYPE col_grp_item = split_item_name( this->columns[i].name );


        return this->name + "/" + col_grp_item.second;
    }



    int GroupImpl::prepare( const CoordinatVariableDefinition& cvd )
    {
        int status = 0;

        PARENT_ITEM_TYPE grp_item = split_item_name( cvd.name );

        const std::string& grp_name = grp_item.first;
        const std::string& itm_name = grp_item.second;

        IGroup::sptr grp_ptr = this->requireGroup( grp_name );

        status = ( grp_ptr != NULL )  ?FDP_FILE_STATUS_NOERR : FDP_FILE_STATUS_ERR;

        IDimension::sptr dim_ptr;

        if( grp_ptr )
        {
            if( cvd.isUnlimited() )
                dim_ptr = grp_ptr->addUnlimitedDim( itm_name );
            else
                dim_ptr = grp_ptr->addDim( itm_name, cvd.size );
        }

        status = ( dim_ptr != NULL ) ? FDP_FILE_STATUS_NOERR : FDP_FILE_STATUS_ERR;

        std::vector< IDimension::sptr > vdims = { dim_ptr };


        IVar::sptr var_ptr = grp_ptr->addVar( itm_name, cvd.datatype, vdims );

        if( var_ptr )
            var_ptr->putVar( cvd.values );

        status = ( var_ptr != NULL ) ? FDP_FILE_STATUS_NOERR : FDP_FILE_STATUS_ERR;

        if( !cvd.description.empty() )
            var_ptr->putAttVal( ATTRIB_KEY_DESC, DataType::STRING, &(cvd.description[0]) );

        if( !cvd.units.empty() )
            var_ptr->putAttVal( ATTRIB_KEY_UNITS, DataType::STRING, &( cvd.units[0]) );

        if( !cvd.long_name.empty() )
            var_ptr->putAttVal( ATTRIB_KEY_LNAME, DataType::STRING, &(cvd.long_name[0]) );

        if(cvd._missing_ptr)
            var_ptr->putAttVal( ATTRIB_KEY_FILLVALUE, cvd.datatype, cvd._missing_ptr.get() );

        return status;
    }
    int GroupImpl::prepare( const TableDefinition& td )
    {
        int status = 0;

        PARENT_ITEM_TYPE parent_item = split_item_name( td.name );
        const std::string& grp_name = parent_item.first;
        const std::string& itm_name = parent_item.second;

        IGroup::sptr grp_ptr = this->requireGroup( grp_name );

        status = ( grp_ptr != NULL ) ? 0 : 1;

        std::string dim_name = "index";

        if( 0 == status )
        {
            IDimension::sptr dim_ptr;

            if( grp_ptr )
            {
                if( td.isUnlimited() )
                    dim_ptr = grp_ptr->addUnlimitedDim( dim_name );
                else
                    dim_ptr = grp_ptr->addDim( dim_name, td.getSize() );
            }

            status = ( dim_ptr != NULL ) ? 0 : 1;
        }

        if( 0==status )
        {
            if( !td.description.empty() )
                grp_ptr->putAttVal( ATTRIB_KEY_DESC, DataType::STRING, &(td.description[0]) );

            if( !td.long_name.empty() )
                grp_ptr->putAttVal( ATTRIB_KEY_LNAME, DataType::STRING, &(td.long_name[0]) );

            // add the columns as DimensionalVariableDefinitions
            for( int i = 0; i < !status && td.getColumns().size(); ++i )
            {
                const LocalVAriableDefinition& lvd = td.getColumns()[i];

                DimensionalVariableDefinition dvd;

                if( grp_name.empty() )
                    dvd.name = lvd.name;
                else
                    dvd.name = grp_name + "/" + lvd.name;

                dvd.datatype = lvd.datatype;
                dvd.dimensions = { dim_name };

                status = grp_ptr->prepare( dvd );
            }

            grp_ptr->putAttString( ATTRIB_KEY_GROUP_TYPE, "table" );
        }

        return status;
    }

    const std::vector< std::string >& DimensionalVariableDefinition::getDimensions() const
    {
        return this->dimensions;
    }

    int GroupImpl::prepare( const DimensionalVariableDefinition& dvd )
    {
        IVar::sptr var_ptr;
        int status = 0;

        PARENT_ITEM_TYPE grp_item = split_item_name( dvd.name );
        const std::string& dvd_grp_name = grp_item.first;
        const std::string& dvd_itm_name = grp_item.second;

        IGroup::sptr dvd_grp_ptr = this->requireGroup( dvd_grp_name );

        status = dvd_grp_ptr ? 0 : 1;

        std::vector< IDimension::sptr > dims;

        for( size_t i = 0; i < dvd.getDimensions().size() && !status ; ++i )
        {
            IDimension::sptr dim_ptr = this->getDim( dvd.getDimensions()[i] );

            if( dim_ptr )
                dims.push_back( dim_ptr );
            else
                status = 1;
        }

        if( !status )
        {
            var_ptr = dvd_grp_ptr->addVar( dvd_itm_name, dvd.datatype, dims );
            status = var_ptr ? 0 : 1;
        }

        if( var_ptr )
        {
            if( !dvd.description.empty() )
                var_ptr->putAttVal( ATTRIB_KEY_DESC, DataType::STRING, &(dvd.description[0]) );
            if( !dvd.units.empty() )
                var_ptr->putAttVal( ATTRIB_KEY_UNITS, DataType::STRING, &(dvd.units[0]) );
            if( !dvd.long_name.empty() )
            {
                const std::string s = ATTRIB_KEY_LNAME;
                var_ptr->putAttVal( s, DataType::STRING, &(dvd.long_name[0]) );
            }

            if( dvd._missing_ptr )
                var_ptr->putAttVal( ATTRIB_KEY_FILLVALUE, dvd.datatype, dvd._missing_ptr.get() );
        }

        return status;
    }


    DataType VarAtt::getAttType()
    {
        auto nctype = _ncvaratt.getType().getTypeClass();
        DataType dtype = NcType2DataType( nctype );
        return dtype;
    }

    DataType GroupAtt::getAttType()
    {
        auto nctype = _ncatt.getType().getTypeClass();

        DataType dtype = NcType2DataType( nctype );
        return dtype;
    }
}


