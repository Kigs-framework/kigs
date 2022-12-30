#include <Sample8.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>

#include "UI/UIItem.h"

using namespace Kigs;

IMPLEMENT_CLASS_INFO(Sample8);

IMPLEMENT_CONSTRUCTOR(Sample8)
{

}

void	Sample8::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	SP<File::FilePathManager> pathManager = KigsCore::Singleton<File::FilePathManager>();
	pathManager->AddToPath(".", "xml");
	
	// Load AppInit, GlobalConfig then launch first sequence
	DataDrivenBaseApplication::ProtectedInit();
}

void	Sample8::ProtectedUpdate()
{
	DataDrivenBaseApplication::ProtectedUpdate();
}

void	Sample8::ProtectedClose()
{
	DataDrivenBaseApplication::ProtectedClose();
}

void	Sample8::ProtectedInitSequence(const std::string& sequence)
{
	if (sequence == "sequencemain")
	{

	}
}
void	Sample8::ProtectedCloseSequence(const std::string& sequence)
{
	if (sequence == "sequencemain")
	{
		
	}
}

