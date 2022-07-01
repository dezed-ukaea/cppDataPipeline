#include "fdp/fdpnetcdf.hxx"
#include<sstream>

#include<netcdf>

namespace FairDataPipeline
{
    void split_str( const std::string& s, const char delim, std::vector< std::string >& splits )
    {
        std::stringstream ss( s );

        std::string s_;
        while( std::getline( ss, s_, delim ))
        {
            splits.push_back( s_ );
        }
    }
#if 0

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
#if 0
	Variable::sptr Variable::create()
	{
		return sptr( new Variable );
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

#endif

#if 0
    const NAME_DIMENSION_MAP& Group::getDims() const
    {
        return this->_name_dim_map;
    }
#endif
#if 0
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
#endif
#if 0
    Dimension::sptr Group::addDim( const std::string& name, size_t N )
    {
        auto this_ncgrp = this->_ncgrp;

        netCDF::NcDim ncdim = this_ncgrp.addDim( name, N );

        Dimension::sptr dim = Dimension::create( name, N );

        dim->_pimpl->ncdim = ncdim;
        this->_name_dim_map[ name ] = dim;

        return dim;
    }
    Variable::sptr Group::addVar( const std::string& name, int vtype, const std::vector< Dimension::sptr >& dims )
    {
        auto this_ncgrp = this->_ncgrp;
        std::vector< netCDF::NcDim > ncdims;

        for( auto it = dims.begin(); it != dims.end(); ++it )
        {
            netCDF::NcDim& ncdim = (*it)->_pimpl->ncdim;
            ncdims.push_back( ncdim );
        }


	Variable::sptr var;
	try
	{
		netCDF::NcVar ncvar = this_ncgrp.addVar( name, vtype, ncdims );

		var = Variable::create();
		var->_pimpl->ncvar = ncvar;

		this->_name_var_map[ name ] = var;
	}
	catch( netCDF::exceptions::NcException& e )
	{
		e.what();
	}

        return var;
    }

#endif
#if 0
    const NAME_VARIABLE_MAP& Group::getVars() const
    {
        return this->_name_var_map;

    }
#endif
#if 0
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
#endif
#if 0
	class Builder::bimpl
	{
		public:
			typedef std::shared_ptr< bimpl > sptr;
			static sptr create( const std::string& path, enum Builder::Mode mode ); 


			netCDF::NcFile _ncfile;
		private:
			bimpl( const std::string& path, Builder::Mode mode );
	};

	Builder::bimpl::bimpl( const std::string& path, Builder::Mode mode )
	{
		netCDF::NcFile::FileMode ncmode = netCDF::NcFile::FileMode::read;
		switch( mode )
		{
			case READ:
				{
					ncmode = netCDF::NcFile::FileMode::read;

					_ncfile.open( path, ncmode );
				}
				break;
			case WRITE:
				try
				{
					ncmode = netCDF::NcFile::FileMode::write;

					_ncfile.open( path, ncmode );
				}
				catch( netCDF::exceptions::NcException& e )
				{
					ncmode = netCDF::NcFile::FileMode::replace;

					_ncfile.open( path, ncmode );
				}
				break;
			default:
				break;
		};
	}

	Builder::bimpl::sptr Builder::bimpl::create( const std::string& path, Builder::Mode mode )
	{
		sptr p( new bimpl( path, mode ) );
		return p;
	}
#endif
	Builder::sptr Builder::create( const std::string& path, Builder::Mode mode )
	{
		return Builder::sptr( new Builder( path, mode ) );
	}

	Builder::Builder( const std::string& path, Builder::Mode mode ) :   BuilderImpl( NULL )
	{
		//NcFilePtr ncfile = std::dynamic_pointer_cast< netCDF::NcFile>(_ncgrp);

		netCDF::NcFile::FileMode ncmode = netCDF::NcFile::FileMode::read;
		switch( mode )
		{
			case READ:
				{
					ncmode = netCDF::NcFile::FileMode::read;

					_ncgrp.open( path, ncmode );
				}
				break;
			case WRITE:
				try
				{
					ncmode = netCDF::NcFile::FileMode::write;

					_ncgrp.open( path, ncmode );
				}
				catch( netCDF::exceptions::NcException& e )
				{
					ncmode = netCDF::NcFile::FileMode::replace;

					_ncgrp.open( path, ncmode );
				}
				break;
			default:
				break;
		}

	}

	const NAME_GROUP_MAP&  Builder::getGroups() const 
	{
		return NAME_GROUP_MAP();
	}

#if 1
	IGroup::sptr Builder::requireGroup( const std::string& name )
	{
		return NULL;
	}
#endif

	IGroup::sptr Builder::getGroup( const std::string& name )
	{
		return NULL;
	}

	void prepareDimension( const std::string& grpName, DimensionDefinition& dimensionDefinition )
	{
	}
#if 0
    Group::sptr Builder::_requireGroup( const std::string& name )
    {
	    Group::sptr grp = this->_getGroup( name );

	    if( !grp )
	    {
		    try
		    {
			    netCDF::NcGroup ncgrp = _pbimpl->_ncfile.addGroup( name );

			    grp = Group::create( shared_from_this() );
			    grp->_ncgrp = ncgrp;

			    this->_name_grp_map[ name ] = grp;
		    }
		    catch( netCDF::exceptions::NcException& e )
		    {
			    e.what();
		    }
	    }

	    return grp;
    }
#endif

#endif
}

