#include "PrecompiledHeaders.h"
#include "ThreadEvent.h"

IMPLEMENT_CLASS_INFO(ThreadEvent)

ThreadEvent::ThreadEvent(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myEventCounter(*this, false, LABEL_AND_ID(EventCounter), 1)
, myCurrentCount(0)
, myAutoReset(*this, false, LABEL_AND_ID(AutoReset), false)
{
	myCriticalSection = new std::mutex();
}

ThreadEvent::~ThreadEvent()
{
	if (myOwnCriticalSection)
	{
		delete myCriticalSection;
		myCriticalSection = nullptr;
	}
}



void	ThreadEvent::wait()
{
	std::unique_lock<std::mutex> lk(*myCriticalSection);
	myConditionVariable.wait(lk, [this]
		{
			while (myCurrentCount < myEventCounter)
				return false;
			if (myAutoReset)
				myCurrentCount = 0;
			return true;
		});

}
void	ThreadEvent::signal()
{
	{
		std::lock_guard<std::mutex> lk(*myCriticalSection);
		myCurrentCount++;
	}
	myConditionVariable.notify_all();
}