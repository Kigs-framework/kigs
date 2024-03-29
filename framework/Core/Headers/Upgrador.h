#pragma once

#include "CoreTypes.h"
#include "CoreModifiable.h"

namespace Kigs
{
	namespace Time
	{
		class Timer;
	}
	namespace Core
	{
		using namespace Kigs::Time;

#define DEFINE_UPGRADOR_METHOD(upgrador,name) DEFINE_METHOD(upgrador::UpgradorMethods,name)

#define ADD_UPGRADOR_METHOD(methodname,callingname) toUpgrade->InsertMethod(#callingname,static_cast<CoreModifiable::ModifiableMethod>(&UpgradorMethods::methodname));


#define UPGRADOR_METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<CoreModifiable::ModifiableMethod>(&UpgradorMethods::name) });

		// Need #include "AttributePacking.h"
#define UPGRADOR_METHODS(...) class UpgradorMethods : public currentBaseClass \
{ \
 public:\
	UpgradorType* GetUpgrador(){return (UpgradorType*)CoreModifiable::GetUpgrador();}\
	bool UpgradorUpdate(const Timer& timer, void* addParam);\
FOR_EACH(DECLARE_METHOD, __VA_ARGS__)\
};\
public:\
bool	UpgradorUpdate(CoreModifiable* toUpdate, const Timer& timer, void* addParam) override\
{\
	return ((UpgradorMethods*)toUpdate)->UpgradorUpdate(timer, addParam);\
}\
virtual void GetMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
{\
	FOR_EACH(UPGRADOR_METHOD_PUSH_BACK, __VA_ARGS__)\
}

#define UPGRADOR_WITHOUT_METHODS() class UpgradorMethods : public currentBaseClass \
{ \
 public:\
	UpgradorType* GetUpgrador(){return (UpgradorType*)CoreModifiable::GetUpgrador();}\
	bool UpgradorUpdate(const Timer& timer, void* addParam);\
};\
public:\
bool	UpgradorUpdate(CoreModifiable* toUpdate, const Timer& timer, void* addParam) override\
{\
	return ((UpgradorMethods*)toUpdate)->UpgradorUpdate(timer, addParam);\
}\
virtual void GetMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
{\
}

#define START_UPGRADOR(name) \
	static inline const KigsID	m_ID = #name;\
	const KigsID& getID() const override {return m_ID; };\
	typedef name UpgradorType; 

#define DEFINE_UPGRADOR_UPDATE(upgrador) bool upgrador::UpgradorMethods::UpgradorUpdate(const Timer& timer, void* addParam) 


		class UpgradorBase : public StructLinkedListBase
		{
		protected:
			friend class CoreModifiable;
			virtual void GetMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) = 0;
			void UpgradeInstance(CoreModifiable* toUpgrade, bool reinit = true, bool attachmethod = true);
			void DowngradeInstance(CoreModifiable* toDowngrade, bool dodestroy = true, bool detachmethod = true);
			// create and init Upgrador if needed, add dynamic attributes, connect things
			virtual void	Init(CoreModifiable* toUpgrade) { mIsInit = 1; }

			// destroy Upgrador and remove dynamic attributes, disconnect things 
			virtual void	Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted = false) { mIsInit = 0; }

			bool	matchFlag(const u32 flags)
			{
				if ((flags & mFlags) == flags)
				{
					return true;
				}
				return false;
			}

		public:
			virtual ~UpgradorBase()
			{
				// should be managed by CoreModifiable
				/*if (mNextItem)
				{
					delete mNextItem;
					mNextItem = nullptr;
				}*/
			}
			virtual bool	UpgradorUpdate(CoreModifiable* toUpdate, const Timer& timer, void* addParam) = 0;

			virtual const KigsID& getID() const = 0;

			u32	mIsInit : 1 = 0;
			u32	mFlags : 31 = 0;
		};

		template<typename baseclass>
		class Upgrador : public UpgradorBase
		{
		protected:

			typedef baseclass currentBaseClass;

		public:

		};

	}
}