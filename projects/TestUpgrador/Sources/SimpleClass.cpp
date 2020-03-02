#include "SimpleClass.h"
#include <iostream>

IMPLEMENT_CLASS_INFO(SimpleClass)

IMPLEMENT_CONSTRUCTOR(SimpleClass)
// attribute is set as init attribute : it will be set as readonly after instance was initialized
, m_IntValue(*this,true,"IntValue",5)					
// attribute is not set as init attribute : it will be read/write enabled
, m_StringValue(* this, false, "StringValue")
{
	// this will be notified when StringValue is changed
	m_StringValue.changeNotificationLevel(Owner);
}

void SimpleClass::NotifyUpdate(const u32 labelid)
{
	if (labelid == m_StringValue.getID())
	{
		std::cout << "StringValue new value is : " <<  m_StringValue.const_ref() << std::endl;
	}
}

void	SimpleClass::DoSomethingFun()
{
	if ((CoreModifiable*)m_Ref) // check that m_Ref point to an existing instance
	{
		CoreModifiable* other = (CoreModifiable*)m_Ref;
		if (other->isSubType(SimpleClass::myClassID)) // if other is also a SimpleClass instance
		{
			std::cout << "referenced instance IntValue = " << other->getValue<int>("IntValue") << std::endl;
		}
		else
		{
			std::cout << "referenced instance with name " << other->getName() <<" is not a SimpleClass" << std::endl;
		}

		// if an attribute named "DynamicAttribute" is found, print its value
		std::string dynattr;
		if (other->getValue("DynamicAttribute", dynattr)) 
		{
			std::cout << "DynamicAttribute found on " << other->getName() << " with value : " << dynattr << std::endl;
		}
			 
	}
}

DEFINE_UPGRADOR_UPDATE(UpgradeSimple)
{
	GetUpgrador()->myTestData += 2;
}

DEFINE_UPGRADOR_METHOD(UpgradeSimple, DoSomethingElse)
{

	std::cout << "DoSomethingElse" << std::endl;

	// access SimpleClass data
	std::cout << "Simple class m_StringValue = " << m_StringValue.c_str() << std::endl;

	// access localdata 
	std::cout << "UpgradeSimple myTestData = " << GetUpgrador()->myTestData << std::endl;

	// access dynamic upgrade data 
	std::cout << "UpgradorInt = " << getValue<int>("UpgradorInt") << std::endl;


	return false;
}

DEFINE_UPGRADOR_METHOD(OtherUpgradeSimple, DoSomethingMore)
{

	std::cout << "DoSomethingMore" << std::endl;

	// access SimpleClass data
	std::cout << "Simple class m_StringValue = " << m_StringValue.c_str() << std::endl;

	return false;
}


// create and init UpgradorData if needed and add dynamic attributes and connections
void	UpgradeSimple::Init(CoreModifiable* toUpgrade) 
{
	myTestData = 12;
	toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::INT, "UpgradorInt", 56);
}

// destroy UpgradorData and remove dynamic attributes 
void	UpgradeSimple::Destroy(CoreModifiable* toDowngrade)
{
	toDowngrade->RemoveDynamicAttribute("UpgradorInt");
}

// create and init UpgradorData if needed and add dynamic attributes and connections
void	OtherUpgradeSimple::Init(CoreModifiable* toUpgrade)
{

	KigsCore::Connect((CoreModifiable*)KigsCore::GetCoreApplication(), "Signal", toUpgrade, "DoSomethingMore");
}

// destroy UpgradorData and remove dynamic attributes 
void	OtherUpgradeSimple::Destroy(CoreModifiable* toDowngrade)
{
	KigsCore::Disconnect((CoreModifiable*)KigsCore::GetCoreApplication(), "Signal", toDowngrade, "DoSomethingMore");
}

DEFINE_UPGRADOR_UPDATE(OtherUpgradeSimple)
{
	std::cout << "in OtherUpgradeSimple Update" << std::endl;
}