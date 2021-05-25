#include <Sample7.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>
#include <iostream>

// Kigs framework Sample7 project
// Lua Binding features :
// - LuaBehaviour class
// - LuaImporter class
// - Adding lua method to CoreModifiable instance
// - Calling CoreModifiable method from Lua code
// - ...

IMPLEMENT_CLASS_INFO(Sample7);

IMPLEMENT_CONSTRUCTOR(Sample7)
{

}

void	Sample7::ProtectedInit()
{
	// Base modules have been created at this point

	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	SP<FilePathManager> pathManager = KigsCore::Singleton<FilePathManager>();
	pathManager->AddToPath(".", "xml");

	// Load AppInit, GlobalConfig then launch first sequence
	DataDrivenBaseApplication::ProtectedInit();

	// create a LuaBehaviour instance and aggregate it to this
	CMSP behaviour = KigsCore::GetInstanceOf("behaviour", "LuaBehaviour");
	behaviour->setValue("Script", "#ScriptOnApp.lua");
	aggregateWith(behaviour);
	behaviour->Init();

}

void	Sample7::ProtectedUpdate()
{
	DataDrivenBaseApplication::ProtectedUpdate();
}

void	Sample7::ProtectedClose()
{
	DataDrivenBaseApplication::ProtectedClose();
}

void	Sample7::ProtectedInitSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{

	}
}
void	Sample7::ProtectedCloseSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{
		
	}
}

// method called from Lua script 
void	Sample7::HelloFromLua()
{
	std::cout << "Lua called me" << std::endl;

	static int counter = 0;
	++counter;
	if ((counter % 500) == 0)
	{
		// call Lua method on Interface
		CMSP instance = GetFirstInstanceByName("UIItem", "Interface");
		if (instance)
		{
			instance->SimpleCall("reset",GetApplicationTimer()->GetTime());
		}
	}

	if (counter > 4000)
	{
		mNeedExit = true;
	}
}
