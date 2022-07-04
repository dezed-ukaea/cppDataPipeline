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
	
    struct IGroup
	{
		typedef std::shared_ptr< IGroup > sptr;

        typedef std::map< std::string, IGroup::sptr > NAME_GROUP_MAP;

        virtual IGroup::sptr parent() = 0;

        virtual IGroup::sptr requireGroup( const std::string& name ) = 0;

		virtual IGroup::sptr addGroup( const std::string& s ) = 0;

        virtual IGroup::sptr getGroup( const std::string& name ) = 0;

        virtual int getGroupCount() = 0;

        virtual std::string name() = 0;
	};

	struct IBuilder : public IGroup
	{
			enum Mode{ READ, WRITE};
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

			static GroupImpl::sptr create( GroupImpl::sptr p );

		protected:

			NcGroupPtr _nc;	

			GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc );

		private:
			std::weak_ptr<GroupImpl> _parent;

            IGroup::NAME_GROUP_MAP _name_grp_map;

	};

	class BuilderImpl 
		: public GroupImpl//, public IBuilder
	{
		public:
			typedef std::shared_ptr< BuilderImpl > sptr;

			static sptr create(const std::string& path, IBuilder::Mode mode );

        private:
			BuilderImpl( const std::string path, IBuilder::Mode mode ) ;

	};


    class Builder
    {
        public:
            Builder() = delete;

            static IBuilder::sptr create( const std::string& path, IBuilder::Mode mode );



    };



}
#endif
