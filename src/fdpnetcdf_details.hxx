#ifndef __FDP_NETCDF_DETAILS__
#define __FDP_NETCDF_DETAILS__

#include<memory>

#include<netcdf>

#include "fdp/fdpnetcdf.hxx"

typedef netCDF::exceptions::NcException NcException;
typedef netCDF::NcGroup NcGroup;
typedef netCDF::NcFile NcFile;

typedef std::shared_ptr< NcGroup > NcGroupPtr;
typedef std::shared_ptr< NcFile > NcFilePtr;

namespace FairDataPipeline
{
    class GroupImpl;
    typedef std::shared_ptr< GroupImpl > GroupImplSptr;

    class TableImpl;
    typedef std::shared_ptr< TableImpl > TableImplSptr;

    class CompoundTypeImp;
    typedef std::shared_ptr< CompoundTypeImp > CompoundTypeImpSptr;


    class VarAtt : public IVarAtt
    {
        public:
            typedef std::shared_ptr< VarAtt > sptr;
            VarAtt() = delete;
            static VarAtt::sptr create( const netCDF::NcVarAtt& ncvaratt );


            DataType getAttType();
            int getClassType();

            size_t getAttLength();
            std::string getName();

            int getValues( short* values );
            int getValues( int* values );
            int getValues( long* values );
            int getValues( long long* values );
            int getValues( unsigned short* values );
            int getValues( unsigned int* values );
            int getValues( unsigned long* values );
            int getValues( unsigned long long* values );

            int getValues( float* values );
            int getValues( double* values );

            int getValues( std::string& values );
        private:

            template< typename T>
                int getValuesImpl( T* values );

            VarAtt( const netCDF::NcVarAtt& ncvaratt ) : _ncvaratt( ncvaratt ){}
            netCDF::NcVarAtt _ncvaratt;

    };



    class VarImpl : public IVar
    {
        public:
            typedef std::shared_ptr< VarImpl > sptr;

            //typedef std::map< std::string, IVarAttSptr > NAME_VARATT_MAP;
            typedef std::map< std::string, IAttSptr > NAME_ATT_MAP;

            static VarImpl::sptr create( IGroupSptr parent
                    , const std::string& name
                    , const netCDF::NcVar& nc_var );


            void putVar( const void* vals );
            void putVar( const std::vector< size_t >& index, const void* vals );
            void putVar( const std::vector< size_t >& index, const std::vector< size_t >& countp, const void* values );


            void putVar( const std::vector< size_t >& index, int value ) const;
            void putVar( const std::vector< size_t >& index
                    , const std::vector< size_t >& countp
                    , int* values ) const;

            void getVar( void* vals );

            DataType getType();

            std::vector< IDimensionSptr > getDims();

            IGroupSptr parent();


            IAttSptr putAttShort( const std::string& key, short value );
            IAttSptr putAttInt( const std::string& key, int value );
            IAttSptr putAttLong( const std::string& key, long value );
            IAttSptr putAttLongLong( const std::string& key, long long value );

            IAttSptr putAttUShort( const std::string& key, unsigned short value );
            IAttSptr putAttUInt( const std::string& key, unsigned int value );

            IAttSptr putAttString( const std::string& key, const std::string& value);
            IAttSptr putAttFloat( const std::string& key, float value );
            IAttSptr putAttDouble( const std::string& key, double value );

            IAttSptr putAttShorts( const std::string& key, size_t nvals, const short* values );
            IAttSptr putAttInts( const std::string& key, int nvals, const int* values );
            IAttSptr putAttLongs( const std::string& key, size_t nvals, const long* values );
            IAttSptr putAttLongLongs( const std::string& key, size_t nvals, const long long* values );

            IAttSptr putAttUShorts( const std::string& key, size_t nvals, const unsigned short* values );
            IAttSptr putAttUInts( const std::string& key, size_t nvals, const unsigned int* values );
            IAttSptr putAttULongs( const std::string& key, size_t nvals, const unsigned long* values );
            IAttSptr putAttULongLongs( const std::string& key, size_t nvals, const unsigned long long* values );

            IAttSptr putAttFloats( const std::string& key, size_t nvals, const float* values );
            IAttSptr putAttDoubles( const std::string& key, size_t nvals, const double* values );

