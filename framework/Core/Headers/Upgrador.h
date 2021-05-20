#pragma once

#include "CoreModifiable.h"


class CoreModifiable;
class Timer;

#define DEFINE_UPGRADOR_METHOD(upgrador,name) DEFINE_METHOD(upgrador::UpgradorMethods,name)

#define ADD_UPGRADOR_METHOD(methodname,callingname) toUpgrade->InsertMethod(#callingname,static_cast<CoreModifiable::ModifiableMethod>(&UpgradorMethods::methodname));


#define UPGRADOR_METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<CoreModifiable::ModifiableMethod>(&UpgradorMethods::name) });

// Need #include "AttributePacking.h"
#define UPGRADOR_METHODS(...) class UpgradorMethods : public currentBaseClass \
{ \
 public:\
	UpgradorType* GetUpgrador(){return (UpgradorType*)CoreModifiable::GetUpgrador();}\
	void UpgradorUpdate(const Timer& timer, void* addParam);\
FOR_EACH(DECLARE_METHOD, __VA_ARGS__)\
};\
public:\
void	UpgradorUpdate(CoreModifiable* toUpdate, const Timer& timer, void* addParam) override\
{\
	((UpgradorMethods*)toUpdate)->UpgradorUpdate(timer, addParam);\
}\
virtual void GetMethodTable(kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
{\
	FOR_EACH(UPGRADOR_METHOD_PUSH_BACK, __VA_ARGS__)\
}

#define START_UPGRADOR(name) \
	static inline const KigsID	m_ID = #name;\
	const KigsID& getID() const override {return m_ID; };\
	typedef name UpgradorType; 

#define DEFINE_UPGRADOR_UPDATE(upgrador) void upgrador::UpgradorMethods::UpgradorUpdate(const Timer& timer, void* addParam) 


class UpgradorBase : public StructLinkedListBase
{
protected:
	friend class CoreModifiable;
	virtual void GetMethodTable(kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) = 0;
	void UpgradeInstance(CoreModifiable* toUpgrade);
	void DowngradeInstance(CoreModifiable* toDowngrade);
	// create and init Upgrador if needed, add dynamic attributes, connect things
	virtual void	Init(CoreModifiable* toUpgrade) { ; }

	// destroy Upgrador and remove dynamic attributes, disconnect things 
	virtual void	Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted=false) {;}

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
	virtual void	UpgradorUpdate(CoreModifiable* toUpdate, const Timer& timer, void* addParam) = 0;

	virtual const KigsID& getID() const = 0;
}; 

template<typename baseclass>
class Upgrador : public UpgradorBase
{
protected:

	typedef baseclass currentBaseClass;

public:


	

};