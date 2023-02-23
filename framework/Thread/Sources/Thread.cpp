#include "PrecompiledHeaders.h"
#include "Thread.h"
#include "Core.h"
#include "ThreadProfiler.h"

using namespace Kigs::Thread;

IMPLEMENT_CLASS_INFO(Thread)

Thread::Thread(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{
	setInitParameter("Method",true);
	setInitParameter("Callee", true);
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

void	Thread::Kill()
{
	if (mCurrentState == State::RUNNING)
	{
		mKeepAlive = nullptr;
	}
}


template<typename... T>
void	Thread::Start(T&&... params)
{
	CMSP	currentCallee = getValue<CMSP>("Callee");
	if ((mCurrentState == State::RUNNING) || (currentCallee == nullptr) || (mMethod==""))
	{
		return;
	}
	mProgress = 0;

	PackCoreModifiableAttributes	attr(nullptr);
	int expander[]
	{
		(attr << std::forward<T>(params), 0)...
	};
	(void)expander;
	auto& attr_list = (std::vector<CoreModifiableAttribute*>&)attr;

	mCurrentThread = std::thread([this,&attr_list]()
		{
			mCurrentState = State::RUNNING;
#ifdef DO_THREAD_PROFILING
			{ // use block so ThreadProfiler smartpointer is not retained
				ThreadProfiler::mCurrentThread = this;
				SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
				if (threadProfiler)
					threadProfiler->RegisterThread(this);
			}
#endif

			((CoreModifiable*)mCallee)->CallMethod(mMethod, attr_list);

			Done();
		}
	);

}

void	Thread::Start()
{
	CMSP	currentCallee = getValue<CMSP>("Callee");
	if ((mCurrentState == State::RUNNING) || (currentCallee == nullptr) || (mMethod == ""))
	{
		return;
	}

	mProgress = 0;
	mKeepAlive = SharedFromThis();
	mCurrentThread = std::thread([this]()
		{
			mCurrentState = State::RUNNING;
#ifdef DO_THREAD_PROFILING
			{// use block so ThreadProfiler smartpointer is not retained
				ThreadProfiler::mCurrentThread = this;
				SP<ThreadProfiler> threadProfiler = KigsCore::GetSingleton("ThreadProfiler");
				if (threadProfiler)
					threadProfiler->RegisterThread(this);
			}
#endif
			
			std::vector<CoreModifiableAttribute*> attr_list;
			CMSP	currentCallee = getValue<CMSP>("Callee");
			currentCallee->CallMethod(mMethod, attr_list);

			Done();
		}
	);

}

void Thread::InitModifiable()
{
	if (!IsInit())
	{
		CMSP	currentCallee = getValue<CMSP>("Callee");

		if ((currentCallee == nullptr) || (mMethod == ""))
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
		mCallee.lock()->RemoveMethod(mMethod);
		mFunctionWasInserted = false;
	}

	mProgress = 1;
	mCurrentState = State::FINISHED;
	UnInit();
	flagAsPostDestroy();
	mKeepAlive.reset();
}
