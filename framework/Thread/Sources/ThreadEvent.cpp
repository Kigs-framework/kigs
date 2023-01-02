#include "PrecompiledHeaders.h"
#include "ThreadEvent.h"

using namespace Kigs::Thread;

IMPLEMENT_CLASS_INFO(ThreadEvent)

ThreadEvent::ThreadEvent(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mEventCounter(*this, false, "EventCounter", 1)
, mCurrentCount(0)
, mAutoReset(*this, false, "AutoReset", false)
{
	mCriticalSection = new std::mutex();
}

ThreadEvent::~ThreadEvent()
{
	if (mOwnCriticalSection)
	{
		delete mCriticalSection;
		mCriticalSection = nullptr;
	}
}



void	ThreadEvent::wait()
{
	std::unique_lock<std::mutex> lk(*mCriticalSection);
	mConditionVariable.wait(lk, [this]
		{
			while (mCurrentCount < mEventCounter)
				return false;
			if (mAutoReset)
				mCurrentCount = 0;
			return true;
		});

}
void	ThreadEvent::signal()
{
	{
		std::lock_guard<std::mutex> lk(*mCriticalSection);
		mCurrentCount++;
	}
	mConditionVariable.notify_all();
}