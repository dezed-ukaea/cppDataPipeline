#include<map>

#include "fdp/fdpnetcdf.hxx"
#include "fdp/fdpnetcdf2.hxx"



#include <netcdf>

typedef std::shared_ptr< netCDF::NcGroup > NcGroupPtr;
typedef std::shared_ptr< netCDF::NcFile > NcFilePtr;

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
struct NcTraits< unsigned long >
{
	typedef netCDF::NcUint64 MyNcType;
};

template<>
struct NcTraits< unsigned long long >
{
	typedef netCDF::NcUint64 MyNcType;
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



namespace FairDataPipeline
{
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


	void split_str( const std::string& s, const char delim, std::vector< std::string >& splits ) ;


	class Group2;
	typedef std::shared_ptr< Group2 > Group2Sptr;

	class GroupAtt2 : std::enable_shared_from_this< GroupAtt2 >, public IGroupAtt2
	{
		public:
			typedef std::shared_ptr< GroupAtt2 > sptr;

			static sptr create( Group2Sptr parent_grp_ptr, const netCDF::NcGroupAtt& nc_att );

            DataType getAttType();

			size_t getAttLength();
			IGroup2Sptr getParentGroup();
			Group2Sptr _getParentGroup();
			std::string name();

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


		private:
			GroupAtt2( Group2Sptr parent_grp_ptr, const netCDF::NcGroupAtt& nc_at );
			netCDF::NcGroupAtt _nc;
			std::weak_ptr< Group2 > _parent_grp_ptr;

			template< typename T > 
				int getValuesImpl( T* values );

	};

	template< typename T > 
		int GroupAtt2::getValuesImpl( T* values )
		{
			int status = 0;

			try
			{
				_nc.getValues( values );
			}
			catch( netCDF::exceptions::NcException& e )
			{
				e.what();
				status = 1;
			}

			return status;
		}

