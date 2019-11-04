#ifndef __KSTLLIST_H__
#define __KSTLLIST_H__

#include "Platform/Core/CorePlatformDefines.h"
#include <list>

#ifndef _NO_KSTL_OVERLOADING_
#include "CoreSTLAllocator.h"
namespace kstl
{
	template<typename T>
	using list = std::list<T, CoreSTLAllocator<T>>;
}
#else //_NO_KSTL_OVERLOADING_

namespace kstl = std;

#endif //_NO_KSTL_OVERLOADING_

#endif
