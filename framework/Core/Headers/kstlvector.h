#ifndef __KSTLVECTOR__
#define __KSTLVECTOR__

#include "Platform/Core/CorePlatformDefines.h"

#include <vector>


#ifndef _NO_KSTL_OVERLOADING_

#include "CoreSTLAllocator.h"

namespace kstl
{
	template<typename T>
	using vector = std::vector<T, CoreSTLAllocator<T>>;
}

#else //_NO_KSTL_OVERLOADING_
#include <vector>

namespace kstl = std;

#endif //_NO_KSTL_OVERLOADING_

#endif
