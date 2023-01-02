#include "PrecompiledHeaders.h"

// ****************************************************************************
// * NAME: AStream.cpp
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE: 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// ****************************************************************************


#include "ABaseStream.h"
#include "ABaseChannel.h"

using namespace Kigs::Anim;
IMPLEMENT_CLASS_INFO(ABaseStream)
ABaseStream::ABaseStream(const std::string& name, CLASS_NAME_TREE_ARG)
	: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)

{}


void    ABaseStream::SetTime(ATimeValue t)
{

    if(mSpeed)
    {
        ATimeValue  end_time = GetStreamLength();
        ATimeValue  new_time = (ATimeValue)((Float)(t-mStartTime)*mSpeed);
		
		ATimeValue delta = (ATimeValue)((Float)end_time / mSpeed);
		mHasLoop = false;
		// Catch Up
		while(new_time < 0.0f)
        {
            if(mRepeatCount != 1)
            {
				if (mRepeatCount>1)
					--mRepeatCount;

				mHasLoop = true;
                new_time+=end_time;
                mStartTime-=delta;  
            }
			else
            {
                mEndReached=true;
                new_time = 0.0f;
            }
        }

        while(new_time > end_time)
        {
            if(mRepeatCount != 1)
            {
				if(mRepeatCount>1)
					--mRepeatCount;

				mHasLoop = true;
                new_time-=end_time;
                mStartTime+=delta;   
            }
			else
            {
                mEndReached=true;
                new_time = end_time;
            }
        }
 
        mLocalTime=new_time;
        if(mChannel)
        {
            UpdateData(mChannel->GetStandData());
        }
        else
        {
            UpdateData(0);
        }    
                
    }
    else
    {
        mStartTime=t;
    }
};


void    ABaseStream::SetTimeWithoutLoop(ATimeValue t)
{
    if(mSpeed)
    {
        ATimeValue  end_time=GetStreamLength();
		ATimeValue  new_time=(ATimeValue)((Float)(t-mStartTime)*mSpeed);
        mOutsideAnimFlag=false;

        if(new_time < 0.0f)
        {
            new_time = 0.0f;
            mOutsideAnimFlag=true;
        }
        if(new_time > end_time)
        {
            new_time = end_time;
            mOutsideAnimFlag=true;
        }

		if(mOutsideAnimFlag)
		{
			mLocalTime=new_time;
			UpdateData(mChannel->GetStandData());
		}
		else
		{
			SetTime(t);
		}
    }
    else
    {
        mStartTime=t;
    }
};


void    ABaseStream::SetTimeWithStartLoop(ATimeValue t)
{
    if(mSpeed)
    {
        ATimeValue  end_time=GetStreamLength();
		ATimeValue  new_time=(ATimeValue)((Float)(t-mStartTime)*mSpeed);
        mOutsideAnimFlag=false;

        if(new_time < 0.0f)
        {
            new_time = end_time;
			mOutsideAnimFlag=true;
        }
        if(new_time > end_time)
        {
            new_time = 0.0f;
			mOutsideAnimFlag=true;
        }
		
		if(mOutsideAnimFlag)
		{
			mOutsideAnimFlag=false;
			mLocalTime=new_time;
			UpdateData(mChannel->GetStandData());
		}
		else
		{
			SetTime(t);
		}
    }
    else
    {
        mStartTime=t;
    }
};


void ABaseStream::SetWeight(Float weight)
{
	if (mWeight != weight)
	{
		if (mChannel->IsRootChannel())
		{
			LocalToGlobalBaseType*   tmp_data = mChannel->GetLocalToGlobalBeforeChange();
			mWeight = weight;
			mChannel->ResetLocalToGlobalAfterChange(tmp_data);
		}
		mWeight = weight;
	}
};