#include "fdpnetcdf.hxx"

#include<netcdf>

namespace fdp
{

	int DimensionDefinition::UNLIMITED = 0;

	DimensionDefinition::DimensionDefinition( const std::string& name, int datatype, const std::string& description, size_t size, const std::string& units ) :
		_name( name )
		, _datatype( datatype )
		, _description( description )
		, _size(size)
		, _units(units)
	{
		_values = NULL;
	}

	int DimensionDefinition::datatype() const 
	{
		return _datatype;
	}

	size_t DimensionDefinition::size() const
	{
		return this->_size;
	}

	const std::string& DimensionDefinition::name() const 
	{
		return _name;
	}

	const std::string& DimensionDefinition::description() const 
	{
		return _description;
	}

	const std::string& DimensionDefinition::units() const
	{
		return _units;
	}

	bool DimensionDefinition::isUnlimited() const
	{
		return this->size()==DimensionDefinition::UNLIMITED;
	}


	class Group::impl
	{
		public:

			typedef std::shared_ptr< Group::impl > sptr;
			impl();
			impl::sptr addGroup( const std::string& name );
		private:
			netCDF::NcGroup _ncgrp;
			friend Group;
	};

	Group::impl::impl()
	{
	}

	//Group::impl::sptr Group::impl::addGroup( const std::string& name )
	//{
//		//std::make_shared< Group::impl> ();
//	}


	Group::Group( Group::sptr parent ) : _parent( parent )
	{
		_pimpl = std::make_shared< impl >();
	}

	Group::sptr Group::create( Group::sptr parent )
	{
		Group::sptr p( new Group( parent ) );
		return p;
	}

	Group::sptr Group::_addGroup( const std::string& name )
	{
		Group::sptr p(new Group( shared_from_this() ) );

		netCDF::NcGroup ncgrp = _pimpl->_ncgrp.addGroup( name );
		p->_pimpl->_ncgrp = ncgrp;

		return p;
	}

	class Builder::bimpl
	{
		public:
			typedef std::shared_ptr< bimpl > sptr;
			static sptr create( const std::string& path ); 

		private:
			bimpl( const std::string& path );

			netCDF::NcFile _ncfile;
	};

	Builder::bimpl::bimpl( const std::string& path ) : _ncfile( path, netCDF::NcFile::replace )
	{
	//	netCDF::NcFile ncfile( path, netCDF::NcFile::replace );
	}

	Builder::bimpl::sptr Builder::bimpl::create( const std::string& path )
	{
		sptr p( new bimpl( path ) );
		return p;
	}

	Builder::Builder( const std::string& path ) : Group( NULL )
	{
		_pbimpl = bimpl::create( path );
	}

	void prepareDimension( const std::string& grpName, DimensionDefinition& dimensionDefinition )
	{
	}


}
