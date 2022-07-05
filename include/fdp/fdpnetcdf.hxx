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

    class Builder
    {
        public:
            Builder() = delete;
	    Builder (const Builder& rhs ) = delete;

            static IBuilder::sptr create( const std::string& path, IBuilder::Mode mode );



    };

    template< size_t...N >
	    struct DDim
	    {
		    DDim()
		    {
			    int n = sizeof...(N);
			    //printf( "DDIM size:%d\n", n  );
			    
			    std::array< size_t, sizeof...(N) > a = {N...};

			    //printf("DDIM:");
			    //for( int i = 0;i < n; ++i )
			    //{
				    //printf("%d,", a[i]);
			    //}
			    //printf( "\n");
		    }
		    size_t rank(){ return sizeof...(N);}
		    std::vector<size_t> shape()
		    {
			    size_t n = this->rank();

			    std::vector<size_t> v;
			    v.reserve(n);

			    //int a[ sizeof...(N) ]= {N...};

			    std::array< size_t, sizeof...(N) > a = {N...};
			    for( int i = 0;i < n; ++i )
			    {
				   // printf("%d,", a[i]);
				    v.push_back( a[i] );
			    }
			    //printf( "\n");
			    return v;

		    }

		    //std::array< long, N > Dim;
	    };

}
#endif
