#ifndef __KSTLUNORDEREDMAP_H__
#define __KSTLUNORDEREDMAP_H__

#include "Platform/Core/CorePlatformDefines.h"
#include <unordered_map>
#ifndef _NO_KSTL_OVERLOADING_

#include "CoreSTLAllocator.h"
namespace kstl
{
	template<typename KEY_TYPE, typename VALUE_TYPE, class HASHER = std::hash<KEY_TYPE>, class KEYEQ = std::equal_to<KEY_TYPE>>
	using unordered_map = std::unordered_map<KEY_TYPE, VALUE_TYPE, HASHER, KEYEQ, CoreSTLAllocator<std::pair<const KEY_TYPE, VALUE_TYPE>>>;
}

#else //_NO_KSTL_OVERLOADING_

namespace kstl = std;

#endif //_NO_KSTL_OVERLOADING_

#endif //__KSTLUNORDEREDMAP_H__
