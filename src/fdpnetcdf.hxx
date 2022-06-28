#ifndef __FDPAPI_NETCDF_H__
#define __FDPAPI_NETCDF_H__
#include<memory>

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

	class Group : public std::enable_shared_from_this< Group >
	{
		public:
			typedef std::shared_ptr< Group > sptr;

			Group::sptr _addGroup( const std::string& name );

			static sptr create( sptr parent );

		protected:
			Group( Group::sptr parent );
		private:
			struct impl;
			std::shared_ptr< impl > _pimpl;

			std::weak_ptr< Group > _parent;

	};

	class Builder : public Group
	{
		public:
			Builder( const std::string& path );

			void prepareDimension( const std::string& grpName, DimensionDefinition& dimensionDefinition );
		private:
			class bimpl;
			std::shared_ptr< bimpl > _pbimpl;
	};


}
#endif
