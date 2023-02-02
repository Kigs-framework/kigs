#include "Point2DI.h"
namespace Kigs
{
	namespace Maths
	{
		// +---------
		// | Acces Operators
		// +---------
		template<typename coordType>
		const coordType& Point2DIBase<coordType>::operator[](size_t i) const
		{
			assert(i >= 0 && i < 2);
			return *((&x) + i);
		}
		template<typename coordType>
		coordType& Point2DIBase<coordType>::operator[](size_t i)
		{
			assert(i >= 0 && i < 2);
			return *((&x) + i);
		}
	}
}