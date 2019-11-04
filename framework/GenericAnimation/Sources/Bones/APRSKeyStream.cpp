// **********************************************************************
// * FILE  : APRSKeyStream.cpp
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS Keyframe animation for one bone
// * COMMENT : PC version
// *---------------------------------------------------------------------
// **********************************************************************
#include "PrecompiledHeaders.h"

#include "Bones/APRSKeyStream.h"
#include "Bones/ABoneChannel.h"
#include "ModuleFileManager.h"

// ******************************
// * APRSKeyStream
// *-----------------------------
// * - fill data members
// * - 
// ******************************

#define COS_EPSILON KFLOAT_CONST(0.000001)
#define HALFPI KFLOAT_CONST(1.570796326794895)

IMPLEMENT_CLASS_INFO(APRSKeyStream)

APRSKeyStream::APRSKeyStream(const kstl::string& name,CLASS_NAME_TREE_ARG) : APRSStream(name,PASS_CLASS_NAME_TREE_ARG)
{
}

APRSKeyStream::~APRSKeyStream()
{
}

// ******************************
// * UpdateData
// *-----------------------------
// * - UpdateData according to the the local time
// * - 
// ******************************

void    APRSKeyStream::UpdateData(LocalToGlobalBaseType * standdata)
{
	ATimeValue				time=GetLocalTime();

	// Rotation keys
	// Search for the rotation key corresponding to time value

	PRSKey* standprs=(PRSKey*)standdata;//m_pChannel->GetStandData();
	IntU32  keycount=m_pPRSInfo->m_NbRotationKeys;

	if(keycount < 2)
	{
		if(keycount == 0)
		{
			// No rotation key : Use intial rotation of the bone
			if(standprs)
			{
				m_CurrentPRSKey.m_RotationKey=standprs->m_RotationKey;
			}
		}
		else
		{
			m_CurrentPRSKey.m_RotationKey = m_pPRSInfo->GetRotationKeys()[0].m_Orientation;
		}
	}
	else
	{
		RotationKey* rkeys=m_pPRSInfo->GetRotationKeys();

		IntU32  k1=0;
		IntU32  k2=keycount-1;

		if(time > rkeys[k2].m_Time)
		{
			m_CurrentPRSKey.m_RotationKey = rkeys[k2].m_Orientation;
		}
		else
		{

			while((k2-k1)>=2)
			{
				IntU32 newk=(k2+k1)/2;
				ATimeValue newt=rkeys[newk].m_Time;
				if(time > newt)
				{
					k1=newk;
				}
				else
				{
					k2=newk;
				}
			}

			if(k2)
			{
				ATimeValue				t1=rkeys[k1].m_Time;
				ATimeValue				t2=rkeys[k2].m_Time;

				Float wt=(Float)(time - t1)/((Float)(t2-t1));

				m_CurrentPRSKey.m_RotationKey = Slerp(rkeys[k1].m_Orientation,rkeys[k2].m_Orientation,wt);
			}
			else
			{
				m_CurrentPRSKey.m_RotationKey = rkeys[0].m_Orientation;

			}
		}
	}

#ifndef NO_SCALE
	// Scale keys
	// Search for the scale key corresponding to time value
	keycount=m_pPRSInfo->m_NbScaleKeys;
	if(keycount < 2)
	{
		if(keycount == 0)
		{
			if(standprs)
			{
				// No scale key : Use intial scale of the bone
				m_CurrentPRSKey.m_ScaleKey=standprs->m_ScaleKey;
			}
		}
		else
		{
			m_CurrentPRSKey.m_ScaleKey = m_pPRSInfo->GetScaleKeys()[0].m_ScaleValue;
		}
	}
	else
	{

		ScaleKey* skeys=m_pPRSInfo->GetScaleKeys();

		IntU32  k1=0;
		IntU32  k2=keycount-1;

		if(time > skeys[k2].m_Time)
		{
			m_CurrentPRSKey.m_ScaleKey = skeys[k2].m_ScaleValue;
		}
		else
		{
			while((k2-k1)>=2)
			{
				IntU32 newk=(k2+k1)/2;
				ATimeValue newt=skeys[newk].m_Time;
				if(time > newt)
				{
					k1=newk;
				}
				else
				{
					k2=newk;
				}
			}

			if(k2)
			{
				AMPoint3&  p=skeys[k1].m_ScaleValue;
				AMPoint3&  q=skeys[k2].m_ScaleValue;
				ATimeValue t1=skeys[k1].m_Time;
				ATimeValue t2=skeys[k2].m_Time;

				m_CurrentPRSKey.m_ScaleKey = Lerp(p, q, (Float)(time - t1)/((Float)(t2 - t1)));
			}
			else
			{
				m_CurrentPRSKey.m_ScaleKey = skeys[0].m_ScaleValue;
			}
		}

	}
#endif
	// Position keys
	// Search for the position key corresponding to time value

	if (standprs)
		m_CurrentPRSKey.m_PositionKey=standprs->m_PositionKey;

	keycount=m_pPRSInfo->m_NbPositionKeys;
	if(keycount < 2)
	{
		if(keycount == 0)
		{
			if(standprs)
			{
				// No position key : Use intial position of the bone
				m_CurrentPRSKey.m_PositionKey=standprs->m_PositionKey;
			}
		}
		else
		{
			m_CurrentPRSKey.m_PositionKey = m_pPRSInfo->GetPositionKeys()[0].m_Position;
		}
	}
	else
	{
		PositionKey* pkeys=m_pPRSInfo->GetPositionKeys();

		IntU32  k1=0;
		IntU32  k2=keycount-1;

		if(time > pkeys[k2].m_Time)
		{
			m_CurrentPRSKey.m_PositionKey = pkeys[k2].m_Position;
		}
		else
		{
			while((k2-k1)>=2)
			{
				IntU32 newk=(k2+k1)/2;
				ATimeValue newt=pkeys[newk].m_Time;
				if(time > newt)
				{
					k1=newk;
				}
				else
				{
					k2=newk;
				}
			}

			if(k2)
			{
				AMPoint3&				p=pkeys[k1].m_Position;
				AMPoint3&				q=pkeys[k2].m_Position;
				ATimeValue				t1=pkeys[k1].m_Time;
				ATimeValue				t2=pkeys[k2].m_Time;

				m_CurrentPRSKey.m_PositionKey = Lerp(p, q, (Float)(time - t1)/((Float)(t2 - t1)));
			}
			else
			{
				m_CurrentPRSKey.m_PositionKey = pkeys[0].m_Position;
			}
		}
	}

};

// ******************************
// * InitFromResource
// *-----------------------------
// * - init the stream with the given resource
// * - from AStream
// ******************************

void    APRSKeyStream::InitFromResource(AnimationResourceInfo* info,IntU32 streamIndex)
{
	AStreamResourceInfo* ressourceInfo=info->GetStreamResourceInfo(streamIndex);

	m_pPRSInfo=(PRSInfo *)( ((char*)ressourceInfo)+sizeof(AStreamResourceInfo) );
	m_Length = ((ATimeValue)m_pPRSInfo->m_Length)/KFLOAT_CONST(1000.0f);


};

