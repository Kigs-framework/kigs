#include <Sample6.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>
#include "SimpleClass.h"
#include <iostream>

IMPLEMENT_CLASS_INFO(Sample6);

IMPLEMENT_CONSTRUCTOR(Sample6)
{

}

void	Sample6::ProtectedInit()
{
	// Base modules have been created at this point

	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	SP<FilePathManager>& pathManager = KigsCore::Singleton<FilePathManager>();
	pathManager->AddToPath(".", "xml");

	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), SimpleClass, SimpleClass, Application);

	// Load AppInit, GlobalConfig then launch first sequence
	DataDrivenBaseApplication::ProtectedInit();

	CMSP simpleclass = KigsCore::GetInstanceOf("simpleclass", "SimpleClass");
	simpleclass->Init();
	addItem(simpleclass);

	// add an observer on notification "doSomethingElseNotif" 
	// call CatchNotifMethod when notif is received
	KigsCore::GetNotificationCenter()->addObserver(this,"CatchNotifMethod","doSomethingElseNotif");

	KigsCore::Connect(simpleclass.get(),"SendSignal1",this, "MethodWithParams");

	// connect to lambda function
	KigsCore::Connect(simpleclass.get(), "SendSignal2", this, "lambda", [this](int p1)
		{
			std::cout << "lambda received parameter " << p1 << std::endl;
		});


}



// member method that catch notif 
DEFINE_METHOD(Sample6, CatchNotifMethod)
{

	std::cout << "Sample6 CatchNotifMethod method" << std::endl;

	if (sender)
		std::cout << "-- sender : " << sender->getName() << std::endl;

	for (auto p : params)
	{
#ifdef KEEP_NAME_AS_STRING
		std::string v;
		if (p->getValue(v))
		{
			std::cout << "-- parameter : " << p->getID()._id_name << " value is : " << v << std::endl;
		}
		else
		{
			std::cout << "-- parameter : " << p->getID()._id_name << " value cannot be evaluated as string" << std::endl;
		}
#else
		std::string v;
		if (p->getValue(v))
		{
			std::cout << "-- parameter : " << p->getID()._id << " value is : " << v << std::endl;
		}
		else
		{
			std::cout << "-- parameter : " << p->getID()._id << " value cannot be evaluated as string" << std::endl;
		}
#endif
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
	DataDrivenBaseApplication::ProtectedUpdate();

	// emit a "runtime" signal (not declared with the SIGNALS macro) 
	Emit("doSomething");
}

void	Sample6::ProtectedClose()
{
	DataDrivenBaseApplication::ProtectedClose();
}

void	Sample6::ProtectedInitSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{

	}
}
void	Sample6::ProtectedCloseSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{
		
	}
}



