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

IMPLEMENT_CLASS_INFO(ABaseStream)
ABaseStream::ABaseStream(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)

{}


void    ABaseStream::SetTime(ATimeValue t)
{

    if(m_Speed)
    {
        ATimeValue  end_time = GetStreamLength();
        ATimeValue  new_time = (ATimeValue)((Float)(t-m_StartTime)*m_Speed);
		
		ATimeValue delta = (ATimeValue)((Float)end_time / m_Speed);
		m_HasLoop = false;
		// Catch Up
		while(new_time < KFLOAT_ZERO)
        {
            if(m_RepeatCount != 1)
            {
				if (m_RepeatCount>1)
					--m_RepeatCount;

				m_HasLoop = true;
                new_time+=end_time;
                m_StartTime-=delta;  
            }
			else
            {
                m_bEndReached=true;
                new_time = KFLOAT_ZERO;
            }
        }

        while(new_time > end_time)
        {
            if(m_RepeatCount != 1)
            {
				if(m_RepeatCount>1)
					--m_RepeatCount;

				m_HasLoop = true;
                new_time-=end_time;
                m_StartTime+=delta;   
            }
			else
            {
                m_bEndReached=true;
                new_time = end_time;
            }
        }
 
        m_LocalTime=new_time;
        if(m_pChannel)
        {
            UpdateData(m_pChannel->GetStandData());
        }
        else
        {
            UpdateData(0);
        }    
                
    }
    else
    {
        m_StartTime=t;
    }
};


void    ABaseStream::SetTimeWithoutLoop(ATimeValue t)
{
    if(m_Speed)
    {
        ATimeValue  end_time=GetStreamLength();
		ATimeValue  new_time=(ATimeValue)((Float)(t-m_StartTime)*m_Speed);
        m_OutsideAnimFlag=false;

        if(new_time < KFLOAT_ZERO)
        {
            new_time = KFLOAT_ZERO;
            m_OutsideAnimFlag=true;
        }
        if(new_time > end_time)
        {
            new_time = end_time;
            m_OutsideAnimFlag=true;
        }

		if(m_OutsideAnimFlag)
		{
			m_LocalTime=new_time;
			UpdateData(m_pChannel->GetStandData());
		}
		else
		{
			SetTime(t);
		}
    }
    else
    {
        m_StartTime=t;
    }
};


void    ABaseStream::SetTimeWithStartLoop(ATimeValue t)
{
    if(m_Speed)
    {
        ATimeValue  end_time=GetStreamLength();
		ATimeValue  new_time=(ATimeValue)((Float)(t-m_StartTime)*m_Speed);
        m_OutsideAnimFlag=false;

        if(new_time < KFLOAT_ZERO)
        {
            new_time = end_time;
			m_OutsideAnimFlag=true;
        }
        if(new_time > end_time)
        {
            new_time = KFLOAT_ZERO;
			m_OutsideAnimFlag=true;
        }
		
		if(m_OutsideAnimFlag)
		{
			m_OutsideAnimFlag=false;
			m_LocalTime=new_time;
			UpdateData(m_pChannel->GetStandData());
		}
		else
		{
			SetTime(t);
		}
    }
    else
    {
        m_StartTime=t;
    }
};


void ABaseStream::SetWeight(Float weight)
{
	if (m_Weight != weight)
	{
		if (m_pChannel->IsRootChannel())
		{
			LocalToGlobalBaseType*   tmp_data = m_pChannel->GetLocalToGlobalBeforeChange();
			m_Weight = weight;
			m_pChannel->ResetLocalToGlobalAfterChange(tmp_data);
		}
		m_Weight = weight;
	}
};