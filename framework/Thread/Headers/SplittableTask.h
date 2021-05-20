#ifndef _SPLITTABLETASK_H_
#define _SPLITTABLETASK_H_

#include "Thread.h"
#include "Semaphore.h"
#include "ThreadEvent.h"
#include "maReference.h"

// ****************************************
// * SplitDataStructBase class
// * --------------------------------------
/**
 * \file	SplittableTask.h
 * \class	SplitDataStructBase
 * \ingroup Thread
 * \brief	Structure used to pass data to splittable task.
 */
 // ****************************************
class SplitDataStructBase
{
public:
	const Timer*	mCurrentTimer;
};


// ****************************************
// * SplittableTask class
// * --------------------------------------
/**
 * \file	SplittableTask.h
 * \class	SplittableTask
 * \ingroup Thread
 * \brief	Abstract class to manage splittable tasks.
 */
 // ****************************************
class SplittableTask : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(SplittableTask, CoreModifiable, Thread)
	SplittableTask(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~SplittableTask();

	void	Update(const Timer& timer, void* addParam) override;

protected:

	void	clear();

	virtual SplitDataStructBase*	createSplitDataStruct(int index, const Timer&)
	{
		// base class can't split
		return 0;
	};
	virtual SplitDataStructBase*	updateSplitDataStruct(SplitDataStructBase*, int index, const Timer&)
	{
		// base class can't split
		return 0;
	}

	// to be overloaded, default do nothing
	virtual	void	protectedSplitUpdate(SplitDataStructBase*){};

	DECLARE_METHOD(SplittedUpdate);
	COREMODIFIABLE_METHODS(SplittedUpdate);

	maReference		mThreadPoolManager;

	maBool			mIsSplittable;
	maUInt			mSplitCount;
	maBool			mWaitFinish;

	kstl::vector<SplitDataStructBase*>	mSplitDataStructList;
};

#endif //_SPLITTABLETASK_H_