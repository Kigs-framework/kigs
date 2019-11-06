#ifndef __KSTLMAP_H__
#define __KSTLMAP_H__

#include "Platform/Core/CorePlatformDefines.h"
#include <map>
#ifndef _NO_KSTL_OVERLOADING_

#include "CoreSTLAllocator.h"
namespace kstl
{
	template<typename KEY_TYPE, typename VALUE_TYPE, typename PREDICATE_TYPE=std::less<KEY_TYPE>>
	using map = std::map<KEY_TYPE, VALUE_TYPE, PREDICATE_TYPE, CoreSTLAllocator<std::pair<const KEY_TYPE, VALUE_TYPE>>>;
}

#else //_NO_KSTL_OVERLOADING_

namespace kstl = std;

#endif //_NO_KSTL_OVERLOADING_

#endif
