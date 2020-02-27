#pragma once

#include "RefCountedClass.h"

class CoreModifiable;

#define DEFINE_UPGRADOR_METHOD(upgrador,name) DEFINE_METHOD(upgrador##::UpgradorMethods,name)

#define ADD_UPGRADOR_METHOD(methodname,callingname) toUpgrade->InsertMethod(#callingname,static_cast<RefCountedClass::ModifiableMethod>(&UpgradorMethods::methodname));


#define UPGRADOR_METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<RefCountedClass::ModifiableMethod>(&UpgradorMethods::name) });

// Need #include "AttributePacking.h"
#define UPGRADOR_METHODS(...) class UpgradorMethods : public baseclass \
{ \
 public:\
FOR_EACH(DECLARE_METHOD, __VA_ARGS__)\
};\
public:\
virtual void GetMethodTable(kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& table) override\
{\
	FOR_EACH(UPGRADOR_METHOD_PUSH_BACK, __VA_ARGS__)\
}

class UpgradorBase
{
protected:
	friend class CoreModifiable;
	virtual void GetMethodTable(kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& table) = 0;
	void UpgradeInstance(CoreModifiable* toUpgrade);
	void DowngradeInstance(CoreModifiable* toDowngrade);
	UpgradorBase* myNextUpgrador = nullptr;
	struct UpgradorData
	{

	};

	UpgradorData* myData = nullptr;

	// create and init UpgradorData if needed and add dynamic attributes
	virtual void	InitData(CoreModifiable* toUpgrade) { ; }

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	RemoveData(CoreModifiable* toDowngrade) {;}

}; 

template<typename baseclass>
class Upgrador : public UpgradorBase
{
protected:

	typedef baseclass baseclass;

public:


};