	int GroupAtt2::getValues( short* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( int* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( long* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( long long* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( unsigned short* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( unsigned int* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( unsigned long* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( unsigned long long* values )
	{
		return this->getValuesImpl( values );
	}


	int GroupAtt2::getValues( float* values )
	{
		return this->getValuesImpl( values );
	}

	int GroupAtt2::getValues( double* values )
	{
		return this->getValuesImpl( values );
	}

	std::string GroupAtt2::name()
	{
		return _nc.getName();
	}

	size_t GroupAtt2::getAttLength()
	{
		return _nc.getAttLength();
	}

	GroupAtt2::GroupAtt2( Group2Sptr parent_grp_ptr, const netCDF::NcGroupAtt& nc_att ) : _nc( nc_att ) , _parent_grp_ptr( parent_grp_ptr )
	{
	}

	GroupAtt2::sptr GroupAtt2::create( Group2Sptr parent_grp_ptr, const netCDF::NcGroupAtt& nc_att )
	{
		return sptr( new GroupAtt2 ( parent_grp_ptr, nc_att ) );
	}

	class Group2 : public std::enable_shared_from_this< Group2 >,  public IGroup2
	{
		public:

			typedef std::shared_ptr< Group2 > sptr;
            typedef std::map< std::string, sptr > NAME_GROUP_MAP; 
			typedef std::map< std::string, GroupAtt2::sptr > NAME_ATTRIB_MAP;

			static sptr create( sptr grp_parent, NcGroupPtr nc_grp_ptr	);

			//IGroup2::sptr addGroup( const std::string& grp_name );
			IGroup2::sptr getGroup( const std::string& grp_name );
			IGroup2::sptr requireGroup( const std::string& grp_name );

			IGroup2::sptr parent();

			std::string name();

            std::vector< IGroupAtt2::sptr > getAttributes();

			IGroupAtt2::sptr getAtt( const std::string& key );
			GroupAtt2::sptr _getAtt( const std::string& key );

            IGroupAtt2::sptr putAtt( const std::string& key, const std::string& value );

            IGroupAtt2::sptr putAtt( const std::string& key, float value );

            IGroupAtt2::sptr putAtt( const std::string& key, short value );
            IGroupAtt2::sptr putAtt( const std::string& key, int value );
            IGroupAtt2::sptr putAtt( const std::string& key, long value );
            IGroupAtt2::sptr putAtt( const std::string& key, long long value );

            IGroupAtt2::sptr putAtt( const std::string& key, unsigned short value );
            IGroupAtt2::sptr putAtt( const std::string& key, unsigned int value );
            //IGroupAtt2::sptr putAtt( const std::string& key, unsigned long value );
            IGroupAtt2::sptr putAtt( const std::string& key, unsigned long long value );


			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const short* vals );
			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const int* vals );
			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const long* vals );
			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const long long* vals );

			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned short* vals );
			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned int* vals );
			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned long* vals );
			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned long long* vals );

			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const float* vals );
			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const double* vals );

			IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const char** vals );


		protected:

			Group2( Group2::sptr grp_parent, NcGroupPtr _nc_grp );

			template< typename T >
				GroupAtt2::sptr putAttImpl( const std::string& key, size_t nvals, const T* vals );

			NcGroupPtr _nc_ptr;

		private:
            template< typename T > 
                IGroupAtt2::sptr _putAttImpl( const std::string& key, const T& value );


			sptr _addGroup( const std::string& grp_name );

			sptr _requireGroup( const std::string& grp_name );

			std::weak_ptr< Group2 > _parent_grp_ptr;

			sptr _getGroup( const std::string& name );


			NAME_GROUP_MAP _name_grp_map; 
			NAME_ATTRIB_MAP _name_attrib_map;
	};


    template<>
        IGroupAtt2::sptr Group2::_putAttImpl< std::string >( const std::string& key, const std::string& value )
        {
            GroupAtt2::sptr grp_att_ptr;

            try
            {
                netCDF::NcGroupAtt ncatt = _nc_ptr->putAtt( key, value );
                if( !ncatt.isNull() )
                {
                    grp_att_ptr = GroupAtt2::create( this->shared_from_this(), ncatt );

                }
            }
            catch( netCDF::exceptions::NcException& e )
            {
                std::cerr << e.what();
            }

            return grp_att_ptr;
        }

    template< typename T > 
        IGroupAtt2::sptr Group2::_putAttImpl( const std::string& key, const T& value )
    {
        GroupAtt2::sptr grp_att_ptr;
        try
        {
			typedef typename NcTraits< T >::MyNcType MyNcType;
			MyNcType nctype = MyNcType();
            netCDF::NcGroupAtt ncatt = _nc_ptr->putAtt( key, nctype, value );

            if( !ncatt.isNull() )
                grp_att_ptr = GroupAtt2::create( this->shared_from_this(), ncatt );
        }
        catch( netCDF::exceptions::NcException& e )
        {
            std::cerr << e.what();
        }
        return grp_att_ptr;
    }

    IGroupAtt2::sptr Group2::putAtt( const std::string& key, const std::string& value )
    {
        return this->_putAttImpl< std::string >( key, value );
    }

    IGroupAtt2::sptr Group2::putAtt( const std::string& key, float value )
    {
        return this->_putAttImpl( key, value );
    }



    IGroupAtt2::sptr Group2::putAtt( const std::string& key, short value )
    {
        return this->_putAttImpl( key, value );
    }
#if 1
    IGroupAtt2::sptr Group2::putAtt( const std::string& key, long value )
    {
        return this->_putAttImpl( key, value );
    }

    IGroupAtt2::sptr Group2::putAtt( const std::string& key, long long value )
    {
        return this->_putAttImpl( key, value );
    }

    IGroupAtt2::sptr Group2::putAtt( const std::string& key, unsigned short value )
    {
        return this->_putAttImpl( key, value );
    }

#endif        
    IGroupAtt2::sptr Group2::putAtt( const std::string& key, unsigned int value )
    {
        return this->_putAttImpl( key, value );
    }



#if 0
    IGroupAtt2::sptr Group2::putAtt( const std::string& key, unsigned long value )
    {
        return this->_putAttImpl( key, value );
    }
