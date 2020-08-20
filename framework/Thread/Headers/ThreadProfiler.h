#ifndef ThreadProfiler_H
#define ThreadProfiler_H


#include <CoreModifiable.h>
#include "Thread.h"
#include "Timer.h"
#include "TimeProfiler.h"
#include "CoreBaseApplication.h"

#define THREAD_PROFILER_BUFFER_SIZE 4096
#define THREAD_PROFILER_STRING_SIZE 64

#define DO_THREAD_PROFILING


enum class TimeEventType : unsigned int
{
	Start,
	End,
	Mark,
	UnInit
};
struct TimeEvent
{
	TimeEvent()
	{
		type = TimeEventType::UnInit;
		time = DBL_MAX;
		name[0] = 0;
		function_name[0] = 0;
	}
	void Set(TimeEventType t, const char* evt_name, const char* func_name, kdouble ti)
	{
		type = t;
		time = ti;
		memcpy(name, evt_name, MAX(strlen(evt_name), THREAD_PROFILER_STRING_SIZE));
		name[THREAD_PROFILER_STRING_SIZE-1] = 0;
		memcpy(function_name, func_name, MAX(strlen(evt_name), THREAD_PROFILER_STRING_SIZE));
		function_name[THREAD_PROFILER_STRING_SIZE-1] = 0;
	}
	TimeEventType type;
	char name[THREAD_PROFILER_STRING_SIZE];
	char function_name[THREAD_PROFILER_STRING_SIZE];
	kdouble time;
};

struct TimeEventCircularBuffer
{
	TimeEventCircularBuffer()
	{
		for (int i = 0; i < THREAD_PROFILER_BUFFER_SIZE; ++i)
		{
			buffer[i].type = TimeEventType::UnInit;
		}
	}
	TimeEvent buffer[THREAD_PROFILER_BUFFER_SIZE];
};



// ****************************************
// * ThreadProfiler class
// * --------------------------------------
/**
 * \file	ThreadProfiler.h
 * \class	ThreadProfiler
 * \ingroup Thread
 * \brief	Helper class to profile threads.
 */
 // ****************************************

class ThreadProfiler : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(ThreadProfiler, CoreModifiable, Thread)

	//! constructor
	ThreadProfiler(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


	void AddTimeEvent(TimeEventType type, const char* name, const char* function_name)
	{
		if (!mAllowNewEvents) return;
		CoreModifiable* thread = mCurrentThread;
		int index = mCircularBufferIndexes[thread];
		mCircularBufferIndexes[thread] = (mCircularBufferIndexes[thread] + 1) % THREAD_PROFILER_BUFFER_SIZE;
		mCircularBufferMap[thread].buffer[index].Set(type, name, function_name, mGlobalTimer->GetTime());
	}

	void Stop()
	{
		mAllowNewEvents = false;
	}

	void Start()
	{
		mAllowNewEvents = true;
	}

	bool IsRunning()
	{
		return mAllowNewEvents;
	}

	void ClearProfiler();

	void RemoveThread(Thread* thread);

	void RegisterThread(Thread* thread);

	kstl::map<CoreModifiable*, TimeEventCircularBuffer>& GetThreadTimeEventMap(){ return mCircularBufferMap; }
	kstl::map<CoreModifiable*, unsigned int>& GetThreadIndexesMap(){ return mCircularBufferIndexes; }
	unsigned int GetThreadCircularBufferIndex(CoreModifiable* thread){ return (mCircularBufferIndexes[thread] + THREAD_PROFILER_BUFFER_SIZE - 1) % THREAD_PROFILER_BUFFER_SIZE; }

	SP<Timer>& GetThreadProfilerTimer(){ return mGlobalTimer; }

	void ExportProfile(const kstl::string path);

	//Remotery* rmt;

	static thread_local Thread* mCurrentThread;

private:
	
	kstl::map<CoreModifiable*, TimeEventCircularBuffer> mCircularBufferMap;
	kstl::map<CoreModifiable*, unsigned int> mCircularBufferIndexes;
	SP<Timer> mGlobalTimer;

	

	bool mAllowNewEvents;
	CMSP mSemaphore;
};


#endif // ThreadProfiler_H



#ifdef DO_THREAD_PROFILING

#if 1
/*#define BEGIN_TIMED_BLOCK(name) rmt_BeginCPUSample(name);
#define END_TIMED_BLOCK(name) rmt_EndCPUSample();
#define SCOPED_TIMED_BLOCK(name)  rmt_ScopedCPUSample(name);
#define ADD_TIMED_MARK(name)
#define STOP_THREAD_PROFILER ((ThreadProfiler*)KigsCore::GetThreadProfiler())->Stop();
#define RESUME_THREAD_PROFILER
#define NAME_THREAD(name) rmt_SetCurrentThreadName(name);*/
#else
#define BEGIN_TIMED_BLOCK(name) ((ThreadProfiler*)KigsCore::GetThreadProfiler())->AddTimeEvent(TimeEventType::Start, #name, __FUNCTION__);
#define END_TIMED_BLOCK(name) ((ThreadProfiler*)KigsCore::GetThreadProfiler())->AddTimeEvent(TimeEventType::End, #name, __FUNCTION__);
#define ADD_TIMED_MARK(name) ((ThreadProfiler*)KigsCore::GetThreadProfiler())->AddTimeEvent(TimeEventType::Mark, #name, __FUNCTION__);
#define STOP_THREAD_PROFILER ((ThreadProfiler*)KigsCore::GetThreadProfiler())->Stop();
#define RESUME_THREAD_PROFILER ((ThreadProfiler*)KigsCore::GetThreadProfiler())->Start();
#define SCOPED_TIMED_BLOCK(name)  
#define NAME_THREAD(name)
#endif


#else
/*#define BEGIN_TIMED_BLOCK(name) static Timer* timer_profiler_##name = KigsCore::Instance()->GetCoreApplication()->GetApplicationTimer(); kdouble time_##name = timer_profiler_##name->GetTime();
#define END_TIMED_BLOCK(name) printf("%s : %.3f ms\n", #name, (timer_profiler_##name->GetTime() - time_##name)*1000.0);
#define ADD_TIMED_MARK(name)*/
#define BEGIN_TIMED_BLOCK(name)
#define END_TIMED_BLOCK(name)
#define ADD_TIMED_MARK(name)
#define STOP_THREAD_PROFILER 
#define RESUME_THREAD_PROFILER
#define SCOPED_TIMED_BLOCK(name)  
#define NAME_THREAD(name)
#endif



/*
#define BEGIN_TIMED_BLOCK(name) printf("BEGIN_TIMED_BLOCK\n"); printf(#name); printf("\n");
#define END_TIMED_BLOCK(name) printf("END_TIMED_BLOCK\n"); printf(#name); printf("\n");
#define ADD_TIMED_MARK(name) printf("ADD_TIMED_MARK\n");
*/