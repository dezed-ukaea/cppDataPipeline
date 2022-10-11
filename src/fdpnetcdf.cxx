#include "fdp/fdpnetcdf.hxx"
#include<sstream>

#include<netcdf>

typedef netCDF::exceptions::NcException NcException;
typedef netCDF::NcGroup NcGroup;
typedef netCDF::NcFile NcFile;

typedef std::shared_ptr< NcGroup > NcGroupPtr;
typedef std::shared_ptr< NcFile > NcFilePtr;

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

    class VarImpl : public IVar
    {
        public:
            typedef std::shared_ptr< VarImpl > sptr;

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

            IGroup::sptr parent(){return _parent.lock();}

            IVarAtt::sptr putAtt( const std::string& key, const std::string& value );
            IVarAtt::sptr putAtt( const std::string& key, int value );
            IVarAtt::sptr putAtt( const std::string& key, float value );

            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const short* values );
            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const int* values );
            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const long* values );
            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const long long* values );

            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned short* values );
            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned int* values );
            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long* values );
            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long long* values );

            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const float* values );
            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const double* values );

            IVarAtt::sptr putAtt( const std::string& key, size_t nvals, const char** values );



            IVarAtt::sptr getAtt( const std::string& key );

        private:
            template< typename T >
                IVarAtt::sptr putAttImpl( const std::string& key
                        , size_t nvals, const T* values );

	    template< typename T >  
		    void putVarImpl( const std::vector< size_t >& index, T* values ) const;

	    template< typename T >  
		 void putVarImpl( const std::vector< size_t >& index
				 , const std::vector< size_t >& countp
				 , T* values ) const;




            std::weak_ptr< IGroup > _parent;
            VarImpl( IGroup::sptr parent, const std::string& name, const netCDF::NcVar& nc_var ) ;
            netCDF::NcVar _nc_var;

            IVar::NAME_VARATT_MAP _name_varatt_map;
    };
    template< typename T >  
	    void VarImpl::putVarImpl( const std::vector< size_t >& index, T* values ) const
	    {
		    try
		    {
			    _nc_var.putVar( index, values );
		    }
		    catch (NcException& ex )
		    {
			    ex.what();
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
			    ex.what();
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
		    ex.what();
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
			//GroupImplSptr _getParentGroup();
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
            int status =FDP_FILE_NOERR;

            try
            {
                _ncatt.getValues( values );
            }
            catch( NcException& e )
            {
                e.what();
		status = FDP_FILE_ERR;
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
        int status = FDP_FILE_NOERR;

        try
        {
            _ncatt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
	    status = FDP_FILE_ERR;
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
            int status = FDP_FILE_NOERR;

            try
            {
                _ncvaratt.getValues( values );
            }
            catch( NcException& e )
            {
                e.what();
		status = FDP_FILE_ERR;
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
        int status = FDP_FILE_NOERR;

        try
        {
            _ncvaratt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
	    status = FDP_FILE_ERR;
        }

        return status;
    }
    
    IVarAtt::sptr VarImpl::putAtt( const std::string& key, int value )
    {
        VarAtt::sptr att_ptr;
        try
        {
            netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, netCDF::NcInt(), value );
            att_ptr = VarAtt::create( ncatt );
            _name_varatt_map[ key ] = att_ptr;
        }
        catch( NcException& e )
        {
            e.what();
        }
        return att_ptr;
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, float value )
    {
        VarAtt::sptr att_ptr;
        try
        {
            netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, netCDF::NcFloat(), value );
            att_ptr = VarAtt::create( ncatt );
            _name_varatt_map[ key ] = att_ptr;
        }
        catch( NcException& e )
        {
            e.what();
        }
        return att_ptr;

    }
    template< typename T >
        IVarAtt::sptr VarImpl::putAttImpl( const std::string& key, size_t nvals, const T* values )
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
            catch( NcException& e )
            {
                e.what();
            }

            return var_att;
        }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const short* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const int* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const long* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const long long* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const unsigned short* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const unsigned int* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const unsigned long* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const unsigned long long* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const float* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const double* values )
    {
        return this->putAttImpl( key, nvals, values );
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, size_t nvals, const char** values )
    {
        IVarAtt::sptr var_att;
        try
        {
            netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, netCDF::NcString(), nvals, values );
            var_att = VarAtt::create( ncatt );

            _name_varatt_map[ key ] = var_att;
        }
        catch( NcException& e )
        {
            e.what();
        }

        return var_att;
    }

    IVarAtt::sptr VarImpl::putAtt( const std::string& key, const std::string& value )
    {
        VarAtt::sptr att_ptr;
        try
        {
            netCDF::NcVarAtt ncatt = _nc_var.putAtt( key, value );
            att_ptr = VarAtt::create( ncatt );
            _name_varatt_map[ key ] = att_ptr;
        }
        catch( NcException& e )
        {
            e.what();
        }
        return att_ptr;
    }

    IVarAtt::sptr VarImpl::getAtt( const std::string& key )
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
            catch( NcException& e )
            {
                e.what();
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

            IGroup::sptr getGroup( const std::string& name );

            IGroup::sptr  _getGroup( const std::string name );

            std::vector< std::string > getGroups();

            IGroup::sptr requireGroup( const std::string& name );

            IGroup::sptr addGroup( const std::string& name );

            IDimension::sptr addDim( const std::string& name, size_t sz );
            IDimension::sptr addUnlimitedDim( const std::string& name );
            
            IDimension::sptr getDim( const std::string& name );

            IVar::sptr addVar( const std::string& name, DataType dataType
                    , std::vector< IDimension::sptr >& vdims );

            IVar::sptr getVar( const std::string& name );
            std::vector< std::string > getVars();

            IGroupAtt::sptr putAtt( const std::string& key, const std::string& value );
            IGroupAtt::sptr putAtt( const std::string& key, int value );
            IGroupAtt::sptr putAtt( const std::string& key, float value );

            IGroupAtt::sptr getAtt( const std::string& key );
            std::vector< std::string > getAtts();

        
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const short* values );
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const int* values );
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const long* values );
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const long long* values );

            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned short* values );
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned int* values );
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long* values );
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const unsigned long long* values );

            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const float* values );
            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const double* values );

            IGroupAtt::sptr putAtt( const std::string& key, size_t nvals, const char** values );
            



        protected:

            static GroupImpl::sptr create( GroupImpl::sptr p );

            NcGroupPtr _nc;	

            GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc );

            template< typename T >
                IGroupAtt::sptr putAttImpl( const std::string& key
                        , size_t nvals, const T* values );



        private:
            std::weak_ptr<GroupImpl> _parent;

            IGroup::NAME_GROUP_MAP _name_grp_map;
            IGroup::NAME_DIM_MAP _name_dim_map;
            IGroup::NAME_VAR_MAP _name_var_map;

            IGroup::NAME_ATT_MAP _name_att_map;

    };


    template< typename T >
        IGroupAtt::sptr GroupImpl::putAttImpl( const std::string& key, size_t nvals, const T* values )
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
            catch( NcException& e )
            {
                e.what();
            }


            return grp_att;
        }


    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const char** values )
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcString(), nvals, values );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& e )
        {
            e.what();
        }


        return grp_att;
    }


    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const double* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl< double >( key, nvals, values );
        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const float* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl< float >( key, nvals, values );
        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const short* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl< short >( key, nvals, values );
        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const int* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl< int >( key, nvals, values );
        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl< long >( key, nvals, values );
        return grp_att;
    }



    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const long long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl< long long >( key, nvals, values );
        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const unsigned short* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl( key, nvals, values );

        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const unsigned int* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl( key, nvals, values );
        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const unsigned long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl( key, nvals, values );
        return grp_att;
    }


    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, size_t nvals, const unsigned long long* values )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl( key, nvals, values );
        return grp_att;
    }


    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, float value )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl( key, 1, &value );
        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, int value )
    {
        IGroupAtt::sptr grp_att = this->putAttImpl( key, 1, &value );
        return grp_att;
    }


    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, const std::string& value )
    {
        IGroupAtt::sptr grp_att;
        try
        {
            netCDF::NcGroupAtt ncatt = _nc->putAtt( key, value );
            grp_att = GroupAtt::create( this->shared_from_this(), ncatt );

            _name_att_map[ key ] = grp_att;
        }
        catch( NcException& e )
        {
            e.what();
        }

        return grp_att;
    }

    IGroupAtt::sptr GroupImpl::getAtt( const std::string& key )
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
            catch( NcException& e )
            {
                std::cerr << e.what();
            }

        }
        return att_ptr;

    }

    std::vector< std::string > GroupImpl::getAtts()
    {
        std::vector< std::string > att_names;
        try
        {
            if( !_nc->isNull() )
            {
                std::multimap< std::string, netCDF::NcGroupAtt > ncatts = _nc->getAtts();
                for( auto it = ncatts.begin(); it != ncatts.end(); ++it )
                {
                    att_names.push_back( it->first );
                }
            }
        }
        catch( NcException& e ) 
        {
            std::cerr << e.what();
        }
        return att_names;

    }


    IVar::sptr GroupImpl::addVar( const std::string& name, DataType dataType, IVar::VDIMS& vdims )
    {
        VarImpl::sptr var_ptr;

        std::vector< netCDF::NcDim > ncdims;
        for( auto it = vdims.begin(); it != vdims.end(); ++it )
        {
            DimensionImpl::sptr dim_ptr = std::dynamic_pointer_cast< DimensionImpl >(*it);
            ncdims.push_back( dim_ptr->_nc_dim );
        }

        try
        {
            netCDF::NcType::ncType nctype = DataType2NcType( dataType );
            netCDF::NcVar ncvar = _nc->addVar( name, nctype, ncdims );
            var_ptr = VarImpl::create( this->shared_from_this(), name,  ncvar );
            _name_var_map[ name ] = var_ptr;
        }
        catch( NcException& e )
        {
            e.what();
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
        catch( NcException& e )
        {
            std::cerr << e.what();
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
            catch( NcException& e )
            {
                e.what();
            }
        }

        return var_ptr;
    }

    IDimension::sptr GroupImpl::getDim( const std::string& name )
    {
        IDimension::sptr dim_ptr;

        auto it = _name_dim_map.find( name );
        if( it != _name_dim_map.end() )
        {
            dim_ptr = it->second;
        }
        else
        {
            try
            {
                netCDF::NcDim nc_dim = _nc->getDim( name );

                if( !nc_dim.isNull() )
                {
                    dim_ptr = DimensionImpl::create( this->shared_from_this(), nc_dim );
                    _name_dim_map[ name ] = dim_ptr;
                }
            }
            catch( NcException& e )
            {
                e.what();
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
        catch( NcException& e )
        {
            e.what();
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
        catch( NcException& e )
        {
            e.what();
        }

        return dim_ptr;
    }

    int GroupImpl::getGroupCount()
    {
        return this->_nc->getGroupCount();
    }

    IGroup::sptr GroupImpl::getGroup( const std::string& name )
    {
        std::vector< std::string > splits;
        split_str( name, '/', splits );

        IGroup::sptr parent_grp_ptr = this->shared_from_this();
        IGroup::sptr grp_ptr;

        for( auto it = splits.begin(); it != splits.end(); ++it )
        {
            if( parent_grp_ptr )
            {
                grp_ptr = std::dynamic_pointer_cast< GroupImpl >( parent_grp_ptr)->_getGroup( *it );
                parent_grp_ptr = grp_ptr;
            }
        }
        return grp_ptr;
    }

    IGroup::sptr  GroupImpl::_getGroup( const std::string name )
    {
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
                    GroupImpl::sptr new_grp_ptr = GroupImpl::create( this->shared_from_this() );
                    new_grp_ptr->_nc = std::make_shared<NcGroup>(new_ncgrp);
                    grp_ptr = new_grp_ptr;
                    _name_grp_map[ name ] = grp_ptr;
                }
            }
            catch( NcException& e )
            {
                e.what();
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
        catch( NcException& e )
        {
            std::cerr << e.what();
        }

        return grp_names;
    }

    IGroup::sptr GroupImpl::requireGroup( const std::string& name )
    {
        IGroup::sptr grp_ptr = this->_getGroup( name );

        if( !grp_ptr )
            grp_ptr = this->addGroup( name );

        return grp_ptr;
    }

    IGroup::sptr GroupImpl::addGroup( const std::string& name )
    {
        GroupImpl::sptr grp_ptr;

        NcGroupPtr nc = std::dynamic_pointer_cast< NcGroup >(_nc);

        try
        {
            NcGroup new_ncgrp( nc->addGroup( name ) );

            if( !new_ncgrp.isNull() )
            {
                auto self_ptr = this->shared_from_this();

                GroupImpl::sptr new_grp_ptr = GroupImpl::create( self_ptr );

                new_grp_ptr->_nc =std::make_shared< NcGroup >( new_ncgrp) ;

                grp_ptr = new_grp_ptr;

                _name_grp_map[ name ] = grp_ptr;
            }
        }
        catch( NcException& e )
        {
            e.what();
        }

        return  grp_ptr;

    }

    GroupImpl::sptr GroupImpl::create( GroupImpl::sptr p )
    {
        auto _nc = std::make_shared< NcGroup >();
        auto pgrp = GroupImpl::sptr( new GroupImpl(p, _nc));
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
                    catch( NcException& e )
                    {
                        e.what();
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
        catch( NcException& e )
        {
            e.what();
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

    Builder::Builder( const std::string& path, IFile::Mode mode )
    {
        _file = FileFactory::create( path, mode );

        if( IFile::Mode::WRITE == mode )	
            _file->putAtt( "schema", 1 );
    }

    int Builder::readDim_metadata( const std::string& name
            , DimensionDefinition& dimdef )
    {
        int status = 0;
        std::string dim_name( name );
        std::string dim_name_orig = dim_name;

        IGroup::sptr grp_ptr = _file;

        std::vector< std::string > splits;
        split_str( dim_name, '/', splits );

        if( splits.size() > 1 )
        {
            auto its = splits.begin();
            auto ite = splits.end();
            for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
            {
                    grp_ptr = grp_ptr->getGroup( *it );
            }
            dim_name = splits.back();
        }

        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( dim_name );

            if( var_ptr )
            {
                std::vector< IDimension::sptr > dimdef_ptrs = var_ptr->getDims();

                auto dim_def_ptr = dimdef_ptrs[0];
                size_t n = dim_def_ptr->getSize();
                DataType dtype = var_ptr->getType();

                std::string units, description;
                IVarAtt::sptr att_units_ptr = var_ptr->getAtt( "units" );
                if( att_units_ptr )
                    att_units_ptr->getValues( units );

                IVarAtt::sptr att_desc_ptr = var_ptr->getAtt( "description" );
                if( att_desc_ptr )
                    att_desc_ptr->getValues( description );

                dimdef.dataType = dtype;
                dimdef.size = n;
                dimdef.units = units;
                dimdef.description = description;
            }
            else status = FDP_FILE_ERR;
        }
        else
            status = FDP_FILE_ERR;

        return status;
    }
    int Builder::readDim_data( const std::string& name, const DimensionDefinition& dimdef, void* data )
    {
        int status = 0;

        std::string dim_name_orig = name;
        std::string dim_name = dim_name_orig;

        IGroup::sptr grp_ptr = _file;

        std::vector< std::string > splits;
        split_str( dim_name_orig, '/', splits );

        if( splits.size() > 1 )
        {
            auto its = splits.begin();
            auto ite = splits.end();
            for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
            {
                    grp_ptr = grp_ptr->getGroup( *it );
            }
            dim_name = splits.back();
        }
        
        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( dim_name );

            if( var_ptr )
                var_ptr->getVar( data );
            else
                status = FDP_FILE_ERR;
        }
        else 
            status = FDP_FILE_ERR;

        return status;
    }



    int Builder::readArray_metadata( const std::string& name,  ArrayDefinition& arraydef )
    {
        int status = 0;
        std::string array_name( name );
        std::string array_name_orig = array_name;

        IGroup::sptr grp_ptr = _file;

        std::vector< std::string > splits;
        split_str( array_name, '/', splits );

        if( splits.size() > 1 )
        {
            auto its = splits.begin();
            auto ite = splits.end();
            for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
            {
                    grp_ptr = grp_ptr->getGroup( *it );
            }
            array_name = splits.back();
        }

        std::vector< size_t > shape;

        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( array_name );

            if( var_ptr )
            {
                std::vector< std::string > dim_names;

                size_t n = 1;
                auto dims = var_ptr->getDims();
                for( int i = 0; i < dims.size(); ++i )
                {
                    IDimension::sptr dim_ptr = dims[i];

                    n = n * dim_ptr->getSize();
                    const std::string& dim_name = dim_ptr->getName();

                    size_t size = dim_ptr->getSize();
                    shape.push_back( size );

                    std::string dim_var_name( dim_name.begin(), dim_name.end()/*-4*/ );
                    IVar::sptr dim_var_ptr = grp_ptr->getVar( dim_var_name );

                    if( dim_var_ptr )
                    {
                        IGroup::sptr parent_grp_ptr = dim_var_ptr->parent();
                        while( parent_grp_ptr && parent_grp_ptr->getName() != "/" )
                        {
                            dim_var_name = parent_grp_ptr->getName() + "/" +dim_var_name;

                            parent_grp_ptr = parent_grp_ptr->parent();
                        }
                    }


                    if( dim_var_ptr )
                        dim_names.push_back( dim_var_name );
                    else
                        dim_names.push_back( "" );
                }

                DataType dtype =  var_ptr->getType();

                std::string units, description;

                IVarAtt::sptr att_units_ptr = var_ptr->getAtt( "units" );
                if( att_units_ptr )
                    att_units_ptr->getValues( units );

                IVarAtt::sptr att_desc_ptr = var_ptr->getAtt( "description" );
                if( att_desc_ptr )
                    att_desc_ptr->getValues( description );

                arraydef.dataType = dtype;
                arraydef.dimension_names = dim_names;
                //arraydef.shape = shape;
                arraydef.units = units;
                arraydef.description = description;
            }
            else
                status = FDP_FILE_ERR;
        }
        else
            status = FDP_FILE_ERR;

        return status;
    }
    int Builder::readArray_data( const std::string& name, const ArrayDefinition& arraydef, void* data )
    {
        int status = 0;
        std::string array_name_orig = name;
        std::string array_name = array_name_orig;

        IGroup::sptr grp_ptr = _file;

        std::vector< std::string > splits;
        split_str( array_name_orig, '/', splits );

        if( splits.size() > 1 )
        {
            auto its = splits.begin();
            auto ite = splits.end();
            for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
            {
                    grp_ptr = grp_ptr->getGroup( *it );
            }
            array_name = splits.back();
        }

        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( array_name );
            if( var_ptr )
                var_ptr->getVar( data );
            else
                status = FDP_FILE_ERR;
        }
        else
            status = FDP_FILE_ERR;

        return status;
    }

    int Builder::writeDimension( const std::string& name, const DimensionDefinition& dimdef, const void* data )
    {
        int status = 0;

        std::string dimdef_name = name;

        IGroup::sptr grp_ptr = _file;
        
        std::vector< std::string > splits;

        split_str( name, '/', splits );

        if( splits.size() > 1 )
        {
            auto its = splits.begin();
            auto ite = splits.end();
            for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
            {
                    grp_ptr = grp_ptr->requireGroup( *it );
            }
            dimdef_name = splits.back();
        }
        
        IVar::sptr dim_var_ptr = grp_ptr->getVar( dimdef_name );
        if( !dim_var_ptr )
        {
            IDimension::sptr dim_ptr = grp_ptr->addDim( dimdef_name /*+ "_dim"*/, dimdef.size );

            std::vector< IDimension::sptr > vdims = {dim_ptr};

            dim_var_ptr = grp_ptr->addVar( dimdef_name, dimdef.dataType, vdims );

            if( data )
                dim_var_ptr->putVar( data );

            dim_var_ptr->putAtt( "units", dimdef.units );
            dim_var_ptr->putAtt( "description", dimdef.description );
        }
        else
            status = FDP_FILE_ERR;

        return status;
    }

    int Builder::writeArray( const std::string& name, const ArrayDefinition& arraydef, const void* data )
    {
        int status = 0;
        
        std::vector< IDimension::sptr > vdims;

        IGroup::sptr grp_ptr;

        for( int i = 0; i < arraydef.dimension_names.size();++i )
        {
            grp_ptr = _file;
            std::string dim_name = arraydef.dimension_names[i];
            std::vector< std::string > splits;
            split_str( dim_name, '/', splits );

            if( splits.size() > 1 )
            {
                auto its = splits.begin();
                auto ite = splits.end();
                for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
                {
                    grp_ptr = grp_ptr->getGroup( *it );
                }
                dim_name = splits.back();
            }

            IVar::sptr dim_var_ptr = grp_ptr->getVar( dim_name );
            if( dim_var_ptr )
            {
                std::vector< IDimension::sptr > dim_ptrs = dim_var_ptr->getDims();

                vdims.push_back( dim_ptrs[0] );
            }
            else 
                status = FDP_FILE_ERR;
        }                    

        if( !status )
        {
            std::vector< std::string > splits;
            split_str( name, '/', splits );
            std::string arrname = name;
            grp_ptr = _file;

            if( splits.size() > 1 )
            {
                auto its = splits.begin();
                auto ite = splits.end();
                for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
                {
                    grp_ptr = grp_ptr->requireGroup( *it );
                }
                arrname = splits.back();
            }

            IVar::sptr arr_var_ptr = grp_ptr->getVar( arrname );
            if( !arr_var_ptr )
            {
                arr_var_ptr = grp_ptr->addVar( arrname, arraydef.dataType, vdims );
                if( data )
                    arr_var_ptr->putVar( data );

                arr_var_ptr->putAtt( "units", arraydef.units );
                arr_var_ptr->putAtt( "description", arraydef.description );
            }
            else
                status = FDP_FILE_ERR;
        }

        return status;
    }


    template< typename T >
    int Builder_putAttImpl( IGroup::sptr grp_ptr, const std::string& name
            , const std::string& key, size_t nvals, T* values  )
    {
        int status = 0;

        std::vector< std::string > splits;
        split_str( name, '/', splits );
        std::string arrname = name;

        auto its = splits.begin();
        auto ite = splits.end();
        for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
        {
            grp_ptr = grp_ptr->getGroup( *it );
        }

        if( grp_ptr )
        {
            IGroup::sptr last_grp_ptr = grp_ptr;

            //last split could be a var or a group
            grp_ptr = grp_ptr->getGroup( splits.back() );
            if( grp_ptr )
            {
                grp_ptr->putAtt( key, nvals, values );
            }
            else
            {
                //could be a var
                auto var_ptr = last_grp_ptr->getVar( splits.back() );

                if( var_ptr )
                    var_ptr->putAtt( key, nvals, values );
                else
                    status = FDP_FILE_ERR;
            }
        }
        else
            status = FDP_FILE_ERR;

        return status;
    }

    int Builder_putAttImpl_strings( IGroup::sptr grp_ptr
            , const std::string& name
            , const std::string& key, size_t nvals, const char** values  )
    {
        int status = 0;

        std::vector< std::string > splits;
        split_str( name, '/', splits );
        std::string arrname = name;

        auto its = splits.begin();
        auto ite = splits.end();
        for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
        {
            grp_ptr = grp_ptr->getGroup( *it );
        }

        if( grp_ptr )
        {
            //last split could be a var or a group
            grp_ptr = grp_ptr->getGroup( splits.back() );
            if( grp_ptr )
            {
                grp_ptr->putAtt( key, nvals, values );
            }
            else
            {
                //could be a var
                auto var_ptr = grp_ptr->getVar( splits.back() );

                if( var_ptr )
                    var_ptr->putAtt( key, nvals, values );
                else
                    status = FDP_FILE_ERR;
            }
        }
        else
            status = FDP_FILE_ERR;

        return status;
    }


    int Builder::putAtt( const std::string& name
            , const std::string& key, size_t nvals, const short* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name
            , const std::string& key, size_t nvals, const int* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name
            , const std::string& key, size_t nvals, const long* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name
            , const std::string& key, size_t nvals, const long long* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name, const std::string& key, size_t nvals, const unsigned short* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name, const std::string& key, size_t nvals,  const unsigned int* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name, const std::string& key, size_t nvals,  const unsigned long* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name, const std::string& key, size_t nvals,  const unsigned long long* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name, const std::string& key, size_t nvals,  const double * values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name, const std::string& key, size_t nvals,  const float* values  )
    {
        return Builder_putAttImpl( _file, name, key, nvals, values );
    }

    int Builder::putAtt( const std::string& name, const std::string& key, size_t nvals,  const char** values )
    {
        return Builder_putAttImpl_strings( _file, name, key, nvals, values );
    }


    int Builder::getAtt( const std::string& name, const std::string& key, IAtt::sptr& att_ptr )
    {
        int status = 0;

        std::vector< std::string > splits;
        split_str( name, '/', splits );
        std::string arrname = name;

        auto its = splits.begin();
        auto ite = splits.end();

        att_ptr = NULL;

        IGroup::sptr grp_ptr = _file;

        for( auto it = splits.begin(); it != ite-1 && grp_ptr; ++it )
        {
            grp_ptr = grp_ptr->getGroup( *it );
        }

        if( grp_ptr )
        {
            IGroup::sptr last_grp_ptr = grp_ptr;

            //last split could be a var or a group
            grp_ptr = grp_ptr->getGroup( splits.back() );
            if( grp_ptr )
            {
                IGroupAtt::sptr grp_att_ptr = grp_ptr->getAtt( key );

                att_ptr = grp_att_ptr;

                status = (att_ptr != NULL) ? FDP_FILE_NOERR : FDP_FILE_ERR;
            }
            else
            {
                //could be a var
                auto var_ptr = last_grp_ptr->getVar( splits.back() );

                if( var_ptr )
                {
                    IVarAtt::sptr var_att_ptr =  var_ptr->getAtt( key );

                    att_ptr = var_att_ptr;

                    status = (att_ptr != NULL) ? FDP_FILE_NOERR : FDP_FILE_ERR;
                }
                else
                    status = FDP_FILE_ERR;
            }
        }
        else
            status = FDP_FILE_ERR;

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


