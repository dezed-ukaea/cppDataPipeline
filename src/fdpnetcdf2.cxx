#include <memory>
#include<string>
#include<vector>
#include<map>



#include <netcdf>

typedef std::shared_ptr< netCDF::NcGroup > NcGroupPtr;
typedef std::shared_ptr< netCDF::NcFile > NcFilePtr;



namespace FairdataPipeline
{
	void split_str( const std::string& s, const char delim, std::vector< std::string >& splits ) ;
	class IGroup2
	{
		public:
			typedef std::shared_ptr< IGroup2 > sptr;
	};

	class IFile2
	{
		public:
			enum Mode{ READ, WRITE };
			typedef std::shared_ptr< IFile2 > sptr;


	};

	//class File2;

	class Group2: public std::enable_shared_from_this< Group2 >,  public IGroup2
	{
		public:

			//friend File2;
			typedef std::shared_ptr< Group2 > sptr;

			typedef std::map< std::string, sptr > NAME_GROUP_MAP; 

			sptr addGroup( const std::string& grp_name );
			sptr getGroup( const std::string& grp_name );
			sptr requireGroup( const std::string& grp_name );

			static sptr create( sptr grp_parent
				       , NcGroupPtr nc_grp_ptr	);
			sptr parent();

			std::string name();
		protected:

			Group2( Group2::sptr grp_parent
					, NcGroupPtr _nc_grp );

		private:
			std::weak_ptr< Group2 > _parent_grp_ptr;

			sptr _getGroup( const std::string& name );

			//netCDF::NcGroup _nc;
			NcGroupPtr _nc_ptr;

			NAME_GROUP_MAP _name_grp_map; 
	};

	std::string Group2::name()
	{
		return _nc_ptr->getName();
	}

	Group2::sptr Group2::parent()
	{
		return _parent_grp_ptr.lock();
	}

	Group2::Group2( Group2::sptr grp_parent
			, NcGroupPtr nc_grp_ptr )
	{
		_nc_ptr = nc_grp_ptr;
		_parent_grp_ptr = grp_parent;
	}

	Group2::sptr Group2::create( Group2::sptr grp_parent
		       , NcGroupPtr nc_grp_ptr
	       	       )
	{
		Group2::sptr grp_ptr( new Group2( grp_parent, nc_grp_ptr ) );
		return grp_ptr;
	}

	Group2::sptr Group2::requireGroup( const std::string& grp_name )
	{
		sptr grp_ptr = this->_getGroup( grp_name );

		if( !grp_ptr )
			grp_ptr = this->addGroup( grp_name );

		return grp_ptr;
	}

	Group2::sptr Group2::getGroup( const std::string& grp_name )
	{
		Group2::sptr grp_ptr;
#if 1
		grp_ptr = this->_getGroup( grp_name );
#else

		std::vector< std::string > splits;
		split_str( grp_name, '/', splits );

		Group2::sptr parent_grp_ptr = this->shared_from_this();

		for( auto it = splits.begin(); it != splits.end(); ++it )
		{
			if( parent_grp_ptr )
			{
				grp_ptr = parent_grp_ptr->_getGroup( *it );
				parent_grp_ptr = grp_ptr;
			}
		}
#endif
		return grp_ptr;
	}

	Group2::sptr Group2::_getGroup( const std::string& name )
	{
		Group2::sptr grp_ptr;
		auto it = this->_name_grp_map.find( name );
		if( it != this->_name_grp_map.end() )
		{
			grp_ptr = it->second;
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

	Group2::sptr Group2::addGroup( const std::string& grp_name )
	{
		Group2::sptr grp_ptr;

		try
		{
			netCDF::NcGroup new_ncgrp = _nc_ptr->addGroup( grp_name );
			if( !new_ncgrp.isNull() )
			{
				NcGroupPtr nc_grp_ptr = std::make_shared< netCDF::NcGroup >( new_ncgrp );
				grp_ptr = Group2::create( this->shared_from_this(), nc_grp_ptr ); 
			}
		}
		catch( netCDF::exceptions::NcException& e )
		{
		}
		return grp_ptr;
	}


	class File2 : public IFile2, public Group2
	{
		public:
			typedef std::shared_ptr< File2 > sptr;
			static sptr create( const std::string& path, enum IFile2::Mode mode);
;

			~File2();

			void build();

			void prepare_coordinate_variable();
			void prepare_table_definition();
			void prepare_dimension_variable_definition();

			//Group2::sptr getGroup( const std::string& grp_path );

			//Group2::sptr addGroup( const std::string& grp_path );
			//Group2::sptr requireGroup( const std::string& grp_path );

		private:

			static const std::string ATTRIB_DESC;
			static const std::string ATTRIB_LNAME;
			static const std::string ATTRIB_FILLVALUE;
			static const std::string ATTRIB_UNITS;
			static const std::string ATTRIB_GROUP_TYPE;

			File2( const std::string& path, IFile2::Mode mode );
			bool _hasBeenBuilt;
			netCDF::NcFile _nc;
	};

	const std::string File2::ATTRIB_DESC = "description";
	const std::string File2::ATTRIB_LNAME = "long_name";
	const std::string File2::ATTRIB_FILLVALUE = "_FillValue";
	const std::string File2::ATTRIB_UNITS= "units";
	const std::string File2::ATTRIB_GROUP_TYPE= "group_type";


	void File2::prepare_coordinate_variable()
	{
	}

	void File2::prepare_table_definition()
	{
	}

	void File2::prepare_dimension_variable_definition()
	{
	}
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
		sptr p = sptr( new File2(path, mode) );
		return p;
	}

	File2::File2(const std::string& path, enum IFile2::Mode mode ) : Group2( NULL, std::make_shared< netCDF::NcFile >() )
	{
		_hasBeenBuilt = false;

		switch( mode )
		{
			case IFile2::Mode::READ:
				try
				{
					_nc.open( path, netCDF::NcFile::FileMode::read );
				}
				catch( netCDF::exceptions::NcException& e )
				{
					e.what();
				}
				break;
			case IFile2::Mode::WRITE:
				try
				{
					_nc.open( path, netCDF::NcFile::FileMode::newFile );
				}
				catch( netCDF::exceptions::NcException& e )
				{
					_nc.open( path, netCDF::NcFile::FileMode::write );
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
			_nc.close();
		}
		catch( netCDF::exceptions::NcException& e )
		{
			e.what();
		}

	}

	void build()
	{
	}


	

}
