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

			static GroupImpl::sptr create( GroupImpl::sptr p );

		protected:

			NcGroupPtr _nc;	

			GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc );

		private:
			std::weak_ptr<GroupImpl> _parent;

            IGroup::NAME_GROUP_MAP _name_grp_map;

	};

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

		private:
			BuilderImpl( const std::string path, IBuilder::Mode mode ) ;

	};



    BuilderImpl::sptr BuilderImpl::create(const std::string& path, IBuilder::Mode mode )
    {
        auto p = BuilderImpl::sptr( new BuilderImpl(path, mode ));
        return p;
    }

    BuilderImpl::BuilderImpl( const std::string path, IBuilder::Mode mode ) 
        : GroupImpl( NULL, std::make_shared<NcFile>() ) 
    {
        auto ncmode = NcFile::FileMode::read;

        auto ncfile = std::dynamic_pointer_cast< NcFile >(_nc);

        switch( mode )
        {
            case IBuilder::Mode::READ:
                {
                    try
                    {
                        ncfile->open( path, ncmode );
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
                        ncfile->open( path, ncmode );
                    }
                    catch( NcException& e )
                    {
                        ncmode = NcFile::FileMode::newFile;

                        ncfile->open( path, ncmode );
                    }
                }
                break;
            default:
                break;

        }
    }




    IBuilder::sptr Builder::create( const std::string& path, IBuilder::Mode mode ) 
    {
        auto builder = BuilderImpl::create( path, mode );
        return builder;
    }

}


