#include "PrecompiledHeaders.h"
#include "ThreadEvent.h"

IMPLEMENT_CLASS_INFO(ThreadEvent)

ThreadEvent::ThreadEvent(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mySemaphore(0)
, myEventCounter(*this, false, LABEL_AND_ID(EventCounter), 1)
, myCurrentCount(0)
, myAutoReset(*this, false, LABEL_AND_ID(AutoReset), false)
{
}

ThreadEvent::~ThreadEvent()
{

}



void	ThreadEvent::wait()
{
	platformWait();
}
void	ThreadEvent::signal()
{
	platformSignal();
}