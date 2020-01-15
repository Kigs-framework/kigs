#include <Sample5bis.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>


IMPLEMENT_CLASS_INFO(Sample5bis);

IMPLEMENT_CONSTRUCTOR(Sample5bis)
{

}

void	Sample5bis::ProtectedInit()
{
	// Base modules have been created at this point

	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	SP<FilePathManager> pathManager = KigsCore::GetSingleton("FilePathManager");
	pathManager->AddToPath(".", "xml");


	// Load AppInit, GlobalConfig then launch first sequence
	DataDrivenBaseApplication::ProtectedInit();
}

void	Sample5bis::ProtectedUpdate()
{
	DataDrivenBaseApplication::ProtectedUpdate();

	if (GetApplicationTimer()->GetTime() > 4)
	{
		myNeedExit = true;
	}
}

void	Sample5bis::ProtectedClose()
{
	DataDrivenBaseApplication::ProtectedClose();
}

void	Sample5bis::ProtectedInitSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{

	}
}
void	Sample5bis::ProtectedCloseSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{
		
	}
}

