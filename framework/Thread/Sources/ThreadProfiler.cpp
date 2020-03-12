#include "PrecompiledHeaders.h"
#include <ThreadProfiler.h>
#include <FilePathManager.h>

thread_local Thread* ThreadProfiler::myCurrentThread=nullptr;


IMPLEMENT_CLASS_INFO(ThreadProfiler)

ThreadProfiler::ThreadProfiler(const kstl::string& name, CLASS_NAME_TREE_ARG): CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
	//rmt_CreateGlobalInstance(&rmt);
	mySemaphore = KigsCore::GetInstanceOf("threadprofilersepmaphore", "Semaphore");
	myGobalTimer = KigsCore::GetInstanceOf("ThreadProfilerTimer", "Timer");
#ifdef DO_THREAD_PROFILING
	myAllowNewEvents = true;
#else
	myAllowNewEvents = false;
#endif
}



void ThreadProfiler::ClearProfiler()
{
	for (kstl::map<CoreModifiable*, TimeEventCircularBuffer>::iterator it = myCircularBufferMap.begin(); it != myCircularBufferMap.end(); ++it)
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
	mySemaphore->GetMutex().lock();
	myCircularBufferMap.erase(thread);
	mySemaphore->GetMutex().unlock();
#endif 
}

void ThreadProfiler::RegisterThread(Thread* thread)
{
#ifdef DO_THREAD_PROFILING
	mySemaphore->GetMutex().lock();
	//myCircularBufferMap[thread];
	myCircularBufferIndexes[thread] = 0;
	mySemaphore->GetMutex().unlock();
#endif
}

void ThreadProfiler::ExportProfile(const kstl::string path)
{
	auto& pathManager = KigsCore::Singleton<FilePathManager>();
	kstl::string str = pathManager->DevicePath(path, FilePathManager::DOCUMENT_FOLDER);

	SmartPointer<FileHandle> file = Platform_fopen(str.c_str(), "wb");

	if (!file.isNil())
	{
		int num_threads = myCircularBufferMap.size();
		Platform_fwrite(&num_threads, sizeof(int), 1, file.get());
		
		for (kstl::map<CoreModifiable*, TimeEventCircularBuffer>::iterator it = myCircularBufferMap.begin(); it != myCircularBufferMap.end(); ++it)
		{
			Platform_fwrite(&it->second, sizeof(TimeEventCircularBuffer), 1, file.get());
			Platform_fwrite(&myCircularBufferIndexes[it->first], sizeof(int), 1, file.get());
		}
		Platform_fclose(file.get());
	}
}
