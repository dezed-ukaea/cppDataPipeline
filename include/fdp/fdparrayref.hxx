#ifndef FDP_ARRARYREF_H__
#define FDP_ARRARYREF_H__
#include<cstdio>
#include<vector>
namespace FairDataPipeline
{
	namespace Array
	{
		class Shape
		{
			public:
				Shape( size_t* shape, size_t nshape ) : _shape( shape, shape+nshape )
			{
			}
				Shape( const Shape& rhs ) : _shape( rhs._shape )
			{
			}

				const std::vector< size_t >& shape() const
				{
					return _shape;
				}

				size_t rank(){ return _shape.size();}


			private:
				std::vector< size_t > _shape;
		};
		template< typename T >
			Shape make_shape( T* shape, size_t nshape )
			{
				std::vector< size_t > vshape( shape, shape+nshape );

				return Shape( vshape.data(), nshape );
			}

		template< typename T >
			class ArrayRef
			{
				public:
					ArrayRef( const T* _data_, const Shape& shape ) 
						: _data( _data_ )
						  , _shape( shape )
				{
				}

					const T* data()
					{
						return _data;
					}

					const Shape & shape()
					{
						return _shape;
					}

				private:
					const T* _data;
					Shape _shape;
			};
	}
}
#endif

