#include "PrecompiledHeaders.h"
#include "CoreActionKeyFrame.h"

bool	CoreActionKeyFrameBase::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	bool done=false;
	while(!done)
	{
		if(time<(mTimeArray[mLastKeyIndex+1]+mStartTime))
		{
			done=true;
		}
		else
		{
			++mLastKeyIndex;
			protectedSetValue(mLastKeyIndex);
			if((mLastKeyIndex+1)>=(int)mKeyFrameCount)
			{
				return true;
			}
		}
	}
	return false;
}