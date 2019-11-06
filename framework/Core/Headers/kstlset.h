#ifndef __KSTLSET_H__
#define __KSTLSET_H__

#include "Platform/Core/CorePlatformDefines.h"

#include <set>

#ifndef _NO_KSTL_OVERLOADING_
#include "CoreSTLAllocator.h"
namespace kstl
{
	template<typename T, typename PREDICATE_TYPE = std::less<T>>
	using set = std::set<T, PREDICATE_TYPE, CoreSTLAllocator<T>>;
}
#else //_NO_KSTL_OVERLOADING_
namespace kstl = std;
#endif //_NO_KSTL_OVERLOADING_

#endif
