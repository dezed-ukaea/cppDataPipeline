#include "fdp/fdpnetcdf.hxx"
#include<sstream>

#include<netcdf>

typedef netCDF::exceptions::NcException NcException;
typedef netCDF::NcGroup NcGroup;
typedef netCDF::NcFile NcFile;

typedef std::shared_ptr< NcGroup > NcGroupPtr;
typedef std::shared_ptr< NcFile > NcFilePtr;


namespace FairDataPipeline
{
    netCDF::NcType::ncType DataType2NcType( DataType dtype )
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


    DataType NcType2DataType( netCDF::NcType::ncType nctype )
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
                    , netCDF::NcVar& nc_var );

            
            void putVar( const void* vals );
            void getVar( void* vals );

            DataType getType();

            std::vector< std::string > getDims();

            IGroup::sptr parent(){return _parent.lock();}

            IVarAtt::sptr putAtt( const std::string& key, const std::string& value );
            IVarAtt::sptr getAtt( const std::string& key );

        private:
            std::weak_ptr< IGroup > _parent;
            VarImpl( IGroup::sptr parent, const std::string& name, netCDF::NcVar& nc_var ) ;
            netCDF::NcVar _nc_var;

            IVar::NAME_VARATT_MAP _name_varatt_map;
    };

    class GroupAtt : public IGroupAtt
    {
        public:
            typedef std::shared_ptr< GroupAtt > sptr;

            GroupAtt() = delete;
            static GroupAtt::sptr create( netCDF::NcGroupAtt& ncatt ) 
            {
                return GroupAtt::sptr( new GroupAtt( ncatt ) );
            }

            int getValues( int* values );
            int getValues( float* values );
            int getValues( std::string& values );
        private:
            GroupAtt( netCDF::NcGroupAtt& ncatt ) : _ncatt( ncatt ){}
            netCDF::NcGroupAtt _ncatt;

    };

    int GroupAtt::getValues( int* values )
    {
        int status = 0;
        
        try
        {
            _ncatt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
        }

        return status;
    }

    int GroupAtt::getValues( float* values )
    {
        int status = 0;
        
        try
        {
            _ncatt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
        }

        return status;
    }


    int GroupAtt::getValues( std::string& values )
    {
        int status = 0;
        
        try
        {
            _ncatt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
        }

        return status;
    }


    class VarAtt : public IVarAtt
    {
        public:
            VarAtt() = delete;
            static VarAtt::sptr create( netCDF::NcVarAtt& ncvaratt ) { return VarAtt::sptr( new VarAtt( ncvaratt ) );}
            int getValues( int* values );
            int getValues( float* values );
            int getValues( std::string& values );
        private:
            VarAtt( netCDF::NcVarAtt& ncvaratt ) : _ncvaratt( ncvaratt ){}
            netCDF::NcVarAtt _ncvaratt;

    };

   int VarAtt::getValues( int* values )
    {
        int status = 0;
        
        try
        {
            _ncvaratt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
        }

        return status;
    }

   int VarAtt::getValues( float* values )
    {
        int status = 0;
        
        try
        {
            _ncvaratt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
        }

        return status;
    }



    int VarAtt::getValues( std::string& values )
    {
        int status = 0;
        
        try
        {
            _ncvaratt.getValues( values );
        }
        catch( NcException& e )
        {
            e.what();
        }

        return status;
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

    std::vector< std::string > VarImpl::getDims()
    {
        std::vector< netCDF::NcDim > nc_dims = _nc_var.getDims();
        std::vector< std::string > names;

        for( auto it = nc_dims.begin(); it != nc_dims.end(); ++it )
        {
            names.push_back( (*it).getName() );
        }

        return names;
    }

    void VarImpl::putVar( const void* vals )
    {
        _nc_var.putVar( vals );
    }

    void VarImpl::getVar( void* vals )
    {
        _nc_var.getVar( vals );
    }

    VarImpl::VarImpl( IGroup::sptr parent, const std::string& name, netCDF::NcVar& nc_var ) 
        : _parent(parent), _nc_var( nc_var ) 
    {
    }

    VarImpl::sptr VarImpl::create( IGroup::sptr parent, const std::string& name, netCDF::NcVar& nc_var )
    {
        return VarImpl::sptr( new VarImpl( parent, name, nc_var ) );
    }

    class DimensionImpl : public IDimension
	{
		public:
			typedef std::shared_ptr< DimensionImpl > sptr;
            friend class GroupImpl;

            DimensionImpl() = delete;

			static sptr create( IGroup::sptr parent, netCDF::NcDim& nc_dim )
            {
                return DimensionImpl::sptr( new DimensionImpl( parent, nc_dim ) );
            }

            size_t getSize(){ return _nc_dim.getSize();}
            std::string getName(){return _nc_dim.getName();}
            IGroup::sptr getParentGroup(){ return _parent;}

		private:
			DimensionImpl( IGroup::sptr parent, netCDF::NcDim& nc_dim ) : _parent(parent), _nc_dim( nc_dim ) 
            {
            }
            netCDF::NcDim _nc_dim;
            IGroup::sptr _parent;
	};


	class GroupImpl
		: public std::enable_shared_from_this< GroupImpl>
          , public IGroup
	{
		public:
            virtual ~GroupImpl(){}

			typedef std::shared_ptr< GroupImpl > sptr;

			std::string name();

            int getGroupCount();
			IGroup::sptr parent();

            IGroup::sptr getGroup( const std::string& name );

			IGroup::sptr  _getGroup( const std::string name );

            const IGroup::NAME_GROUP_MAP& getGroups();

			IGroup::sptr requireGroup( const std::string& name );

			IGroup::sptr addGroup( const std::string& name );

            IDimension::sptr addDim( const std::string& name, size_t sz );
            IDimension::sptr getDim( const std::string& name );

            IVar::sptr addVar( const std::string& name, DataType dataType
                    , std::vector< IDimension::sptr >& vdims );

            IVar::sptr getVar( const std::string& name );

            IGroupAtt::sptr putAtt( const std::string& key, const std::string& value );
            IGroupAtt::sptr putAtt( const std::string& key, int value );
            IGroupAtt::sptr putAtt( const std::string& key, float value );
            IGroupAtt::sptr getAtt( const std::string& key );


			static GroupImpl::sptr create( GroupImpl::sptr p );

		protected:

			NcGroupPtr _nc;	

			GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc );

		private:
			std::weak_ptr<GroupImpl> _parent;

            IGroup::NAME_GROUP_MAP _name_grp_map;
            IGroup::NAME_DIM_MAP _name_dim_map;
            IGroup::NAME_VAR_MAP _name_var_map;

            IGroup::NAME_ATT_MAP _name_att_map;

	};

	IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, float value )
	{
		IGroupAtt::sptr grp_att;
		try
		{
			netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcFloat(), value );
			grp_att = GroupAtt::create( ncatt );

			_name_att_map[ key ] = grp_att;
		}
		catch( NcException& e )
		{
			e.what();
		}

		return grp_att;
	}
	IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, int value )
	{
		IGroupAtt::sptr grp_att;
		try
		{
			netCDF::NcGroupAtt ncatt = _nc->putAtt( key, netCDF::NcInt(), value );
			grp_att = GroupAtt::create( ncatt );

			_name_att_map[ key ] = grp_att;
		}
		catch( NcException& e )
		{
			e.what();
		}

		return grp_att;
	}


    IGroupAtt::sptr GroupImpl::putAtt( const std::string& key, const std::string& value )
    {
        IGroupAtt::sptr grp_att;
            try
            {
                netCDF::NcGroupAtt ncatt = _nc->putAtt( key, value );
                grp_att = GroupAtt::create( ncatt );

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
                att_ptr = GroupAtt::create( ncatt );
                _name_att_map[ key ] = att_ptr;
            }
            catch( NcException& e )
            {
                e.what();
            }

        }
        return att_ptr;

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

    const IGroup::NAME_GROUP_MAP& GroupImpl::getGroups()
    {
        auto nc_grps = this->_nc->getGroups();
        for( auto it_nc_grp = nc_grps.begin(); it_nc_grp != nc_grps.end(); ++it_nc_grp )
        {
            std::string name = it_nc_grp->first;

            IGroup::sptr grp_ptr = this->requireGroup( name );
        }

        return _name_grp_map;

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

            auto self_ptr = this->shared_from_this();

            GroupImpl::sptr new_grp_ptr = GroupImpl::create( self_ptr );

            new_grp_ptr->_nc =std::make_shared< NcGroup >( new_ncgrp) ;

            grp_ptr = new_grp_ptr;

            _name_grp_map[ name ] = grp_ptr;
        }
        catch( NcException& e )
        {
            printf("%s\n", e.what());
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
    std::string GroupImpl::name()
    {
        std::string s = this->_nc->getName();
        return s;
    }


	class BuilderImpl 
		: public GroupImpl
	{
		public:
			typedef std::shared_ptr< BuilderImpl > sptr;

			static sptr create(const std::string& path, IBuilder::Mode mode );

            void open( const std::string path, IBuilder::Mode mode );
            void close();

		private:
            NcFilePtr ncfile(){ return std::dynamic_pointer_cast< NcFile >( _nc ); }

			BuilderImpl( const std::string path, IBuilder::Mode mode ) ;

	};



    BuilderImpl::sptr BuilderImpl::create(const std::string& path, IBuilder::Mode mode )
    {
        auto p = BuilderImpl::sptr( new BuilderImpl(path, mode ));
        return p;
    }

    void BuilderImpl::open( const std::string path, IBuilder::Mode mode )
    {
        auto ncmode = NcFile::FileMode::read;

        switch( mode )
        {
            case IBuilder::Mode::READ:
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
            case IBuilder::Mode::WRITE:
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

    void BuilderImpl::close()
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


    BuilderImpl::BuilderImpl( const std::string path, IBuilder::Mode mode ) 
        : GroupImpl( NULL, std::make_shared<NcFile>() ) 
    {
        this->open( path, mode );

    }


    IBuilder::sptr BuilderFactory::create( const std::string& path, Mode mode ) 
    {
        auto builder = BuilderImpl::create( path, mode );
        return builder;
    }

    Builder::Builder( const std::string& path, IBuilder::Mode mode )
    {
        _builder = BuilderFactory::create( path, mode );

	if( IBuilder::Mode::WRITE == mode )	
		_builder->putAtt( "schema", 1 );
    }

    int Builder::readDim_metadata( /*const std::string& grp_name
            , */std::string dim_name
            , DimensionDefinition& dimdef )
    {
        int status = 0;
	std::string dim_name_orig = dim_name;

        IGroup::sptr grp_ptr = _builder;

	std::vector< std::string > splits;
	split_str( dim_name, '/', splits );

	if( splits.size() > 1 )
	{
		for(int i = 0; i < splits.size()-1; ++i )
		{
			if (grp_ptr)
				grp_ptr = grp_ptr->getGroup( splits[i] );
		}
		dim_name = splits[ splits.size()-1 ];
	}

        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( dim_name );
            std::vector< std::string > dimdef_names = var_ptr->getDims();

            auto dim_def_ptr = grp_ptr->getDim( dimdef_names[0] );
            size_t n = dim_def_ptr->getSize();
            DataType dtype = var_ptr->getType();

            std::string units, description;
            IVarAtt::sptr att_units_ptr = var_ptr->getAtt( "units" );
            if( att_units_ptr )
                att_units_ptr->getValues( units );
 
            IVarAtt::sptr att_desc_ptr = var_ptr->getAtt( "description" );
            if( att_desc_ptr )
                att_desc_ptr->getValues( description );
                  
            dimdef.name = dim_name_orig;
            dimdef.dataType = dtype;
            dimdef.size = n;
            dimdef.units = units;
            dimdef.description = description;
        }

        return status;
    }
    int Builder::readDim_data( /*const std::string& grp_name
            , */const DimensionDefinition& dimdef, void* data )
    {
        int status = 0;

	std::string dim_name_orig = dimdef.name;
	std::string dim_name = dim_name_orig;

        IGroup::sptr grp_ptr = _builder;

	std::vector< std::string > splits;
	split_str( dim_name_orig, '/', splits );

	if( splits.size() > 1 )
	{
		for(int i = 0; i < splits.size()-1; ++i )
		{
			if (grp_ptr)
				grp_ptr = grp_ptr->getGroup( splits[i] );
		}
		dim_name = splits[ splits.size()-1 ];
	}



       // IGroup::sptr grp_ptr = _builder->getGroup( grp_name );
        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( dim_name );
            var_ptr->getVar( data );
        }

        return status;
    }



    int Builder::readArray_metadata( /*const std::string& grp_name, */std::string array_name
            ,  ArrayDefinition& arraydef )
    {
        int status = 0;
	std::string array_name_orig = array_name;

        IGroup::sptr grp_ptr = _builder;

	std::vector< std::string > splits;
	split_str( array_name, '/', splits );

	if( splits.size() > 1 )
	{
		for(int i = 0; i < splits.size()-1; ++i )
		{
			if (grp_ptr)
				grp_ptr = grp_ptr->getGroup( splits[i] );
		}
		array_name = splits[ splits.size()-1 ];
	}

        std::vector< size_t > shape;

        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( array_name );

            if( var_ptr )
            {
                std::vector< std::string > dim_names;

                auto dims = var_ptr->getDims();
                for( int i = 0; i < dims.size(); ++i )
                {
                    const std::string& dim_name = dims[i];
                    IDimension::sptr dim_ptr = grp_ptr->getDim( dim_name );


                    size_t size = dim_ptr->getSize();
                    shape.push_back( size );

                    std::string dim_var_name( dim_name.begin(), dim_name.end()-4 );
                    IVar::sptr dim_var_ptr = grp_ptr->getVar( dim_var_name );

		    IGroup::sptr parent_grp_ptr = dim_var_ptr->parent();
		    while( parent_grp_ptr && parent_grp_ptr->name() != "/" )
		    {
			    dim_var_name = parent_grp_ptr->name() + "/" +dim_var_name;

			    parent_grp_ptr = parent_grp_ptr->parent();
		    }

                    if( dim_var_ptr )
                        dim_names.push_back( dim_var_name );
                    else
                        dim_names.push_back( "" );
                }

                size_t n = 1;
                for( int i = 0; i < shape.size(); ++i )
                {
                    n = n * shape[i];
                }

                DataType dtype =  var_ptr->getType();

                std::string units, description;

                IVarAtt::sptr att_units_ptr = var_ptr->getAtt( "units" );
                if( att_units_ptr )
                    att_units_ptr->getValues( units );

                IVarAtt::sptr att_desc_ptr = var_ptr->getAtt( "description" );
                if( att_desc_ptr )
                    att_desc_ptr->getValues( description );


                arraydef.name = array_name_orig;
                arraydef.dataType = dtype;
                arraydef.dimension_names = dim_names;
                arraydef.shape = shape;
                arraydef.units = units;
                arraydef.description = description;

            }
        }
        else
            status = 1;

        return status;
    }
    int Builder::readArray_data( /*const std::string& grp_name,*/ const ArrayDefinition& arraydef, void* data )
    {
        int status = 0;
	std::string array_name_orig = arraydef.name;
	std::string array_name = array_name_orig;

        IGroup::sptr grp_ptr = _builder;

	std::vector< std::string > splits;
	split_str( array_name_orig, '/', splits );

	if( splits.size() > 1 )
	{
		for(int i = 0; i < splits.size()-1; ++i )
		{
			if (grp_ptr)
				grp_ptr = grp_ptr->getGroup( splits[i] );
		}
		array_name = splits[ splits.size()-1 ];
	}


        //IGroup::sptr grp_ptr = _builder->getGroup( grp_name );
        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( array_name );
            if( var_ptr )
                var_ptr->getVar( data );
            else
                status = 1;
        }
        else
            status = 1;

        return status;
    }

    int Builder::writeDimension( /*const std::string& group_name,*/ const DimensionDefinition& dimdef, const void* data )
    {
        int status = 0;

	std::string dimdef_name = dimdef.name;

	IGroup::sptr grp_ptr =_builder;// this->shared_from_this();
#if 1
	std::vector< std::string > splits;

        split_str( dimdef.name, '/', splits );

	if( splits.size() > 1 )
	{
		for( int i=0; i < splits.size()-1; ++i )
		{
			grp_ptr = grp_ptr->requireGroup( splits[i] );
		}

		dimdef_name = splits[ splits.size()-1];
	}
#else
        grp_ptr = _builder->requireGroup( group_name );
#endif
        IVar::sptr dim_var_ptr = grp_ptr->getVar( dimdef_name );
        if( !dim_var_ptr )
        {
            IDimension::sptr dim_ptr = grp_ptr->addDim( dimdef_name + "_dim", dimdef.size );

            std::vector< IDimension::sptr > vdims = {dim_ptr};

            dim_var_ptr = grp_ptr->addVar( dimdef_name, dimdef.dataType, vdims );

            if( data )
                dim_var_ptr->putVar( data );

            dim_var_ptr->putAtt( "units", dimdef.units );
            dim_var_ptr->putAtt( "description", dimdef.description );
        }
        else
            status = 1;

        return status;
    }

    int Builder::writeArray( /*const std::string& group_name
            ,*/ const ArrayDefinition& arraydef, const void* data )
    {
        int status = 0;
        //write dims
        std::vector< IDimension::sptr > vdims;

        IGroup::sptr grp_ptr;// = _builder->requireGroup( group_name );

        for( int i = 0; i < arraydef.dimension_names.size();++i )
        {
		grp_ptr = _builder;
           std::string dim_name = arraydef.dimension_names[i];
	   std::vector< std::string > splits;
	   split_str( dim_name, '/', splits );

	   if( splits.size() > 1 )
	   {
		   for( int i=0; i < splits.size()-1; ++i )
		   {
			   grp_ptr = grp_ptr->requireGroup( splits[i] );
		   }
		   dim_name = splits[ splits.size()-1];
	   }

           if( dim_name == "" )//no dim
           {
               //create a dummy dim based on shape
               std::ostringstream os;
               os << arraydef.name << "_dim" << i;

               size_t n = arraydef.shape[ i ];

               std::string dummy_name = os.str();

               IDimension::sptr dim_ptr = grp_ptr->addDim(dummy_name, n );

               vdims.push_back( dim_ptr );
           }
           else
           {
               IVar::sptr dim_var_ptr = grp_ptr->getVar( dim_name );
               if( dim_var_ptr )
               {
                   std::vector< std::string > dims = dim_var_ptr->getDims();

                   IDimension::sptr dim_ptr = grp_ptr->getDim( dims[0] );
                   vdims.push_back( dim_ptr );
               }
               else 
                   status = 1;
           }
        }                    

        if( !status )
        {
	   std::vector< std::string > splits;
	   split_str( arraydef.name, '/', splits );
	   std::string arrname = arraydef.name;
	   grp_ptr = _builder;

	   if( splits.size() > 1 )
	   {
		   for( int i=0; i < splits.size()-1; ++i )
		   {
			   grp_ptr = grp_ptr->requireGroup( splits[i] );
		   }
		   arrname = splits[ splits.size()-1];
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
                status = 1;
        }

        return status;
    }
}


