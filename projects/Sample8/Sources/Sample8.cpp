#include <Sample8.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>

#include "UI/UIItem.h"

IMPLEMENT_CLASS_INFO(Sample8);

IMPLEMENT_CONSTRUCTOR(Sample8)
{

}

void	Sample8::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	SP<FilePathManager> pathManager = KigsCore::Singleton<FilePathManager>();
	pathManager->AddToPath(".", "xml");
	
	
	const auto uiitem = sizeof(UIItem);
	const auto node2D = sizeof(Node2D);
	
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

