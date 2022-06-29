#ifndef __FDPAPI_NETCDF_H__
#define __FDPAPI_NETCDF_H__
#include<memory>
#include<map>
#include<vector>

#include <string>

namespace fdp
{
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
    class Dimension
    {
        public:
            typedef std::shared_ptr< Dimension > sptr;
            static sptr create( const::std::string& name, size_t N );

            friend class Group;

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
            Variable(){}

          private:
            class impl;
            std::shared_ptr< impl > _pimpl;
    };


	class Group : public std::enable_shared_from_this< Group >
	{
		public:
			typedef std::shared_ptr< Group > sptr;

			static sptr create( sptr parent );

            Group() = delete;

            const std::map< std::string, Group::sptr >& getGroups() const;

			Group::sptr getGroup( const std::string& name );
			Group::sptr requireGroup( const std::string& name );


            Group::sptr parent();

            std::string name() const;

            Dimension::sptr addDim( const std::string& name, size_t N );
            const std::map< std::string, Dimension::sptr >& getDims() const;

            Variable::sptr addVar( const std::string& name, int vtype, const std::vector< Dimension::sptr >& dims );
            const std::map< std::string, Variable::sptr >& getVars() const;

		protected:
			Group( Group::sptr parent );
		private:
            std::map< std::string, Group::sptr > _name_grp_map;
            std::map< std::string, Dimension::sptr > _name_dim_map;
            std::map< std::string, Variable::sptr > _name_var_map;


			Group::sptr _getGroup( const std::string& name );
			Group::sptr _requireGroup( const std::string& name );

			struct impl;
			std::shared_ptr< impl > _pimpl;

			std::weak_ptr< Group > _parent;

	};


	class Builder : public Group
	{
		public:
            enum Mode{ READ, WRITE};

            Builder() = delete;
			Builder( const std::string& path, Mode mode );

			void prepareDimension( const std::string& grpName, DimensionDefinition& dimensionDefinition );
		private:
			class bimpl;
			std::shared_ptr< bimpl > _pbimpl;
	};


}
#endif
