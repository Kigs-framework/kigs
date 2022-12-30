#include <Sample6.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>
#include "SimpleClass.h"
#include <iostream>

using namespace Kigs;

// Kigs framework Sample6 project
// Signal / Slot / Notifications features :
// - declare signals
// - emit signals
// - connect to signals
// - add/remove notification observer
// - post notification

IMPLEMENT_CLASS_INFO(Sample6);

IMPLEMENT_CONSTRUCTOR(Sample6)
{

}

void	Sample6::ProtectedInit()
{
	SetUpdateSleepTime(1);

	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleClass, SimpleClass, Application);


	// ask instance factory to add a connection on each created SimpleClass 
	// for the PreInit signal to call this OnSimpleClassPreInit
	KigsCore::Instance()->GetInstanceFactory()->addModifiableCallback("PreInit", this, "OnSimpleClassPreInit", "SimpleClass");

	CMSP simpleclass = KigsCore::GetInstanceOf("simpleclass", "SimpleClass");
	simpleclass->Init();
	addItem(simpleclass);

	// get the list of SimpleClass signals
	std::cout << "simpleclass instance has following signals available" << std::endl;
	auto signallist=simpleclass->GetSignalList();
	for (const auto& s : signallist)
	{
		std::cout << s.toString() << std::endl;
	}

	std::cout << std::endl;

	// add an observer on notification "doSomethingElseNotif" 
	// call CatchNotifMethod when notif is received
	KigsCore::GetNotificationCenter()->addObserver(this,"CatchNotifMethod","doSomethingElseNotif");

	KigsCore::Connect(simpleclass.get(),"SendSignal1",this, "MethodWithParams");

	// connect to lambda function
	KigsCore::Connect(simpleclass.get(), "SendSignal2", this, "lambda", [this](int p1)
		{
			std::cout << "lambda received parameter " << p1 << std::endl;
		});


	// remove instance factory auto connection previously set
	KigsCore::Instance()->GetInstanceFactory()->removeModifiableCallback("PreInit", this, "OnSimpleClassPreInit");


}

void    Sample6::OnSimpleClassPreInit()
{
	std::cout << "A simple class instance is about to be initialized" << std::endl;
}



// member method that catch notif 
DEFINE_METHOD(Sample6, CatchNotifMethod)
{
	std::cout << "Sample6 CatchNotifMethod method" << std::endl;

	if (sender)
		std::cout << "-- sender : " << sender->getName() << std::endl;

	for (auto p : params)
	{
		std::string v;
		if (p->getValue(v))
		{
			std::cout << "-- parameter : " << p->getID().toString() << " value is : " << v << std::endl;
		}
		else
		{
			std::cout << "-- parameter : " << p->getID().toString() << " value cannot be evaluated as string" << std::endl;
		}
	}

	if (privateParams)
		std::cout << "-- private parameter is not null" << std::endl;
	else
		std::cout << "-- private parameter is null" << std::endl;

	return true;
}

void	Sample6::MethodWithParams(float p1, float p2)
{
	std::cout << "MethodWithParams received params : " << p1 << "," << p2 <<  std::endl;
}

void	Sample6::ProtectedUpdate()
{

	// emit a "runtime" signal (not declared with the SIGNALS macro) 
	EmitSignal("doSomething");

	// disconnect this so SendSignal1 will not be catched anymore
	CMSP simplecass=GetInstanceByPath("simpleclass");
	KigsCore::Disconnect(simplecass.get(), "SendSignal1", this, "MethodWithParams");

	static int loopcount = 0;
	if (++loopcount > 5)
	{
		mNeedExit = true;
	}
}

void	Sample6::ProtectedClose()
{

}