#endif
    IGroupAtt2::sptr Group2::putAtt( const std::string& key, unsigned long long value )
    {
        return this->_putAttImpl( key, value );
    }




    IGroupAtt2::sptr Group2::putAtt( const std::string& key, int value )
    {
        return this->_putAttImpl( key, value );
    }

    std::vector< IGroupAtt2::sptr > Group2::getAttributes()
    {
        std::vector< IGroupAtt2::sptr > v;

        try
        {
            std::multimap< std::string, netCDF::NcGroupAtt > nc_atts = _nc_ptr->getAtts();

            for( auto it = nc_atts.begin(); it != nc_atts.end(); ++it )
            {
                IGroupAtt2::sptr grp_att_ptr = this->getAtt( it->first );
                v.push_back( grp_att_ptr );
            }
        }
        catch( netCDF::exceptions::NcException& e )
        {
        }

        return v;
    }

    IGroupAtt2::sptr Group2::getAtt( const std::string& key )
    {
        return _getAtt( key );
    }

	GroupAtt2::sptr Group2::_getAtt( const std::string& key )
	{
		GroupAtt2::sptr att_ptr;
		auto it = _name_attrib_map.find( key );
		if( it != _name_attrib_map.end() )
		{
			att_ptr = it->second;
		}
		else
		{
			try
			{
				netCDF::NcGroupAtt ncatt = _nc_ptr->getAtt( key );

                if( !ncatt.isNull() )
                {
                    att_ptr = GroupAtt2::create( this->shared_from_this(), ncatt );
                    _name_attrib_map[ key ] = att_ptr;
                }
			}
			catch( netCDF::exceptions::NcException& e )
			{
				e.what();
			}

		}
		return att_ptr;
	}

    DataType GroupAtt2::getAttType()
    {
        auto nctype = _nc.getType().getTypeClass();

        DataType dtype = NcType2DataType( nctype );
        return dtype;
    }

	template< typename T >
	GroupAtt2::sptr Group2::putAttImpl( const std::string& key, size_t nvals, const T* values )
	{
		GroupAtt2::sptr grp_att;
		try
		{
			typedef typename NcTraits< T >::MyNcType MyNcType;

			MyNcType theNcType = MyNcType();

			netCDF::NcGroupAtt ncatt = _nc_ptr->putAtt( key, theNcType, nvals, values );
			grp_att = GroupAtt2::create( this->shared_from_this(), ncatt );

			_name_attrib_map[ key ] = grp_att;
		}
		catch( netCDF::exceptions::NcException& e )
		{
			e.what();
		}


		return grp_att;
	}

    IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const char** values )
	{
		GroupAtt2::sptr grp_att_ptr;
       try
       {
           netCDF::NcGroupAtt ncatt = _nc_ptr->putAtt( key, netCDF::NcString(), nvals, values );
           if( !ncatt.isNull() )
           {
               grp_att_ptr = GroupAtt2::create( this->shared_from_this(), ncatt );
               _name_attrib_map[ key ] = grp_att_ptr;
           }
       
       }
       catch( netCDF::exceptions::NcException& e )
       {
           std::cerr << e.what();
       }
		return grp_att_ptr;
	}

	
	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const double* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl< double >( key, nvals, values );
		return grp_att;
	}

	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const float* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl< float >( key, nvals, values );
		return grp_att;
	}

	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const short* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl< short >( key, nvals, values );
		return grp_att;
	}
	
	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const int* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl< int >( key, nvals, values );
		return grp_att;
	}

	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const long* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl< long >( key, nvals, values );
		return grp_att;
	}

	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const long long* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl< long long >( key, nvals, values );
		return grp_att;
	}

	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const unsigned short* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl( key, nvals, values );

		return grp_att;
	}

	IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const unsigned int* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl( key, nvals, values );
		return grp_att;
	}

    IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const unsigned long* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl( key, nvals, values );
		return grp_att;
	}

    IGroupAtt2::sptr Group2::putAtt( const std::string& key, size_t nvals, const unsigned long long* values )
	{
		GroupAtt2::sptr grp_att = this->putAttImpl( key, nvals, values );
		return grp_att;
	}


	
	std::string Group2::name()
	{
		return _nc_ptr->getName();
	}

	IGroup2::sptr Group2::parent()
	{
		return _parent_grp_ptr.lock();
	}

	Group2::Group2( Group2::sptr grp_parent
			, NcGroupPtr nc_grp_ptr ) : _parent_grp_ptr( grp_parent ), _nc_ptr( nc_grp_ptr )
	{
	//	_nc_ptr = nc_grp_ptr;
	//	_parent_grp_ptr = grp_parent;
    //    grp_parent->_name_grp_map[ _nc_ptr->getName() ];
	}

	Group2::sptr Group2::create( Group2::sptr grp_parent
		       , NcGroupPtr nc_grp_ptr
	       	       )
	{
		Group2::sptr grp_ptr( new Group2( grp_parent, nc_grp_ptr ) );

		return grp_ptr;
	}

	Group2::sptr Group2::_requireGroup( const std::string& grp_name )
	{
		sptr grp_ptr = this->_getGroup( grp_name );

		if( !grp_ptr )
			grp_ptr = /*std::static_pointer_cast< Group2 >*/( this->_addGroup( grp_name ) );

		return grp_ptr;
	}

	IGroup2::sptr Group2::requireGroup( const std::string& grp_name )
	{
		std::vector< std::string > splits;
		split_str( grp_name, '/', splits );

        sptr parent_grp_ptr = this->shared_from_this();
        sptr grp_ptr;

        for( auto it = splits.begin(); it != splits.end(); ++it )
        {

			if( parent_grp_ptr )
            {
				grp_ptr = parent_grp_ptr->_requireGroup( *it );
				parent_grp_ptr = grp_ptr;
            }
        }

		return grp_ptr;
	}

	IGroup2::sptr Group2::getGroup( const std::string& grp_name )
	{
		sptr grp_ptr;

		std::vector< std::string > splits;
		split_str( grp_name, '/', splits );

        sptr parent_grp_ptr = this->shared_from_this();

        for( auto it = splits.begin(); it != splits.end(); ++it )
		{
			if( parent_grp_ptr )
			{
				grp_ptr = parent_grp_ptr->_getGroup( *it );
				parent_grp_ptr = grp_ptr;
			}
		}
		return grp_ptr;
	}

	Group2::sptr Group2::_getGroup( const std::string& name )
	{
		sptr grp_ptr;
		auto it = this->_name_grp_map.find( name );
		if( it != this->_name_grp_map.end() )
		{
			grp_ptr = std::static_pointer_cast< Group2 >( it->second );
		}
		else
		{
			try
			{
				netCDF::NcGroup new_ncgrp = _nc_ptr->getGroup( name );
				if( !new_ncgrp.isNull() )
				{
					NcGroupPtr nc_grp_ptr = std::make_shared< netCDF::NcGroup >( new_ncgrp );
					grp_ptr = Group2::create( this->shared_from_this(), nc_grp_ptr );
					_name_grp_map[ name ] = grp_ptr;
				}
			}
			catch( netCDF::exceptions::NcException& e )
			{
				e.what();
			}
		}
		return grp_ptr;
	}

	Group2::sptr Group2::_addGroup( const std::string& grp_name )
	{
		sptr grp_ptr;

		try
		{
			netCDF::NcGroup new_ncgrp( _nc_ptr->addGroup( grp_name ) );
			if( !new_ncgrp.isNull() )
			{
				NcGroupPtr nc_grp_ptr = std::make_shared< netCDF::NcGroup >( new_ncgrp );
				grp_ptr = Group2::create( this->shared_from_this(), nc_grp_ptr ); 
                _name_grp_map[ grp_name ] = grp_ptr;

			}
		}
		catch( netCDF::exceptions::NcException& e )
		{
            std::cerr << e.what();
		}

		return grp_ptr;
	}
