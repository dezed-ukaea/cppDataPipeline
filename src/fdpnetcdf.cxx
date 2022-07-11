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
    size_t DataType_sizeof( DataType dtype )
    {
        size_t sz = 0;

        switch( dtype )
        {
            case BYTE:
                sz = 1;
                break;
            case CHAR:
                sz = 1;
                break;
            case SHORT:
                sz = sizeof( short );
                break;
            case INT:
                sz = sizeof( int );
                break;
            case INT64:
                sz = sizeof( int64_t );
                break;

            case UBYTE:
                sz = 1;
                break;

            case USHORT:
                sz = sizeof( unsigned short );
                break;
            case UINT:
                sz = sizeof( unsigned int );
                break;
            case UINT64:
                sz = sizeof( uint64_t );
                break;


            case FLOAT:
                sz = sizeof( float );
                break;
            case DOUBLE:
                sz = sizeof(double);
                break;
            default:
                break;

        }
        return sz;
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

    class VarAtt : public IVarAtt
    {
        public:
            VarAtt() = delete;
            static VarAtt::sptr create( netCDF::NcVarAtt& ncvaratt ) { return VarAtt::sptr( new VarAtt( ncvaratt ) );}
            int getValues( std::string& values );
        private:
            VarAtt( netCDF::NcVarAtt& ncvaratt ) : _ncvaratt( ncvaratt ){}
            netCDF::NcVarAtt _ncvaratt;

    };

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
#if 0
            void rename(const std::string& new_name)
            {
                std::string name = this->name();

                auto it = _parent->_name_dim_map.find( this->name() );
                if( it != this->_name_dim_map.end() )
                    this->_name_dim_map.erase( it );

                _parent->_name_dim_map[ name ] = this->shared_from_this();

                _nc_dim.rename( new_name );
                
            }
#endif
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

			static GroupImpl::sptr create( GroupImpl::sptr p );

		protected:

			NcGroupPtr _nc;	

			GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc );

		private:
			std::weak_ptr<GroupImpl> _parent;

            IGroup::NAME_GROUP_MAP _name_grp_map;
            IGroup::NAME_DIM_MAP _name_dim_map;
            IGroup::NAME_VAR_MAP _name_var_map;

	};

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
            netCDF::NcVar ncvar = _nc->addVar( name, netCDF::NcInt(), ncdims );
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
		: public GroupImpl//, public IBuilder
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
                        ncmode = NcFile::FileMode::write;
                        ncfile()->open( path, ncmode );
                    }
                    catch( NcException& e )
                    {
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
    }

    int Builder::readDim_metadata( const std::string& grp_name
            , const std::string dim_name
            , DimensionDefinition& dimdef )
    {
        int status = 0;

        IGroup::sptr grp_ptr = _builder->getGroup( grp_name );
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
                  
            dimdef.name = dim_name;
            dimdef.dataType = dtype;
            dimdef.size = n;
            dimdef.data = NULL;
            dimdef.units = units;
            dimdef.description = description;
        }

        return status;
    }
    int Builder::readDim_data( const std::string& grp_name
            , const std::string dim_name
            , DimensionDefinition& dimdef )
    {
        int status = 0;

        IGroup::sptr grp_ptr = _builder->getGroup( grp_name );
        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( dim_name );
            var_ptr->getVar( dimdef.data );
        }

        return status;
    }



    int Builder::readArray_metadata( const std::string& grp_name, const std::string& array_name
            ,  ArrayDefinition& arraydef )
    {
        int status = 0;
        IGroup::sptr grp_ptr = _builder->getGroup( grp_name );

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

                //char* p = (char*)malloc(  n * DataType_sizeof( dtype ) );
                //var_ptr->getVar( p );

                std::string units, description;

                IVarAtt::sptr att_units_ptr = var_ptr->getAtt( "units" );
                if( att_units_ptr )
                    att_units_ptr->getValues( units );

                IVarAtt::sptr att_desc_ptr = var_ptr->getAtt( "description" );
                if( att_desc_ptr )
                    att_desc_ptr->getValues( description );


                arraydef.name = array_name;
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
    int Builder::readArray_data( const std::string& grp_name, const std::string& name
            ,  ArrayDefinition& arraydef )
    {
        int status = 0;
        IGroup::sptr grp_ptr = _builder->getGroup( grp_name );
        if( grp_ptr )
        {
            IVar::sptr var_ptr = grp_ptr->getVar( name );
            if( var_ptr )
                var_ptr->getVar( arraydef.data );
            else
                status = 1;
        }
        else
            status = 1;

        return status;
    }

    int Builder::writeDimension(const std::string& group_name, const DimensionDefinition& dimdef )
    {
        int status = 0;

        IGroup::sptr grp_ptr = _builder->requireGroup( group_name );

        IVar::sptr dim_var_ptr = grp_ptr->getVar( dimdef.name );
        if( !dim_var_ptr )
        {
            IDimension::sptr dim_ptr = grp_ptr->addDim( dimdef.name + "_dim", dimdef.size );

            std::vector< IDimension::sptr > vdims = {dim_ptr};

            dim_var_ptr = grp_ptr->addVar( dimdef.name, dimdef.dataType, vdims );
            dim_var_ptr->putVar( dimdef.data );

            dim_var_ptr->putAtt( "units", dimdef.units );
            dim_var_ptr->putAtt( "description", dimdef.description );
        }
        else
            status = 1;

        return status;
    }

    int Builder::writeArray( const std::string& group_name
            , const ArrayDefinition& arraydef )
    {
        int status = 0;
        //write dims
        std::vector< IDimension::sptr > vdims;

        IGroup::sptr grp_ptr = _builder->requireGroup( group_name );

        for( int i = 0; i < arraydef.dimension_names.size();++i )
        {
           const std::string& dim_name = arraydef.dimension_names[i];

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
            IVar::sptr arr_var_ptr = grp_ptr->getVar( arraydef.name );
            if( !arr_var_ptr )
            {
                arr_var_ptr = grp_ptr->addVar( arraydef.name, arraydef.dataType, vdims );
                arr_var_ptr->putVar( arraydef.data );

                arr_var_ptr->putAtt( "units", arraydef.units );
                arr_var_ptr->putAtt( "description", arraydef.description );
            }
            else
                status = 1;
        }

        return status;
    }



}


