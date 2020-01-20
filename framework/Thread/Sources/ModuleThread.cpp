#include "PrecompiledHeaders.h"
#include "ThreadIncludes.h"
#include "Timer.h"
#include "Core.h"
#include "WorkerThread.h"
#include "ThreadPoolManager.h"
#include "ThreadProfiler.h"
#ifdef _KIGS_ONLY_STATIC_LIB_
#include "Platform/Thread/Threads.h"
#endif

IMPLEMENT_CLASS_INFO(ModuleThread)

ModuleThread::ModuleThread(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
}

ModuleThread::~ModuleThread()
{
}    


void ModuleThread::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"Thread",params);
	#ifdef _KIGS_ONLY_STATIC_LIB_
	RegisterDynamic(PlatformThreadModuleInit(core,params));
	#endif

	DECLARE_FULL_CLASS_INFO(core, WorkerThread, WorkerThread, Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadPoolManager, ThreadPoolManager, Thread)
	DECLARE_FULL_CLASS_INFO(core, ThreadProfiler, ThreadProfiler, Thread)

#ifdef DO_THREAD_PROFILING
	KigsCore::GetSingleton("ThreadLocalStorageManager")->Init();
	KigsCore::SetThreadProfiler(KigsCore::GetSingleton("ThreadProfiler").get());
	KigsCore::GetThreadProfiler()->Init();
#endif

	KigsCore::SetMultiThread();
}

void ModuleThread::Close()
{
#ifdef DO_THREAD_PROFILING
	KigsCore::ReleaseSingleton("ThreadProfiler");
	KigsCore::ReleaseSingleton("ThreadLocalStorageManager");
#endif
	KigsCore::CloseMultiThread();
	BaseClose();
}    

void ModuleThread::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
}    
