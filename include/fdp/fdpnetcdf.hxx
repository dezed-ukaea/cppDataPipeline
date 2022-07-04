#ifndef __FDPAPI_NETCDF_H__
#define __FDPAPI_NETCDF_H__
#include<stdio.h>
#include<memory>
#include<map>
#include<vector>

#include <string>

#include<sstream>
#include<netcdf>

namespace FairDataPipeline
{
	void split_str( const std::string& s, const char delim, std::vector< std::string >& splits );

	typedef netCDF::exceptions::NcException NcException;
	typedef netCDF::NcGroup NcGroup;
	typedef netCDF::NcFile NcFile;

	typedef std::shared_ptr< NcGroup > NcGroupPtr;
	typedef std::shared_ptr< NcFile > NcFilePtr;

	class DimensionDefinition
	{
		public:
			static int UNLIMITED;

			DimensionDefinition( const std::string& name
					, int datatype
					, const std::string& description
					, size_t size
					, const std::string& units
					);

			int datatype() const ;
			const std::string& name() const ;
			const std::string& description() const ;
			const std::string& units() const ;
			bool isUnlimited() const;
			size_t size() const;

		private:
			std::string _name;
			std::string _description;
			int _datatype;
			size_t _size;
			std::string _units;
			void* _values;
	};
#if 0
	class Dimension
	{
		public:
			typedef std::shared_ptr< Dimension > sptr;
			static sptr create( const::std::string& name, size_t N );

			//friend class Group;

		private:
			Dimension( const std::string&, size_t N );
			class impl;
			std::shared_ptr< impl >  _pimpl;
	};

	class Variable
	{
		public:
			typedef std::shared_ptr< Variable > sptr;
			friend class Group;
			static sptr create();

		private:
			class impl;
			std::shared_ptr< impl > _pimpl;
	};
#endif
#if 0
	class IGroup;
	class IBuilder;

	typedef std::shared_ptr< IGroup > IGroupPtr;
	typedef std::shared_ptr< IBuilder > IBuilderPtr;


	typedef std::map< std::string, IGroupPtr > NAME_GROUP_MAP;
	//typedef std::map< std::string, Dimension::sptr > NAME_DIMENSION_MAP;
	//typedef std::map< std::string, Variable::sptr > NAME_VARIABLE_MAP;


	struct IGroup
	{
		typedef IGroupPtr sptr;

		virtual const NAME_GROUP_MAP& getGroups() const= 0;

		virtual sptr getGroup( const std::string& name) = 0;
		virtual sptr requireGroup( const std::string& name ) = 0;


		virtual sptr parent() = 0;

		virtual std::string name() const = 0;

		//virtual Dimension::sptr addDim( const std::string& name, size_t N ) = 0;
		//virtual const NAME_DIMENSION_MAP& getDims() const;

		//virtual Variable::sptr addVar( const std::string& name, int vtype, const std::vector< Dimension::sptr >& dims ) = 0;
		//virtual const NAME_VARIABLE_MAP& getVars() const = 0;
	};

	struct IBuilder : public IGroup
	{
		typedef IBuilderPtr sptr;
	};


#if 0
	struct GroupCommon
	{
		NAME_GROUP_MAP _name_grp_map;
		NAME_VARIABLE_MAP _name_var_map;
		NAME_DIMENSION_MAP _name_dim_map;


		GroupCommon( NcGroupPtr ncgrp ){_ncgrp = ncgrp;}
		NcGroupPtr _ncgrp;
	};
#endif

