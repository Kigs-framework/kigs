#include "CoreFSMTransition.h"
#include "CoreBaseApplication.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(CoreFSMTransition)
IMPLEMENT_CLASS_INFO(CoreFSMOnSignalTransition)
IMPLEMENT_CLASS_INFO(CoreFSMOnEventTransition)
IMPLEMENT_CLASS_INFO(CoreFSMDelayTransition)
IMPLEMENT_CLASS_INFO(CoreFSMOnValueTransition)
IMPLEMENT_CLASS_INFO(CoreFSMOnMethodTransition)
IMPLEMENT_CLASS_INFO(CoreFSMInternalSetTransition)


CoreFSMTransition::CoreFSMTransition(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{

}


void	CoreFSMOnSignalTransition::start()
{
	ParentClassType::start();
	mSignalReceived = false;

	CMSP toConnect = mConnectedClass;
	if (toConnect && ((std::string)mSignal != ""))
	{
		Connect((std::string)mSignal, toConnect, "receiveSignal");
	}
	
}
void	CoreFSMOnSignalTransition::stop()
{
	CMSP toConnect = mConnectedClass;
	if (toConnect && ((std::string)mSignal != ""))
	{
		Disconnect((std::string)mSignal, toConnect, "receiveSignal");
	}

	ParentClassType::stop();
	mSignalReceived = false;
}

bool CoreFSMDelayTransition::checkTransition(CoreModifiable* currentParentClass)
{
	if (!mIsRunning)
	{
		KIGS_ERROR("check a not started transition", 1);
	}
	
	double delay = KigsCore::GetCoreApplication()->GetApplicationTimer()->GetDelay(this);
	if (delay > mDelay)
	{
		return true;
	}
	return false;
}

double CoreFSMDelayTransition::getRemainingTime()
{
	double delay = KigsCore::GetCoreApplication()->GetApplicationTimer()->GetDelay(this);
	return (mDelay - delay);
}

double  CoreFSMDelayTransition::getElapsedTime()
{
	double elapsed = KigsCore::GetCoreApplication()->GetApplicationTimer()->GetDelay(this);
	return elapsed;
}

void	CoreFSMDelayTransition::start()
{
	ParentClassType::start();
	KigsCore::GetCoreApplication()->GetApplicationTimer()->ResetDelay(this);
}




bool CoreFSMOnValueTransition::checkTransition(CoreModifiable* currentParentClass)
{
	if (!mIsRunning)
	{
		KIGS_ERROR("check a not started transition", 1);
	}

	bool result= currentParentClass->getValue<bool>((std::string)mValueName);

	if (mNotValue)
		result = !result;

	return result;

}

bool CoreFSMOnMethodTransition::checkTransition(CoreModifiable* currentParentClass)
{
	if (!mIsRunning)
	{
		KIGS_ERROR("check a not started transition", 1);
	}

	return currentParentClass->SimpleCall((std::string)mMethodName);

}

void	CoreFSMOnEventTransition::start()
{
	if (mIsRunning)
		return;
	ParentClassType::start();
	KigsCore::GetNotificationCenter()->addObserver(this, "EventReceived", (std::string)mEventName);
	mEventReceived = false;
}
void	CoreFSMOnEventTransition::stop()
{
	ParentClassType::stop();
	KigsCore::GetNotificationCenter()->removeObserver(this);
	mEventReceived = false;
}

bool CoreFSMInternalSetTransition::checkTransition(CoreModifiable* currentParentClass)
{
	if (!mIsRunning)
	{
		KIGS_ERROR("check a not started transition", 1);
	}

	return false;
}
