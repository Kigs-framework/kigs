#pragma once

#include "RefCountedClass.h"


class CoreModifiable;
class Timer;

#define DEFINE_UPGRADOR_METHOD(upgrador,name) DEFINE_METHOD(upgrador::UpgradorMethods,name)

#define ADD_UPGRADOR_METHOD(methodname,callingname) toUpgrade->InsertMethod(#callingname,static_cast<RefCountedClass::ModifiableMethod>(&UpgradorMethods::methodname));


#define UPGRADOR_METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<RefCountedClass::ModifiableMethod>(&UpgradorMethods::name) });

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
virtual void GetMethodTable(kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& table) override\
{\
	FOR_EACH(UPGRADOR_METHOD_PUSH_BACK, __VA_ARGS__)\
}

#define START_UPGRADOR(name) \
	static inline const KigsID	m_ID = #name;\
	const KigsID& getID() const override {return m_ID; };\
	typedef name UpgradorType; 

#define DEFINE_UPGRADOR_UPDATE(upgrador) void upgrador::UpgradorMethods::UpgradorUpdate(const Timer& timer, void* addParam) 


class UpgradorBase
{
protected:
	friend class CoreModifiable;
	virtual void GetMethodTable(kstl::vector<std::pair<KigsID, RefCountedClass::ModifiableMethod>>& table) = 0;
	void UpgradeInstance(CoreModifiable* toUpgrade);
	void DowngradeInstance(CoreModifiable* toDowngrade);
	UpgradorBase* myNextUpgrador = nullptr;
	// create and init Upgrador if needed, add dynamic attributes, connect things
	virtual void	Init(CoreModifiable* toUpgrade) { ; }

	// destroy Upgrador and remove dynamic attributes, disconnect things 
	virtual void	Destroy(CoreModifiable* toDowngrade) {;}

public:
	virtual ~UpgradorBase()
	{
		if (myNextUpgrador)
		{
			delete myNextUpgrador;
			myNextUpgrador = nullptr;
		}
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