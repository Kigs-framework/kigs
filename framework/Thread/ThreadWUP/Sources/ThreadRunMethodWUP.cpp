#include "PrecompiledHeaders.h"
#include "ThreadRunMethodWUP.h"
#include "Core.h"
#include "Thread.h"
#include "CoreBaseApplication.h"

IMPLEMENT_CLASS_INFO(ThreadRunMethodWUP)

ThreadRunMethodWUP::ThreadRunMethodWUP(const kstl::string& name,CLASS_NAME_TREE_ARG) : ThreadRunMethod(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ThreadRunMethodWUP::~ThreadRunMethodWUP()
{  
}

void ThreadRunMethodWUP::EndThread()
{
}

void ThreadRunMethodWUP::StartThread()
{
}

void	ThreadRunMethodWUP::setAffinityMask(int mask)
{
	CoreBaseApplication::setThreadAffinityMask(myHandle,mask);
}

void ThreadRunMethodWUP::sleepThread()
{
}

void ThreadRunMethodWUP::wakeUpThread()
{
}

void ThreadRunMethodWUP::waitDeath(unsigned long P_Time_Out)
{
}