	template< typename NC >
		struct GroupImpl 
		: public IGroup, public std::enable_shared_from_this< GroupImpl< NC > >
		{
			public:
				typedef std::shared_ptr< GroupImpl< NC > > sptr;
				// static IGroup::sptr create( IGroup::sptr parent );

				GroupImpl() = delete;

				GroupImpl( IGroup::sptr parent ) : _parent( parent )
			{
				//_pimpl = std::make_shared< impl >(parent);
				//_pimpl->_parent = parent;
			}

				static IGroup::sptr create( IGroup::sptr parent )
				{
					IGroup::sptr p( new GroupImpl( parent ) );
					return p;
				}

				std::string name()  const 
				{
					bool fullname = false;
					//auto ncgrp = this->_ncgrp;
					return this->_ncgrp.getName( fullname );
				}

				const NAME_GROUP_MAP&  getGroups() const 
				{
					return this->_name_grp_map;
				}

				IGroup::sptr parent()
				{
					return this->_parent;
				}


				IGroup::sptr requireGroup( const std::string& name )
				{
					std::vector< std::string > splits;
					split_str( name, '/', splits );

					IGroup::sptr parent = this->shared_from_this();
					IGroup::sptr grp;

					for( auto it = splits.begin(); it != splits.end(); ++it )
					{
						//does parent have named group?
						const std::string& s = *it;
						if (NULL != parent )
						{
							grp = std::dynamic_pointer_cast<GroupImpl>( parent )->_requireGroup( s );
						}
						parent = grp;
					}

					return grp;
				}

				IGroup::sptr getGroup( const std::string& name )
				{
					std::vector< std::string > splits;
					split_str( name, '/', splits );

					IGroup::sptr parent = this->shared_from_this();
					IGroup::sptr grp;

					for( auto it = splits.begin(); it != splits.end(); ++it )
					{
						if( NULL != parent )
						{
							grp = std::dynamic_pointer_cast< GroupImpl >(parent)->_getGroup( *it );
							parent = grp;
						}
					}

					return grp;
				}


				IGroup::sptr _getGroup( const std::string& name )
				{
					IGroup::sptr grp;

					NAME_GROUP_MAP& name_grp_map = this->_name_grp_map;

					auto it = name_grp_map.find( name );
					if( it != name_grp_map.end() )
					{
						grp = it->second;
					}
					else
					{
						// auto this_ncgrp = this->_ncgrp;

						try
						{
							netCDF::NcGroup nc_grp = this->_ncgrp.getGroup( name );

							grp = GroupImpl::create( this->shared_from_this() );
							//(std::dynamic_pointer_cast< GroupImpl<NC> >(grp) )->_ncgrp = nc_grp;
							name_grp_map[ name ] = grp;
						}
						catch( NcException& e )
						{
							e.what();
						}
					}
					return grp;
				}
				GroupImpl::sptr _requireGroup( const std::string& name )
				{
					IGroup::sptr grp = this->_getGroup( name );

					if( !grp )
					{
						try
						{
							netCDF::NcGroup ncgrp = _ncgrp.addGroup( name );
							GroupImpl<NC>::sptr this_ptr = this->shared_from_this();
							IGroup::sptr igrp = std::dynamic_pointer_cast<IGroup>(this_ptr);

							GroupImpl<netCDF::NcGroup>::sptr
								_grp_(new GroupImpl<netCDF::NcGroup>( igrp ) );
							_grp_->_ncgrp = ncgrp;
							grp = std::dynamic_pointer_cast<IGroup>(_grp_);
							//std::dynamic_pointer_cast<GroupImpl>(grp)->_ncgrp = ncgrp;

							this->_name_grp_map[ name ] = grp;
						}
						catch( NcException& e )
						{
							e.what();
						}
					}

					return std::dynamic_pointer_cast<GroupImpl>(grp);
				}
				//const NAME_GROUP_MAP& getGroups() const;

				//IGroup::sptr getGroup( const std::string& name );

				//IGroup::sptr _getGroup( const std::string& name );
				//IGroup::sptr requireGroup( const std::string& name );

				//IGroup::sptr parent();

				//std::string name() const;

				//Dimension::sptr addDim( const std::string& name, size_t N );
				//const NAME_DIMENSION_MAP& getDims() const;

				//Variable::sptr addVar( const std::string& name, int vtype, const std::vector< Dimension::sptr >& dims );
				//const NAME_VARIABLE_MAP& getVars() const;


				NC _ncgrp;
			protected:
				//GroupImpl( IGroup::sptr parent );

				IGroup::sptr _parent;

				//netCDF::NcGroup  _ncgrp;

				//GroupImpl::sptr _requireGroup( const std::string& name );
				NAME_GROUP_MAP _name_grp_map;
				//NAME_VARIABLE_MAP _name_var_map;
				//NAME_DIMENSION_MAP _name_dim_map;


			private:
		};



	class BuilderImpl : public GroupImpl< netCDF::NcFile >
	{
		public:
		protected:
			BuilderImpl( IGroupPtr parent ) : GroupImpl( parent )
		{
		}
		private:


	};


	class Builder : public IBuilder, public BuilderImpl

