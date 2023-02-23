#include "PrecompiledHeaders.h"
#include "SplittableTask.h"
#include "ThreadPoolManager.h"

using namespace Kigs::Thread;

IMPLEMENT_CLASS_INFO(SplittableTask)

SplittableTask::SplittableTask(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
	setInitParameter("IsSplittable",true);
	setInitParameter("SplitCount", true);
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


void	SplittableTask::Update(const Time::Timer& timer, void* addParam)
{
	CMSP	poolManager = getValue<CMSP>("ThreadPoolManager");
	if ((mIsSplittable) && (mSplitCount>1) && poolManager)
	{
		unsigned int i;

		ThreadPoolManager::TaskGroupHandle* grouphandle= poolManager->as<ThreadPoolManager>()->createTaskGroup();

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

		SmartPointer<ThreadEvent> testendwait = poolManager->as<ThreadPoolManager>()->LaunchTaskGroup(grouphandle);
		
		if (mWaitFinish && testendwait)
			testendwait->wait();
	}
}

DEFINE_METHOD(SplittableTask, SplittedUpdate)
{
	// call protected split update method
	protectedSplitUpdate((SplitDataStructBase*)privateParams);
	return false;
}
