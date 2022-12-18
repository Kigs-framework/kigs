#include "PrecompiledHeaders.h"
#include "ThreadIncludes.h"
#include "Timer.h"
#include "Core.h"
#include "WorkerThread.h"
#include "ThreadPoolManager.h"
#include "ThreadProfiler.h"


IMPLEMENT_CLASS_INFO(ModuleThread)

ModuleThread::ModuleThread(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ModuleThread::~ModuleThread()
{
}    


void ModuleThread::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"Thread",params);

	DECLARE_FULL_CLASS_INFO(core, Semaphore, Semaphore, Thread)
	DECLARE_FULL_CLASS_INFO(core, WorkerThread, WorkerThread, Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadPoolManager, ThreadPoolManager, Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadProfiler, ThreadProfiler, Thread)
	DECLARE_FULL_CLASS_INFO(core, Thread, Thread, Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadEvent, ThreadEvent, Thread)
#ifdef DO_THREAD_PROFILING
	KigsCore::SetThreadProfiler(KigsCore::GetSingleton("ThreadProfiler").get());
	KigsCore::GetThreadProfiler()->Init();
#endif

	KigsCore::SetMultiThread();
}

void ModuleThread::Close()
{
#ifdef DO_THREAD_PROFILING
	KigsCore::ReleaseSingleton("ThreadProfiler");
#endif
	KigsCore::CloseMultiThread();
	BaseClose();
}    

void ModuleThread::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    
