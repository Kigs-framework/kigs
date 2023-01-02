#include "PrecompiledHeaders.h"
#include <ThreadProfiler.h>
#include <FilePathManager.h>

using namespace Kigs::Thread;
using namespace Kigs::File;

thread_local Thread* ThreadProfiler::mCurrentThread=nullptr;


IMPLEMENT_CLASS_INFO(ThreadProfiler)

ThreadProfiler::ThreadProfiler(const std::string& name, CLASS_NAME_TREE_ARG): CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
	//rmt_CreateGlobalInstance(&rmt);
	mGlobalTimer = KigsCore::GetInstanceOf("ThreadProfilerTimer", "Timer");
#ifdef DO_THREAD_PROFILING
	mAllowNewEvents = true;
#else
	mAllowNewEvents = false;
#endif
}



void ThreadProfiler::ClearProfiler()
{
	for (std::map<CoreModifiable*, TimeEventCircularBuffer>::iterator it = mCircularBufferMap.begin(); it != mCircularBufferMap.end(); ++it)
	{
		for (int i = 0; i < THREAD_PROFILER_BUFFER_SIZE; ++i)
		{
			it->second.buffer[i].type = TimeEventType::UnInit;
		}
	}
}

void ThreadProfiler::RemoveThread(Thread* thread)
{
#ifdef DO_THREAD_PROFILING
	mMutex.lock();
	mCircularBufferMap.erase(thread);
	mMutex.unlock();
#endif 
}

void ThreadProfiler::RegisterThread(Thread* thread)
{
#ifdef DO_THREAD_PROFILING
	mMutex.lock();
	//myCircularBufferMap[thread];
	mCircularBufferIndexes[thread] = 0;
	mMutex.unlock();
#endif
}

void ThreadProfiler::ExportProfile(const std::string path)
{
	auto pathManager = KigsCore::Singleton<FilePathManager>();
	std::string str = pathManager->DevicePath(path, FilePathManager::DOCUMENT_FOLDER);

	SmartPointer<FileHandle> file = Platform_fopen(str.c_str(), "wb");

	if (file)
	{
		int num_threads = mCircularBufferMap.size();
		Platform_fwrite(&num_threads, sizeof(int), 1, file.get());
		
		for (std::map<CoreModifiable*, TimeEventCircularBuffer>::iterator it = mCircularBufferMap.begin(); it != mCircularBufferMap.end(); ++it)
		{
			Platform_fwrite(&it->second, sizeof(TimeEventCircularBuffer), 1, file.get());
			Platform_fwrite(&mCircularBufferIndexes[it->first], sizeof(int), 1, file.get());
		}
		Platform_fclose(file.get());
	}
}
