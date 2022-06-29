#include "fdpnetcdf.hxx"
#include<sstream>

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

    class Variable::impl
    {
        public:
            impl( const std::string& name, int vtype, const std::vector< Dimension::sptr>& dims );
            netCDF::NcVar ncvar;
    };

    Variable::impl::impl( const std::string& name, int vtype, const std::vector< Dimension::sptr >& dims )
    {
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

    std::string Group::name() const
    {
        bool fullname = false;
        netCDF::NcGroup& ncgrp = this->_pimpl->_ncgrp;
        return ncgrp.getName( fullname );
    }

    const std::map< std::string, Group::sptr >&  Group::getGroups() const 
    {
#if 0
        std::vector< Group::sptr > grps;

        netCDF::NcGroup& ncgrp = this->_pimpl->_ncgrp;
        std::multimap< std::string, netCDF::NcGroup > ncchldgrps = ncgrp.getGroups();

        for( auto it = ncchldgrps.begin(); it != ncchldgrps.end(); ++it )
        {
            std::string name = it->first;
            Group::sptr grp = this->getGroup( name );
            grps.push_back( grp );
        }
#endif
        return this->_name_grp_map;
    }

    Group::sptr Group::parent()
    {
        return this->_parent.lock();
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

	Group::sptr Group::requireGroup( const std::string& name )
    {
        std::vector< std::string > splits;
        split_str( name, '/', splits );

        Group::sptr parent = shared_from_this();
        Group::sptr grp;

        for( auto it = splits.begin(); it != splits.end(); ++it )
        {
            //does parent have named group?
            const std::string& s = *it;
            //grp = parent->_getGroup( s );
            if (NULL == parent )
            {
                grp = parent->_requireGroup( s );
            }
            parent = grp;
        }

        return grp;
    }
	Group::sptr Group::getGroup( const std::string& name )
    {
        std::vector< std::string > splits;
        split_str( name, '/', splits );

        Group::sptr parent = shared_from_this();
        Group::sptr grp;

        for( auto it = splits.begin(); it != splits.end(); ++it )
        {
            if( NULL != parent )
            {
                grp = parent->_getGroup( *it );
                parent = grp;
            }
        }

        return grp;
    }

    Group::sptr Group::_getGroup( const std::string& name )
    {
        Group::sptr grp;

        auto it = this->_name_grp_map.find( name );
        if( it != this->_name_grp_map.end() )
        {
            grp = it->second;
        }
        else
        {
            netCDF::NcGroup& this_ncgrp = this->_pimpl->_ncgrp;

            try
            {
                netCDF::NcGroup nc_grp = this_ncgrp.getGroup( name );

                grp = Group::sptr(new Group( shared_from_this() ) );
                grp->_pimpl->_ncgrp = nc_grp;
                this->_name_grp_map[ name ] = grp;
            }
            catch( netCDF::exceptions::NcException& e )
            {
                e.what();
            }
        }
        return grp;
    }

	Group::sptr Group::_requireGroup( const std::string& name )
	{
        Group::sptr grp = this->_getGroup( name );

        if( NULL != grp )
        {
            try
            {
                netCDF::NcGroup ncgrp = _pimpl->_ncgrp.addGroup( name );

                grp = Group::sptr(new Group( shared_from_this() ) );
                grp->_pimpl->_ncgrp = ncgrp;

                this->_name_grp_map[ name ] = grp;
            }
            catch( netCDF::exceptions::NcException& e )
            {
                e.what();
            }
        }

		return grp;
	}

    const std::map< std::string, Dimension::sptr >& Group::getDims() const
    {
        return this->_name_dim_map;
    }

    class Dimension::impl
    {
        public:
            friend class Group;

            impl() = delete;
            impl( const std::string& name, size_t N );

            netCDF::NcDim ncdim;
        private:
            std::string _name;
            size_t _N;

    };


    Dimension::sptr Group::addDim( const std::string& name, size_t N )
    {
        netCDF::NcGroup& this_ncgrp = this->_pimpl->_ncgrp;

        netCDF::NcDim ncdim = this_ncgrp.addDim( name, N );

        Dimension::sptr dim = Dimension::create( name, N );

        dim->_pimpl->ncdim = ncdim;
        this->_name_dim_map[ name ] = dim;

        return dim;
    }

    Variable::sptr Group::addVar( const std::string& name, int vtype, const std::vector< Dimension::sptr >& dims )
    {
        netCDF::NcGroup& this_ncgrp = this->_pimpl->_ncgrp;
        std::vector< netCDF::NcDim > ncdims;

        for( auto it = dims.begin(); it != dims.end(); ++it )
        {
            netCDF::NcDim& ncdim = (*it)->_pimpl->ncdim;
            ncdims.push_back( ncdim );
        }

        netCDF::NcVar ncvar = this_ncgrp.addVar( name, vtype, ncdims );

        Variable::sptr var( new Variable() );
        var->_pimpl->ncvar = ncvar;

        this->_name_var_map[ name ] = var;

        return var;

    }

    const std::map< std::string, Variable::sptr >& Group::getVars() const
    {
        return this->_name_var_map;

    }

    Dimension::impl::impl( const std::string& name, size_t N ) : _name(name), _N(N)
    {

        
    }

    Dimension::Dimension( const std::string& name, size_t N ) :  _pimpl( new impl( name, N ) )
    {
    }

    Dimension::sptr Dimension::create( const std::string& name, size_t N )
    {
        sptr dim( new Dimension( name, N ) );
        return dim;
    }

	class Builder::bimpl
	{
		public:
			typedef std::shared_ptr< bimpl > sptr;
			static sptr create( const std::string& path, enum Builder::Mode mode ); 

		private:
			bimpl( const std::string& path, Builder::Mode mode );

			netCDF::NcFile _ncfile;
	};

	Builder::bimpl::bimpl( const std::string& path, Builder::Mode mode ) : _ncfile( path, netCDF::NcFile::replace )
	{
	//	netCDF::NcFile ncfile( path, netCDF::NcFile::replace );
	}

	Builder::bimpl::sptr Builder::bimpl::create( const std::string& path, Builder::Mode mode )
	{
		sptr p( new bimpl( path, mode ) );
		return p;
	}

	Builder::Builder( const std::string& path, Builder::Mode mode ) : Group( NULL )
	{
		_pbimpl = bimpl::create( path, mode );
	}

	void prepareDimension( const std::string& grpName, DimensionDefinition& dimensionDefinition )
	{
	}


}
