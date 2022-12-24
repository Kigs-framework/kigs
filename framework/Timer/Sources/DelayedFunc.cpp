#include "DelayedFunc.h"
#include "CoreBaseApplication.h"

using namespace Kigs::Time;

// connect to events and create attributes
void	DelayedFuncUpgrador::Init(CoreModifiable* toUpgrade)
{

	mDelayedFunction = (maString*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, "DelayedFunction", "");
	mDelay = (maFloat*)toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT, "Delay", 1.0f);

	// check if already in auto update mode
	if (toUpgrade->isFlagAsAutoUpdateRegistered())
		mWasdAutoUpdate = true;
	else
		KigsCore::GetCoreApplication()->AddAutoUpdate(toUpgrade);

}

//  remove dynamic attributes and disconnect events
void	DelayedFuncUpgrador::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;

	toDowngrade->RemoveDynamicAttribute("DelayedFunction");
	mDelayedFunction = nullptr;

	toDowngrade->RemoveDynamicAttribute("Delay");
	mDelay = nullptr;

	if (!mWasdAutoUpdate)
		KigsCore::GetCoreApplication()->RemoveAutoUpdate(toDowngrade);
}




bool	DelayedFuncUpgrador::Update(const Timer& _timer, CoreModifiable* parent)
{
	if (mInitTime < 0.0)
	{
		mInitTime=_timer.GetTime();
		return false;
	}
	std::string func = (*mDelayedFunction);
	if (func.length() && (mInitTime>=0.0))
	{
		double currentTime = _timer.GetTime();
		double L_delta = currentTime - mInitTime;
		float delay = (float)(*mDelay);

		if (L_delta > delay)
		{
			parent->SimpleCall(func);
			return true; // ask for remove and downgrade
			//parent->Downgrade("DelayedFuncUpgrador");
		}
	}
	return false;
}

DEFINE_UPGRADOR_UPDATE(DelayedFuncUpgrador)
{
	DelayedFuncUpgrador* currentDelayer = static_cast<DelayedFuncUpgrador*>(GetUpgrador());
	return currentDelayer->Update(timer, this);
}