            IAttSptr putAttStrs( const std::string& key, size_t nvals, const char** values );


            IAttSptr putAttVal( const std::string& key, DataType datatype, const void* pv );
            IAttSptr putAttVals( const std::string& key, DataType datatype, size_t nvals, const void* pv );

            size_t getAttCount();
            std::vector< IAttSptr > getAtts();


            IAttSptr getAtt( const std::string& key );

            int long_name( const std::string& s );
            int description( const std::string& s );
            int units( const std::string& s );


            netCDF::NcVar _nc_var;

            NAME_ATT_MAP _name_att_map;
        private:

            VarImpl( IGroupSptr parent, const std::string& name, const netCDF::NcVar& nc_var ) ;

            template< typename T >
                IVarAttSptr putAttImpls( const std::string& key
                        , size_t nvals, const T* values );

            template< typename T >
                IVarAttSptr putAttImpl( const std::string& key, const T& value );

            template< typename T >  
                void putVarImpl( const std::vector< size_t >& index, T* values ) const;

            template< typename T >  
                void putVarImpl( const std::vector< size_t >& index
                        , const std::vector< size_t >& countp
                        , T* values ) const;

            std::weak_ptr< IGroup > _parent;


    };
    
    class GroupAtt : public IGroupAtt
    {
        public:
            typedef std::shared_ptr< GroupAtt > sptr;

            GroupAtt() = delete;
            static GroupAtt::sptr create( GroupImplSptr grp_ptr, const netCDF::NcGroupAtt& ncatt );

            DataType getAttType();
            int getClassType(){ return ATTRIBUTE_GROUP_TYPE; }
            size_t getAttLength() { return _ncatt.getAttLength();}

            int getValues( std::string& values );

            int getValues( short* values );
            int getValues( int* values );
            int getValues( long* values );
            int getValues( long long* values );
            int getValues( unsigned short* values );
            int getValues( unsigned int* values );
            int getValues( unsigned long* values );
            int getValues( unsigned long long* values );

            int getValues( float* values );
            int getValues( double* values );

            IGroupPtr getParentGroup();
            std::string getName();

        private:
            template< typename T>
                int getValuesImpl( T* values );

            GroupAtt( GroupImplSptr grp_ptr, const netCDF::NcGroupAtt& ncatt );

            netCDF::NcGroupAtt _ncatt;

            std::weak_ptr< GroupImpl > _parent_group_ptr;
    };
    
    class DimensionImpl : public IDimension
    {
        public:
            typedef std::shared_ptr< DimensionImpl > sptr;
            friend class GroupImpl;

            DimensionImpl() = delete;

            static sptr create( IGroupSptr parent, const netCDF::NcDim& nc_dim );

            size_t getSize();
            std::string getName();
            IGroup::sptr getParentGroup();

            bool isUnlimited();


        private:
            DimensionImpl( IGroup::sptr parent, const netCDF::NcDim& nc_dim );

            netCDF::NcDim _nc_dim;
            std::weak_ptr< IGroup > _parent;
    };