			//, public GroupBase< Builder, netCDF::NcFile >
	{
		public:
			typedef std::shared_ptr< Builder > sptr;

			enum Mode{ READ, WRITE };

			Builder() = delete;
			static sptr create( const std::string& path, Mode mode );

			IGroup::sptr parent(){ return NULL;}
			std::string name() const { return "/";}

			void prepareDimension( const std::string& grpName, DimensionDefinition& dimensionDefinition );


			const NAME_GROUP_MAP& getGroups() const;

			IGroup::sptr getGroup( const std::string& name );
			IGroup::sptr requireGroup( const std::string& name );
			//virtual IGroup::sptr _requireGroup( const std::string& name );
		private:
			Builder( const std::string& path, Mode mode );
			//netCDF::NcFile _ncgrp;

	};

#endif	
	struct IGroup2
	{
		typedef std::shared_ptr< IGroup2 > sptr;

		virtual IGroup2::sptr addGroup( const std::string& s ) = 0;
	};

	struct IBuilder2 : public IGroup2
	{

		//void addGroup( std::string s );
	};

	class Group2
		: public std::enable_shared_from_this< Group2>
	{
		public:
			typedef std::shared_ptr< Group2 > sptr;

			std::string name()
			{
				std::string s = this->_nc->getName();
				return s;
			}

			Group2::sptr parent(){ return _parent.lock(); }

			Group2::sptr getGroup( const std::string& name )
			{
				std::vector< std::string > splits;
				split_str( name, '/', splits );

				Group2::sptr parent_grp_ptr = this->shared_from_this();
				Group2::sptr grp_ptr;

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

			Group2::sptr  _getGroup( const std::string name )
			{
				Group2::sptr grp_ptr;

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
						Group2::sptr new_grp_ptr = Group2::create( this->shared_from_this() );
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

			virtual Group2::sptr addGroup( const std::string& name )
			{
				Group2::sptr grp_ptr;

				NcGroupPtr nc = std::dynamic_pointer_cast< NcGroup >(_nc);

				try
				{
					NcGroup new_ncgrp( nc->addGroup( name ) );

					auto self_ptr = this->shared_from_this();

					Group2::sptr new_grp_ptr = Group2::create( self_ptr );
					
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

			static Group2::sptr create( Group2::sptr p )
			{
				auto _nc = std::make_shared< NcGroup >();
				auto pgrp = Group2::sptr( new Group2(p, _nc));
				return pgrp;
			}
			~Group2(){}

			NcGroupPtr _nc;	
		protected:

			std::map< std::string, Group2::sptr > _name_grp_map;

			Group2( Group2::sptr grp, NcGroupPtr nc ) : _parent( grp ), _nc(nc)
			{
			}

		private:
			std::weak_ptr<Group2> _parent;

	};

	class Builder2 
		: public Group2
	{
		public:
			enum Mode{ READ, WRITE};
			typedef std::shared_ptr< Builder2 > sptr;

			~Builder2() {}

			static Builder2::sptr create(const std::string& path, Builder2::Mode mode )
			{
				auto p = Builder2::sptr( new Builder2(path, mode ));
				//auto p = std::make_shared<Builder2>( path, mode );
				return p;
			}
#if 0
			Group2::sptr addGroup( netCDF::NcGroup& ncgrp, const std::string& name )
			{
				return NULL;
			}
#endif
#if 0
			Group2::sptr addGroup( const std::string& name )
			{
				Group2::sptr grp_ptr;

				NcFilePtr ncfile = std::dynamic_pointer_cast<netCDF::NcFile>(_nc);

				try
				{
					netCDF::NcGroup new_ncgrp( ncfile->addGroup( name ) );

					auto self_ptr = this->shared_from_this();

					Group2::sptr new_grp_ptr = Group2::create( self_ptr );
					new_grp_ptr->_nc = NcGroupPtr( new netCDF::NcGroup(new_ncgrp) );

					grp_ptr = new_grp_ptr;

					_name_grp_map[ name ] = grp_ptr;
				}
				catch( netCDF::exceptions::NcException& e )
				{
					printf("%s\n", e.what());
				}

				return  grp_ptr;

			}
#endif
		private:
			Builder2( const std::string path, Mode mode ) 
				: Group2( NULL, std::make_shared<NcFile>() )
			{
				auto ncmode = NcFile::FileMode::read;

				auto ncfile = std::dynamic_pointer_cast< NcFile >(_nc);

				switch( mode )
				{
					case Mode::READ:
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
					case Mode::WRITE:
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
							//netCDF::NcGroup grp1 = ncfile->addGroup("testgrp");
							//netCDF::NcGroup grp2 = ncfile->addGroup("xtestgrp");
						}
						break;
					default:
						break;

				}
			}

	};


}
#endif
