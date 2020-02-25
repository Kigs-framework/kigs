#include "PrecompiledHeaders.h"
#include "SplittableTask.h"
#include "ThreadPoolManager.h"

IMPLEMENT_CLASS_INFO(SplittableTask)

SplittableTask::SplittableTask(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myThreadPoolManager(*this, true, LABEL_AND_ID(ThreadPoolManager))
, myIsSplittable(*this, true, LABEL_AND_ID(isSplittable),false)
, mySplitCount(*this, true, LABEL_AND_ID(SplitCount), 0)
, myWaitFinish(*this, false, LABEL_AND_ID(WaitFinish), true)
{
	mySplitDataStructList.clear();
}

SplittableTask::~SplittableTask()
{
	clear();
}

void	SplittableTask::clear()
{
	unsigned int i;
	
	for (i = 0; i < mySplitDataStructList.size(); i++)
	{
		delete mySplitDataStructList[i];
	}
	mySplitDataStructList.clear();

}


void	SplittableTask::Update(const Timer& timer, void* addParam)
{
	if ((myIsSplittable) && (mySplitCount>1) && ((CoreModifiable*)myThreadPoolManager))
	{
		unsigned int i;

		ThreadPoolManager::TaskGroupHandle* grouphandle=((ThreadPoolManager*)((CoreModifiable*)myThreadPoolManager))->createTaskGroup();

		for (i = 0; i < mySplitCount; i++)
		{
			if (mySplitDataStructList.size() <= i)
			{
				mySplitDataStructList.push_back(createSplitDataStruct(i, timer));
			}
			mySplitDataStructList[i] = updateSplitDataStruct(mySplitDataStructList[i],i, timer);

			MethodCallingStruct* toadd = new MethodCallingStruct();
			toadd->myMethodID = KigsID("SplittedUpdate")._id;
			toadd->myMethodInstance = this;
			toadd->myPrivateParams = mySplitDataStructList[i];

			grouphandle->addTask(toadd);
		}

		SmartPointer<ThreadEvent> testendwait = ((ThreadPoolManager*)((CoreModifiable*)myThreadPoolManager))->LaunchTaskGroup(grouphandle);
		
		if (myWaitFinish && !testendwait.isNil())
			testendwait->wait();
	}
}

DEFINE_METHOD(SplittableTask, SplittedUpdate)
{
	// call protected split update method
	protectedSplitUpdate((SplitDataStructBase*)privateParams);
	return false;
}
