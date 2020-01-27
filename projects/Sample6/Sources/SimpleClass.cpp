#include "SimpleClass.h"
#include "CoreModifiableAttribute.h"
#include "NotificationCenter.h"
#include <iostream>

IMPLEMENT_CLASS_INFO(SimpleClass)

IMPLEMENT_CONSTRUCTOR(SimpleClass)
{
	
}

DEFINE_METHOD(SimpleClass, doSomething)
{
	std::cout << "doSomething method called" << std::endl;

	// emit signal with two parameters
	EmitSignal(Signals::SendSignal1,32,64);

	EmitSignal(Signals::SendSignal2, 32, 64);

	return true;
}

void	SimpleClass::doSomethingElse()
{
	std::cout << "doSomethingElse method called" << std::endl;

	// post a notification "doSomethingElseNotif" 
	KigsCore::GetNotificationCenter()->postNotificationName("doSomethingElseNotif", this);
}


void SimpleClass::InitModifiable()
{
	ParentClassType::InitModifiable();

	// retrieve app 
	CoreModifiable* app=(CoreModifiable * )KigsCore::GetCoreApplication();
	// connect app Update signal to doSomethingElse method
	KigsCore::Connect(app, "Update", this, "doSomethingElse");

	// connect app undeclared doSomething signal to doSomething method
	KigsCore::Connect(app, "doSomething", this, "doSomething");

}