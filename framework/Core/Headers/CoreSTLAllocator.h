#ifndef __CORESTLALLOCATOR_H__
#define __CORESTLALLOCATOR_H__

#include "Platform/Core/CorePlatformDefines.h"

#ifndef _NO_KSTL_OVERLOADING_
#ifndef _DO_KSTL_OVERLOADING_
#error "_NO_KSTL_OVERLOADING_ & _DO_KSTL_OVERLOADING_ are both undefined"
#endif

#include "CoreSTLNoException.h"
#include <new>
#include <assert.h>
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
#include "Platform/Core/MemoryManager.h"
#endif
MEMORYMANAGEMENT_END

#ifdef WIN32
#include <xmemory>
#endif

#ifdef __unix__
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef ANDROID
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef RIM
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef _NINTENDO_3DS_
#include <stdlib.h>
#include <stdio.h>
#endif

#if _HAS_EXCEPTIONS
	#define KSTL_DOESNT_THROW() throw()
	#ifndef _DISABLE_HAS_EXCEPTION_WARNING
		#pragma message("*********************PERFORMANCE WARNING : _HAS_EXCEPTIONS IS DEFINED")
	#endif
#else
	#define KSTL_DOESNT_THROW()
#endif


namespace kstl
{

	//utility functions for construction/destruction/allocation
	template<class TYPE> inline TYPE *KSTL_Allocate(size_t count, TYPE *)
	{
	MEMORYMANAGEMENT_START
	#ifndef _NO_MEMORY_MANAGER_
	#ifdef MM_IS_INIT_BY_CORE
		if(MemoryManager::m_first_memory_manager==0)
		{
			MemoryManager::m_OutOfMainAllocCount++;
			return (TYPE*)malloc(count*sizeof(TYPE));
		}
	#endif
		void* result=MemoryManager::m_first_memory_manager->MMmalloc(count*sizeof(TYPE));
		return (TYPE*)result;
	#endif
	MEMORYMANAGEMENT_END
	NO_MEMORYMANAGEMENT_START
	#ifdef _NO_MEMORY_MANAGER_
	return (TYPE*)malloc(count*sizeof(TYPE));
	#endif
	NO_MEMORYMANAGEMENT_END
	}

	inline void KSTL_Deallocate(void *p)
	{
	MEMORYMANAGEMENT_START
	#ifndef _NO_MEMORY_MANAGER_
		#ifdef MM_IS_INIT_BY_CORE
		if(MemoryManager::m_first_memory_manager==0)
		{
			MemoryManager::m_OutOfMainAllocCount--;
			free(p);
			return;
		}
		#endif

		MemoryManager::m_first_memory_manager->MMfree(p);
	#endif
	MEMORYMANAGEMENT_END
	NO_MEMORYMANAGEMENT_START
	#ifdef _NO_MEMORY_MANAGER_
	free(p);
	#endif
	NO_MEMORYMANAGEMENT_END
	}

	template<class POINTED_TYPE, class VALUE_TYPE> inline void KSTL_Construct(POINTED_TYPE *p, const VALUE_TYPE& v)
		{::new ((void*) p) POINTED_TYPE(v);}

	template<class TYPE> inline void KSTL_Destroy(TYPE *p)
		{p->~TYPE();}

	template<> inline void KSTL_Destroy(char *)
	{}
	template<> inline void KSTL_Destroy(wchar_t *)
	{}

	//WEIRD PATCH FOR Visual Studio
	template<class _Ty>	struct CoreSTLAllocator_Base
	{	typedef _Ty value_type;
	};

	template<class _Ty>	struct CoreSTLAllocator_Base<const _Ty>
	{	typedef _Ty value_type;
	};


	// ****************************************
	// * CoreSTLAllocator class
	// * --------------------------------------
	/*!
		Allocator used for STL in kigs framework
	   \ingroup KigsCore
	*/
	// ****************************************

	template<class TYPE>
	class CoreSTLAllocator : public CoreSTLAllocator_Base<TYPE>
	{
	public:
#ifndef WIN32
		typedef TYPE value_type;
#endif
		typedef CoreSTLAllocator<TYPE> _Mybase;
		typedef value_type* pointer;
		typedef value_type& reference;
		typedef const value_type *const_pointer;
		typedef const value_type &const_reference;

		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		//! convert through CoreSTLAllocator< x >
		template<class TYPE2>
		struct rebind
		{
			typedef CoreSTLAllocator<TYPE2> other;
		};

		pointer address(reference Value) const {return (&Value);}

		const_pointer address(const_reference Value) const {return (&Value);}

		CoreSTLAllocator() KSTL_DOESNT_THROW() {}

		CoreSTLAllocator(const CoreSTLAllocator<TYPE>&) KSTL_DOESNT_THROW() {}

		template<class TYPE2> CoreSTLAllocator(const CoreSTLAllocator<TYPE2>&) KSTL_DOESNT_THROW() {}

		template<class TYPE2> CoreSTLAllocator<TYPE>& operator=(const CoreSTLAllocator<TYPE2>&)	{return *this;}

		void deallocate(pointer p, size_type) {KSTL_Deallocate(p);}

		pointer allocate(size_type Count)	{return KSTL_Allocate(Count, (pointer)0);}

		pointer allocate(size_type Count, const void*)	{return (allocate(Count));}

		void construct(pointer p, const TYPE& Value){KSTL_Construct(p, Value);}

		void destroy(pointer p) {KSTL_Destroy(p);}

		size_t max_size() const KSTL_DOESNT_THROW() {return (size_t)(-1);}
	};

	//friend comparison operators
	template<class TYPE, class TYPE2>
	inline bool operator==(const CoreSTLAllocator<TYPE>&, const CoreSTLAllocator<TYPE2>&) KSTL_DOESNT_THROW()
		{return true;}

	template<class TYPE, class TYPE2>
	inline bool operator!=(const CoreSTLAllocator<TYPE>&, const CoreSTLAllocator<TYPE2>&) KSTL_DOESNT_THROW()
		{return (false);}

}
#else

#include <new>
#include <assert.h>
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
#include "Platform/Core/MemoryManager.h"
#endif
MEMORYMANAGEMENT_END

#ifdef WIN32
#include <xmemory>
#endif

#ifdef __gnu_linux__
#include <stdlib.h>
#include <stdio.h>
#endif

#endif// _NO_KSTL_OVERLOADING_

#endif
