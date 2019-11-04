#ifndef __CORESTLNOEXCEPTION_H__
#define __CORESTLNOEXCEPTION_H__

#include "Platform/Core/CorePlatformDefines.h"

#ifndef _NO_KSTL_OVERLOADING_
#ifndef _DO_KSTL_OVERLOADING_
#error "_NO_KSTL_OVERLOADING_ & _DO_KSTL_OVERLOADING_ are both undefined"
#endif
////////////////////////////////////////////////////////////////////
///This file must be included at top of your precompiled header files
///To disable performance-consuming exception handling

#if defined(_MSC_VER) && !defined(_DISABLE_MS_VISUAL_STUDIO_KSTL_EXCEPTION_OPTIMS)
	#undef _HAS_EXCEPTIONS
	#define _HAS_EXCEPTIONS 0
	#include <xstddef>
	#undef _THROW
	#undef _THROW_NCEE
	#undef _RAISE
	#define _THROW(x, y) ((void)0)
	#define _THROW_NCEE(x, y) ((void)0)
	#define _RAISE(x) ((void)0)
#else
	#define _HAS_EXCEPTIONS 0
#endif

#endif	//_NO_KSTL_OVERLOADING_

#endif
