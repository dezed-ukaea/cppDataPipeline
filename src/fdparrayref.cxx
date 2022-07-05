#include "fdp/fdparrayref.hxx"

namespace FairDataPipeline
{
	namespace Array
	{
		template<>
			Shape make_shape<size_t>( size_t* shape, size_t nshape )
			{
				return Shape( shape, nshape );
			}
	}
}

