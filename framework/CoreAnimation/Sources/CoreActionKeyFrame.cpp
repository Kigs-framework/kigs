#include "PrecompiledHeaders.h"
#include "CoreActionKeyFrame.h"

bool	CoreActionKeyFrameBase::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool done=false;
	while(!done)
	{
		if(time<(myTimeArray[myLastKeyIndex+1]+myStartTime))
		{
			done=true;
		}
		else
		{
			++myLastKeyIndex;
			protectedSetValue(myLastKeyIndex);
			if((myLastKeyIndex+1)>=(int)myKeyFrameCount)
			{
				return true;
			}
		}
	}
	return false;
}