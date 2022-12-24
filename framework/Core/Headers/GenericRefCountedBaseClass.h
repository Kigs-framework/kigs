#pragma once

#include <atomic>
#include <vector>
#include <memory>

namespace Kigs
{
	namespace Core
	{
		class CoreModifiable;
		class CoreModifiableAttribute;

		template<typename To, typename From>
		inline std::unique_ptr<To> static_unique_pointer_cast(std::unique_ptr<From>&& old)
		{
			return std::unique_ptr<To>{static_cast<To*>(old.release())};
		}
	}
}

// ****************************************
// * GenericRefCountedBaseClass class
// * --------------------------------------
/**
* \file	GenericRefCountedBaseClass.h
* \class	GenericRefCountedBaseClass
* \ingroup Core
* \brief	Base class for refcounting classes, no other dependencies
*/
// ****************************************

#ifdef _DEBUG
#define GenericRefCountedBaseClassLeakCheck
#endif

#ifdef GenericRefCountedBaseClassLeakCheck
#include <unordered_set>
#include <shared_mutex>
namespace Kigs
{
	namespace Core
	{
		class GenericRefCountedBaseClass;
		inline std::shared_mutex AllObjectsMutex;
		inline std::unordered_set<GenericRefCountedBaseClass*> AllObjects;
	}
}
#endif

namespace Kigs
{
	namespace Core
	{
		class GenericRefCountedBaseClass : public std::enable_shared_from_this<GenericRefCountedBaseClass>
		{
		public:
			typedef bool (GenericRefCountedBaseClass::* ModifiableMethod)(CoreModifiable* sender, std::vector<CoreModifiableAttribute*>&, void* privateParams);
			GenericRefCountedBaseClass()
			{
#ifdef GenericRefCountedBaseClassLeakCheck
				std::lock_guard<std::shared_mutex> lk{ AllObjectsMutex };
				AllObjects.insert(this);
#endif
			}

			virtual ~GenericRefCountedBaseClass()
			{
#ifdef GenericRefCountedBaseClassLeakCheck
				std::lock_guard<std::shared_mutex> lk{ AllObjectsMutex };
				AllObjects.erase(this);
#endif
			};
		};

	}
}
