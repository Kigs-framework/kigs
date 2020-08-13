#include "PrecompiledHeaders.h"
#include "Thread.h"
#include "Core.h"
#include <ThreadProfiler.h>

IMPLEMENT_CLASS_INFO(Thread)

Thread::Thread(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
  myCurrentState= State::UNINITIALISED;
}     

Thread::~Thread()
{
#ifdef DO_THREAD_PROFILING
	SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
	if (threadProfiler)
		threadProfiler->RemoveThread(this);
#endif
	if(myCurrentThread.joinable())
		myCurrentThread.detach();
}    


template<typename... T>
void	Thread::Start(T&&... params)
{
	if ((myCurrentState == State::RUNNING) || (((CoreModifiable*)myCallee) == nullptr) || (myMethod.const_ref()==""))
	{
		return;
	}
	myProgress = 0;
	GetRef();
	PackCoreModifiableAttributes	attr(nullptr);
	int expander[]
	{
		(attr << std::forward<T>(params), 0)...
	};
	(void)expander;
	auto& attr_list = (kstl::vector<CoreModifiableAttribute*>&)attr;

	myCurrentThread = std::thread([this,&attr_list]()
		{
			myCurrentState = State::RUNNING;
#ifdef DO_THREAD_PROFILING
			ThreadProfiler::myCurrentThread = this;
			SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
			if (threadProfiler)
				threadProfiler->RegisterThread(this);
#endif

			((CoreModifiable*)myCallee)->CallMethod(myMethod.const_ref(), attr_list);

			Done();
		}
	);

}

void	Thread::Start()
{
	if ((myCurrentState == State::RUNNING) || (((CoreModifiable*)myCallee) == nullptr) || (myMethod.const_ref() == ""))
	{
		return;
	}

	myProgress = 0;
	GetRef();
	myCurrentThread = std::thread([this]()
		{
			myCurrentState = State::RUNNING;
#ifdef DO_THREAD_PROFILING
			ThreadProfiler::myCurrentThread = this;
			SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
			if (threadProfiler)
				threadProfiler->RegisterThread(this);
#endif
			
			kstl::vector<CoreModifiableAttribute*> attr_list;
			((CoreModifiable*)myCallee)->CallMethod(myMethod.const_ref(), attr_list);

			Done();
		}
	);

}

void Thread::InitModifiable()
{
	if (!IsInit())
	{
		if ((((CoreModifiable*)myCallee) == nullptr) || (myMethod.const_ref() == ""))
		{
			return;
		}
		else
		{
			ParentClassType::InitModifiable();
			Start();
		}
	}
}

// reset all states
void	Thread::Done()
{
	if (myFunctionWasInserted)
	{
		myCallee->RemoveMethod(myMethod.const_ref());
		myFunctionWasInserted = false;
	}

	myProgress = 1;
	myCurrentState = State::FINISHED;
	UnInit();
	flagAsPostDestroy();
	Destroy();
}