    class GroupImpl
        : public std::enable_shared_from_this< GroupImpl >
          ,  public IGroup
    {
        public:
            virtual ~GroupImpl(){}

            typedef std::shared_ptr< GroupImpl > sptr;

            std::string getName();

            //int getGroupCount();
            IGroup::sptr parent();

            IGroup::sptr rootGroup();

            IGroup::sptr getGroup( const std::string& name );


            std::vector< IGroup::sptr > getGroups();
            std::vector< IGroup::sptr > _getGroups();

            IGroup::sptr requireGroup( const std::string& name );


            IDimension::sptr addDim( const std::string& name, size_t sz );
            IDimension::sptr addUnlimitedDim( const std::string& name );

            IDimension::sptr getDim( const std::string& name );

            IVar::sptr addVar( const std::string& name, DataType dataType
                    , std::vector< IDimension::sptr >& vdims );

            IVar::sptr getVar( const std::string& name );
            std::vector< std::string > getVars();

            int long_name( const std::string& s );
            int description( const std::string& s );



            virtual size_t getAttCount();

            IAtt::sptr getAtt( const std::string& key );

            std::vector< IAtt::sptr > getAtts();

            IAtt::sptr putAttShort( const std::string& key, short value );
            IAtt::sptr putAttInt( const std::string& key, int value );
            IAtt::sptr putAttLong( const std::string& key, long value );
            IAtt::sptr putAttLongLong( const std::string& key, long long value );

            IAtt::sptr putAttUShort( const std::string& key, unsigned short value );
            IAtt::sptr putAttUInt( const std::string& key, unsigned int value );

            IAtt::sptr putAttString( const std::string& key, const std::string& value );
            IAtt::sptr putAttFloat( const std::string& key, float value );
            IAtt::sptr putAttDouble( const std::string& key, double value );

            IAtt::sptr putAttShorts( const std::string& key, size_t nvals, const short* values );
            IAtt::sptr putAttInts( const std::string& key, int nvals, const int* values );
            IAtt::sptr putAttLongs( const std::string& key, size_t nvals, const long* values );
            IAtt::sptr putAttLongLongs( const std::string& key, size_t nvals, const long long* values );

            IAtt::sptr putAttUShorts( const std::string& key, size_t nvals, const unsigned short* values );
            IAtt::sptr putAttUInts( const std::string& key, size_t nvals, const unsigned int* values );
            IAtt::sptr putAttULongs( const std::string& key, size_t nvals, const unsigned long* values );
            IAtt::sptr putAttULongLongs( const std::string& key, size_t nvals, const unsigned long long* values );

            IAtt::sptr putAttFloats( const std::string& key, size_t nvals, const float* values );
            IAtt::sptr putAttDoubles( const std::string& key, size_t nvals, const double* values );

            IAtt::sptr putAttStrs( const std::string& key, size_t nvals, const char** values );

            IAtt::sptr putAttVal( const std::string& key, DataType datatyp, const void* pv );
            IAtt::sptr putAttVals( const std::string& key
                    , DataType datatype, size_t nvals, const void* pv );

            //int prepare( const CoordinatVariableDefinition& cvd );
            //int prepare( const TableDefinition& td, ITable::sptr& table_ptr );
            //int prepare( const DimensionalVariableDefinition& dvd );


            int addTable( const std::string& name, size_t size, ITable::sptr& table_ptr );
            std::vector< ITable::sptr > getTables();

            ITable::sptr getTable( const std::string& name );
            ITable::sptr _getTable( const std::string& name );

            CompoundTypeImpSptr addCompoundType( const std::string& name, size_t size );


        protected:

            TableImplSptr _addTable( const std::string& name );

            typedef std::map< std::string, IGroup::sptr > NAME_GROUP_MAP;
            typedef std::map< std::string, ITable::sptr > NAME_TABLE_MAP;
            typedef std::map< std::string, IDimension::sptr > NAME_DIM_MAP;
            typedef std::map< std::string, IVar::sptr > NAME_VAR_MAP;
            typedef std::map< std::string, IAtt::sptr > NAME_ATT_MAP;


            static GroupImpl::sptr create( GroupImpl::sptr p, const std::string& name );

            NcGroupPtr _nc;	

            GroupImpl( GroupImpl::sptr grp, NcGroupPtr nc );

            template< typename T >
                IGroupAtt::sptr putAttImpls( const std::string& key
                        , size_t nvals, const T* values );

            std::string _name;

            std::weak_ptr<GroupImpl> _parent;
        private:

            IGroup::sptr _addGroup( const std::string& name );

            IGroup::sptr  _getGroup( const std::string name );


            NAME_GROUP_MAP _name_grp_map;
            NAME_TABLE_MAP _name_table_map;
            NAME_DIM_MAP _name_dim_map;
            NAME_VAR_MAP _name_var_map;

            NAME_ATT_MAP _name_att_map;

    };

    #define FDP_COMPOUND_OFFSET(S,M) (offsetof(S,M))
    

    class CompoundTypeImp
    {
        public:
            typedef CompoundTypeImp this_type;
            typedef std::shared_ptr< this_type > sptr;


            void addMember( const std::string& name, const DataType dtype, size_t offset );
            static sptr create(const netCDF::NcCompoundType& nc){ return sptr( new this_type(nc) );}

        private:
            CompoundTypeImp( const netCDF::NcCompoundType& nc ) : _nc(nc)
            {

            }
            netCDF::NcCompoundType _nc;
    };
}
#endif
