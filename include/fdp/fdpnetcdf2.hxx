#ifndef __FDPNETCDF2_HXX__
#define __FDPNETCDF2_HXX__


#include <string>
#include <memory>
#include <vector>


namespace FairDataPipeline
{
    struct IGroup2;
    typedef std::shared_ptr< IGroup2 > IGroup2Sptr;

    struct IGroupAtt2
    {
        typedef std::shared_ptr< IGroupAtt2 > sptr;

        virtual std::string name() = 0;
        virtual DataType getAttType() = 0;

        virtual size_t getAttLength() = 0;

        virtual IGroup2Sptr getParentGroup() = 0;


        virtual int getValues( short* values ) = 0;
        virtual int getValues( int* values ) = 0;
        virtual int getValues( long* values ) = 0;
        virtual int getValues( long long* values ) = 0;

        virtual int getValues( unsigned short* values ) = 0;
        virtual int getValues( unsigned int* values ) = 0;
        virtual int getValues( unsigned long* values ) = 0;
        virtual int getValues( unsigned long long* values ) = 0;

        virtual int getValues( float* values ) = 0;
        virtual int getValues( double* values ) = 0;

    };


	struct IGroup2
	{
		typedef std::shared_ptr< IGroup2 > sptr;

		virtual sptr parent() = 0;
		virtual std::string name() = 0;
		//virtual sptr addGroup( const std::string& grp_name ) = 0;
		virtual sptr getGroup( const std::string& grp_name ) = 0;
		virtual IGroup2::sptr requireGroup( const std::string& grp_name ) = 0;

        virtual std::vector< IGroupAtt2::sptr > getAttributes() = 0;
        virtual IGroupAtt2::sptr getAtt( const std::string& key ) = 0;

        virtual IGroupAtt2::sptr putAtt( const std::string& key, const std::string& value ) = 0;

        virtual IGroupAtt2::sptr putAtt( const std::string& key, short value ) =0;

        virtual IGroupAtt2::sptr putAtt( const std::string& key, int value ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, long value ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, long long value ) = 0;

        virtual IGroupAtt2::sptr putAtt( const std::string& key, unsigned short value ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, unsigned int value ) = 0;
        //virtual IGroupAtt2::sptr putAtt( const std::string& key, unsigned long value );
        virtual IGroupAtt2::sptr putAtt( const std::string& key, unsigned long long value ) = 0;
        
        virtual IGroupAtt2::sptr putAtt( const std::string& key, float value ) = 0;


        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const short* vals ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const int* vals ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const long* vals ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const long long* vals ) = 0;

        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned short* vals ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned int* vals ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned long* vals ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const unsigned long long* vals ) = 0;

        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const float* vals ) = 0;
        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const double* vals ) = 0;

        virtual IGroupAtt2::sptr putAtt( const std::string& key, size_t nvals, const char** vals ) = 0;
	};

	struct IFile2 : public IGroup2
	{
        enum Mode{ READ, WRITE };
        //typedef std::shared_ptr< IFile2 > sptr;
	};

    struct FileFactory2
    {
        IFile2::sptr create( const std::string& path, IFile2::Mode mode );
    };

}


#endif