#if 0
	IGroup2::sptr Group2::addGroup( const std::string& grp_name )
	{
		std::vector< std::string > splits;
		split_str( grp_name, '/', splits );

        sptr parent_grp_ptr = this->shared_from_this();
        sptr new_grp_ptr;

        for( auto it = splits.begin(); !exist && it != splits.end(); ++it )
		{
			if( parent_grp_ptr )
			{
				sptr grp_ptr = parent_grp_ptr->_getGroup( *it );
                if( sptr )
                    exist = true;
           
				parent_grp_ptr = grp_ptr;
			}
		}

		//Group2::sptr grp_ptr = this->_addGroup( grp_name );
		return new_grp_ptr;
	}
#endif

	class File2 : public Group2//, public IFile2
	{
		public:
			typedef std::shared_ptr< File2 > sptr;
			static File2::sptr create( const std::string& path, enum IFile2::Mode mode);
;

			~File2();

			//void build();

			//void prepare_coordinate_variable();
			//void prepare_table_definition();
			//void prepare_dimension_variable_definition();


		private:

			static const std::string ATTRIB_DESC;
			static const std::string ATTRIB_LNAME;
			static const std::string ATTRIB_FILLVALUE;
			static const std::string ATTRIB_UNITS;
			static const std::string ATTRIB_GROUP_TYPE;

			File2( const std::string& path, IFile2::Mode mode );
			bool _hasBeenBuilt;
			//netCDF::NcFile _nc;
	};

	const std::string File2::ATTRIB_DESC = "description";
	const std::string File2::ATTRIB_LNAME = "long_name";
	const std::string File2::ATTRIB_FILLVALUE = "_FillValue";
	const std::string File2::ATTRIB_UNITS= "units";
	const std::string File2::ATTRIB_GROUP_TYPE= "group_type";

