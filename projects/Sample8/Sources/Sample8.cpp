#include <Sample8.h>
#include <FilePathManager.h>
#include <NotificationCenter.h>
#include "GenericAnimationModule.h"
#include "AObject.h"

IMPLEMENT_CLASS_INFO(Sample8);

IMPLEMENT_CONSTRUCTOR(Sample8)
{

}

void	Sample8::ProtectedInit()
{
	// Base modules have been created at this point

	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	theGenericAnimationModule=CoreCreateModule(GenericAnimationModule, 0);

	SP<FilePathManager>& pathManager = KigsCore::Singleton<FilePathManager>();
	pathManager->AddToPath(".", "xml");



	// Load AppInit, GlobalConfig then launch first sequence
	DataDrivenBaseApplication::ProtectedInit();
}

void	Sample8::ProtectedUpdate()
{
	DataDrivenBaseApplication::ProtectedUpdate();
	theGenericAnimationModule->Update(*GetApplicationTimer().get(), nullptr);
}

void	Sample8::ProtectedClose()
{
	DataDrivenBaseApplication::ProtectedClose();
}

void	Sample8::ProtectedInitSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{
		AnimationResourceInfo* frogAnim = theGenericAnimationModule->LoadAnimation("Tree_frog.anim");

		AObject* ao=(AObject * )GetFirstInstanceByName("AObject", "animatedObject");

		// create streams with the good type and add them to channels
//		ao->AddAnimation(frogAnim);

		// start playing streams
//		ao->StartAnimation(frogAnim, myApplicationTimer->GetTime());
	}
}
void	Sample8::ProtectedCloseSequence(const kstl::string& sequence)
{
	if (sequence == "sequencemain")
	{
		
	}
}

