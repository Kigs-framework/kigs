#include "PrecompiledHeaders.h"
#include "SplittableTask.h"
#include "ThreadPoolManager.h"

IMPLEMENT_CLASS_INFO(SplittableTask)

SplittableTask::SplittableTask(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mThreadPoolManager(*this, true, LABEL_AND_ID(ThreadPoolManager))
, mIsSplittable(*this, true, LABEL_AND_ID(IsSplittable),false)
, mSplitCount(*this, true, LABEL_AND_ID(SplitCount), 0)
, mWaitFinish(*this, false, LABEL_AND_ID(WaitFinish), true)
{
	mSplitDataStructList.clear();
}

SplittableTask::~SplittableTask()
{
	clear();
}

void	SplittableTask::clear()
{
	unsigned int i;
	
	for (i = 0; i < mSplitDataStructList.size(); i++)
	{
		delete mSplitDataStructList[i];
	}
	mSplitDataStructList.clear();

}


void	SplittableTask::Update(const Timer& timer, void* addParam)
{
	if ((mIsSplittable) && (mSplitCount>1) && ((CoreModifiable*)mThreadPoolManager))
	{
		unsigned int i;

		ThreadPoolManager::TaskGroupHandle* grouphandle=((ThreadPoolManager*)((CoreModifiable*)mThreadPoolManager))->createTaskGroup();

		for (i = 0; i < mSplitCount; i++)
		{
			if (mSplitDataStructList.size() <= i)
			{
				mSplitDataStructList.push_back(createSplitDataStruct(i, timer));
			}
			mSplitDataStructList[i] = updateSplitDataStruct(mSplitDataStructList[i],i, timer);

			MethodCallingStruct* toadd = new MethodCallingStruct();
			toadd->mMethodID = KigsID("SplittedUpdate")._id;
			toadd->mMethodInstance = this;
			toadd->mPrivateParams = mSplitDataStructList[i];

			grouphandle->addTask(toadd);
		}

		SmartPointer<ThreadEvent> testendwait = ((ThreadPoolManager*)((CoreModifiable*)mThreadPoolManager))->LaunchTaskGroup(grouphandle);
		
		if (mWaitFinish && !testendwait.isNil())
			testendwait->wait();
	}
}

DEFINE_METHOD(SplittableTask, SplittedUpdate)
{
	// call protected split update method
	protectedSplitUpdate((SplitDataStructBase*)privateParams);
	return false;
}
