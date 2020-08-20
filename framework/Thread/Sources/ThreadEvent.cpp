#include "PrecompiledHeaders.h"
#include "ThreadEvent.h"

IMPLEMENT_CLASS_INFO(ThreadEvent)

ThreadEvent::ThreadEvent(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mEventCounter(*this, false, LABEL_AND_ID(EventCounter), 1)
, mCurrentCount(0)
, mAutoReset(*this, false, LABEL_AND_ID(AutoReset), false)
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