#pragma once
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"

#include "Upgrador.h"

class SimpleClass : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(SimpleClass, CoreModifiable, Application);
	DECLARE_CONSTRUCTOR(SimpleClass);

	void	DoSomethingFun();

protected:

	virtual void NotifyUpdate(const u32 labelid) override;

	maInt			m_IntValue;
	maString		m_StringValue;
	maReference		m_Ref = BASE_ATTRIBUTE(Reference,"");

	maVect4DF		m_Vector4D = BASE_ATTRIBUTE(Vector, 0.0,0.0,1.0,2.0);
};

class UpgradeSimple : public Upgrador<SimpleClass>
{
protected:
	START_UPGRADOR(UpgradeSimple);
	UPGRADOR_METHODS(DoSomethingElse);

	// create and init Upgrador if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade) override;

	
	int myTestData;

public:


};

class OtherUpgradeSimple : public Upgrador<SimpleClass>
{
protected:
	START_UPGRADOR(OtherUpgradeSimple);
	UPGRADOR_METHODS(DoSomethingMore);

	// create and init UpgradorData if needed and add dynamic attributes
	virtual void	Init(CoreModifiable* toUpgrade) override;

	// destroy UpgradorData and remove dynamic attributes 
	virtual void	Destroy(CoreModifiable* toDowngrade) override;

public:


};