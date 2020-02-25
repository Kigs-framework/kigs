#ifndef _SPLITTABLETASK_H_
#define _SPLITTABLETASK_H_

#include "Thread.h"
#include "Semaphore.h"
#include "ThreadEvent.h"
#include "maReference.h"

// ****************************************
// * SplittableTask class
// * --------------------------------------
/*!  \class SplittableTask
		base class for task that can be split into several subtask and called in seperated thread	
\ingroup Thread
*/
// ****************************************

// base class used to pass data to splittable task
class SplitDataStructBase
{
public:
	const Timer*	currentTimer;
};

class SplittableTask : public CoreModifiable
{
public:

	DECLARE_ABSTRACT_CLASS_INFO(SplittableTask, CoreModifiable, Thread)

	//! constructor
	SplittableTask(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

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

	//! destructor
	virtual ~SplittableTask();

	DECLARE_METHOD(SplittedUpdate);
	COREMODIFIABLE_METHODS(SplittedUpdate);

	maReference		myThreadPoolManager;

	maBool			myIsSplittable;
	maUInt			mySplitCount;
	maBool			myWaitFinish;

	kstl::vector<SplitDataStructBase*>	mySplitDataStructList;
};

#endif //_SPLITTABLETASK_H_