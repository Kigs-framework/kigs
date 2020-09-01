#include "PrecompiledHeaders.h"
#include "Thread.h"
#include "Core.h"
#include <ThreadProfiler.h>

IMPLEMENT_CLASS_INFO(Thread)

Thread::Thread(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
  mCurrentState= State::UNINITIALISED;
}     

Thread::~Thread()
{
#ifdef DO_THREAD_PROFILING
	SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
	if (threadProfiler)
		threadProfiler->RemoveThread(this);
#endif
	if(mCurrentThread.joinable())
		mCurrentThread.detach();
}    


template<typename... T>
void	Thread::Start(T&&... params)
{
	if ((mCurrentState == State::RUNNING) || (((CoreModifiable*)mCallee) == nullptr) || (mMethod.const_ref()==""))
	{
		return;
	}
	mProgress = 0;
	GetRef();
	PackCoreModifiableAttributes	attr(nullptr);
	int expander[]
	{
		(attr << std::forward<T>(params), 0)...
	};
	(void)expander;
	auto& attr_list = (kstl::vector<CoreModifiableAttribute*>&)attr;

	mCurrentThread = std::thread([this,&attr_list]()
		{
			mCurrentState = State::RUNNING;
#ifdef DO_THREAD_PROFILING
			ThreadProfiler::mCurrentThread = this;
			SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
			if (threadProfiler)
				threadProfiler->RegisterThread(this);
#endif

			((CoreModifiable*)mCallee)->CallMethod(mMethod.const_ref(), attr_list);

			Done();
		}
	);

}

void	Thread::Start()
{
	if ((mCurrentState == State::RUNNING) || (((CoreModifiable*)mCallee) == nullptr) || (mMethod.const_ref() == ""))
	{
		return;
	}

	mProgress = 0;
	GetRef();
	mCurrentThread = std::thread([this]()
		{
			mCurrentState = State::RUNNING;
#ifdef DO_THREAD_PROFILING
			ThreadProfiler::mCurrentThread = this;
			SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
			if (threadProfiler)
				threadProfiler->RegisterThread(this);
#endif
			
			kstl::vector<CoreModifiableAttribute*> attr_list;
			((CoreModifiable*)mCallee)->CallMethod(mMethod.const_ref(), attr_list);

			Done();
		}
	);

}

void Thread::InitModifiable()
{
	if (!IsInit())
	{
		if ((((CoreModifiable*)mCallee) == nullptr) || (mMethod.const_ref() == ""))
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
	if (mFunctionWasInserted)
	{
		mCallee->RemoveMethod(mMethod.const_ref());
		mFunctionWasInserted = false;
	}

	mProgress = 1;
	mCurrentState = State::FINISHED;
	UnInit();
	flagAsPostDestroy();
	Destroy();
}