#if 0
	void File2::prepare_coordinate_variable()
	{
	}

	void File2::prepare_table_definition()
	{
	}

	void File2::prepare_dimension_variable_definition()
	{
	}
#endif
#if 0
	Group2::sptr File2::getGroup( const std::string& grp_path )
	{
		std::vector< std::string > splits;
		split_str( grp_path, '/', splits );

		Group2::sptr grp_ptr;
		Group2::sptr parent_grp_ptr = this->shared_from_this();

		//Group2::sptr parent_grp_ptr = Group2::create( NULL, "/" );
		//parent_grp_ptr->_nc = _nc_ptr;

		for( auto it = splits.begin(); it != splits.end(); ++it )
		{
			if( parent_grp_ptr )
			{
				grp_ptr = parent_grp_ptr->getGroup( *it );
				parent_grp_ptr = grp_ptr;
			}
		}
		return grp_ptr;
	}

	Group2::sptr File2::requireGroup( const std::string& grp_path )
	{
		std::vector< std::string > splits;
		split_str( grp_path, '/', splits );

		Group2::sptr grp_ptr;
#if 0
		Group2::sptr parent_grp_ptr = this->shared_from_this();

		for( auto it = splits.begin(); it != splits.end(); ++it )
		{
			if( parent_grp_ptr )
			{
				grp_ptr = parent_grp_ptr->_getGroup( *it );
				parent_grp_ptr = grp_ptr;
			}
			else
			{
				grp_ptr = parent_grp_ptr->addGroup( *it );
				parent_grp_ptr = grp_ptr;
			}
		}
#endif
		return grp_ptr;

	}

	Group2::sptr File2::addGroup( const std::string& grp_path )
	{
		//Group2::sptr grp_ptr = Group2::create( this->shared_from_this(), grp_path  );
		return NULL;
	}
#endif
	File2::sptr File2::create(const std::string& path, enum IFile2::Mode mode )
	{
		sptr p = File2::sptr( new File2(path, mode) );
		return p;
	}

	File2::File2(const std::string& path, enum IFile2::Mode mode ) : Group2( NULL, std::make_shared< netCDF::NcFile >() )
	{
		_hasBeenBuilt = false;

        auto nc_file_ptr = std::dynamic_pointer_cast< netCDF::NcFile >( _nc_ptr );

		switch( mode )
		{
			case IFile2::Mode::READ:
				try
				{
					nc_file_ptr->open( path, netCDF::NcFile::FileMode::read );
				}
				catch( netCDF::exceptions::NcException& e )
				{
					e.what();
				}
				break;
			case IFile2::Mode::WRITE:
				try
				{
					nc_file_ptr->open( path, netCDF::NcFile::FileMode::newFile );
				}
				catch( netCDF::exceptions::NcException& e )
				{
					nc_file_ptr->open( path, netCDF::NcFile::FileMode::write );
				}
				break;
			default:
				break;

		}
	}

	File2::~File2()
	{
		try
		{
			//_nc_ptr->close();
		}
		catch( netCDF::exceptions::NcException& e )
		{
			e.what();
		}

	}

	void build()
	{
	}

    Group2::sptr GroupAtt2::_getParentGroup()
	{
		return _parent_grp_ptr.lock();
	}

	IGroup2Sptr GroupAtt2::getParentGroup()
	{
        Group2::sptr grp_ptr = _getParentGroup();
		return grp_ptr;
	}


    IFile2::sptr FileFactory2::create( const std::string& path, IFile2::Mode mode )
    {
        File2::sptr ptr = File2::create( path, mode );
        return  ptr;
    }
	

}
