#include "BumperDDSequence.h"
#include "Timer.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(BumperDDSequence)

BumperDDSequence::BumperDDSequence(const kstl::string& name, CLASS_NAME_TREE_ARG)
: BaseDDSequence(name, PASS_CLASS_NAME_TREE_ARG)
,myNextSequence(*this, true, LABEL_AND_ID(NextSequence))
,myDuration(*this, true, LABEL_AND_ID(Duration), 0.0f)
, mStartTime(0)
{
}

BumperDDSequence::~BumperDDSequence()
{
}


void BumperDDSequence::Update(const Timer&  timer, void* addParam)
{
	if (mStartTime == -1)
		return;

	kdouble time = timer.GetTime();
	if (mStartTime == 0)
		mStartTime = time;
	 
	if (time - mStartTime > myDuration)
	{
		KigsCore::GetNotificationCenter()->postNotificationName("StackSequence", this, (void*)&myNextSequence.const_ref());
		mStartTime = -1;
	}
}
