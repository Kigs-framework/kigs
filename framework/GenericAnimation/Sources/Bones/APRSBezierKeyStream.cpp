// **********************************************************************
// * FILE  : APRSBezierKeyStream.cpp
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS Keyframe animation for one bone
// * COMMENT : PC version
// *---------------------------------------------------------------------
// **********************************************************************
#include "PrecompiledHeaders.h"

#include "Bones/APRSBezierKeyStream.h"
#include "Bones/ABoneChannel.h"

IMPLEMENT_CLASS_INFO(APRSBezierKeyStream)

APRSBezierKeyStream::APRSBezierKeyStream(const kstl::string& name,CLASS_NAME_TREE_ARG) : APRSKeyStream(name,PASS_CLASS_NAME_TREE_ARG)
{

}

APRSBezierKeyStream::~APRSBezierKeyStream()
{

}



#define COS_EPSILON KFLOAT_CONST(0.000001)
#define HALFPI KFLOAT_CONST(1.570796326794895)

// 
//	Interpolate between p1 and p2 and use p0 and p3 to get tangent
//

void	APRSBezierKeyStream::BezierInterp(const AMPoint3* p0,const AMPoint3* p1,const AMPoint3* p2,const AMPoint3* p3,AMPoint3& result,const Float& t)
{
	AMPoint3 t1,t2;	// tangent
	kfloat	onmt=KFLOAT_CONST(1.0f)-t;	// 1-t

	// check if p0 exist
	if(p0)
	{
		// use vector p0p2 as tangent 
		t1=*p2;
		t1-=*p0;
		t1*=(KFLOAT_CONST(0.5f))*onmt*onmt*t;	// pre-multiply the vector by the right coef for the final cubic interpolation
	}
	else // if p0 is NULL, then use vector p1p2 as starting tangent
	{
		t1=*p2;
		t1-=*p1;
		t1*=onmt*onmt*t;
	}

	// check if p3 exist
	if(p3)
	{
		// use vector p3p1 as tangent 
		t2=*p1;
		t2-=*p3;
		t2*=KFLOAT_CONST(0.5f)*onmt*t*t;
	}
	else // if p3 is NULL, use p2p1 as tangent
	{
		t2=*p1;
		t2-=*p2;
		t2*=onmt*t*t;
	}

	// cubic interpolation

	result = *p1*(onmt*onmt*(onmt+(KFLOAT_CONST(3.0f))*t));
	result+= t1;
	result+= t2;
	result+= *p2*(t*t*(t+(KFLOAT_CONST(3.0f))*onmt));
};

// 
//	Interpolate between quaternions q1 and q2 and use q0 and q3 to get tangent
//  

void	APRSBezierKeyStream::BezierInterp(const AMQuat* q0,const AMQuat* q1,const AMQuat* q2,const AMQuat* q3,AMQuat& result,const Float& t)
{
	AMQuat t1,t2,a1,a2,a3;

	// the tangent calculation is similar to the one for points
	// but here we need to add the starting quaternion at the 'tangent' to
	// get a 'control quaternion'
	// so t1 is a 'control quaternion' for q1
	if(q0)
	{
		t1=*q2;
		t1-=*q0;
		t1*=KFLOAT_CONST(0.5f)*KFLOAT_CONST(1.0f)/KFLOAT_CONST(3.0f);
		t1+=*q1;
	}
	else
	{
		t1=*q2;
		t1-=*q1;
		t1*=KFLOAT_CONST(1.0f)/KFLOAT_CONST(3.0f);
		t1+=*q1;
	}

	// t2 is the 'control quaternion' for q2
	if(q3)
	{
		t2=*q3;
		t2-=*q1;
		t2*=KFLOAT_CONST(-0.5f)*KFLOAT_CONST(1.0f)/KFLOAT_CONST(3.0f);
		t2+=*q2;
	}
	else
	{
		t2=*q1;
		t2-=*q2;
		t2*=KFLOAT_CONST(1.0f)/KFLOAT_CONST(3.0f);
		t2+=*q2;
	}

	// construct the spline 'geometrically'

	a1=Slerp(*q1,t1,t);
	a2=Slerp(t1,t2,t);
	a3=Slerp(t2,*q2,t);
	a1=Slerp(a1,a2,t);

	a2=Slerp(a2,a3,t);
	result=Slerp(a1,a2,t);

};



// ******************************
// * UpdateData
// *-----------------------------
// * - UpdateData according to the the local time
// * - 
// ******************************

void    APRSBezierKeyStream::UpdateData(LocalToGlobalBaseType* standdata)
{
    ATimeValue				time=GetLocalTime();
    
    // Rotation keys
    // Search for the rotation key corresponding to time value
    
    PRSKey* standprs=(PRSKey*)standdata;//mChannel->GetStandData();
    IntU32  keycount=m_pPRSInfo->m_NbRotationKeys;
    
    if(keycount < 2)
    {
        if(keycount == 0)
        {
            // No rotation key : Use intial rotation of the bone
            m_CurrentPRSKey.m_RotationKey=standprs->m_RotationKey;
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
                AMQuat&			        p=rkeys[k1].m_Orientation;
                AMQuat&			        q=rkeys[k2].m_Orientation;

				AMQuat*					p0=NULL;
				AMQuat*					q1=NULL;

				if(k1)
				{
					p0=&(rkeys[k1-1].m_Orientation);
				}
				else if(mLoop) // in loop mode we suppose first key == last key so we jump one key 
				{
					p0=&(rkeys[keycount-2].m_Orientation);
				}
				if(k2<keycount-1)
				{
					q1=&(rkeys[k2+1].m_Orientation);
				}
				else if(mLoop) // in loop mode we suppose first key == last key so we jump one key 
				{
					q1=&(rkeys[1].m_Orientation);
				}
				
                ATimeValue				t1=rkeys[k1].m_Time;
                ATimeValue				t2=rkeys[k2].m_Time;				
				BezierInterp(p0, &p, &q, q1,m_CurrentPRSKey.m_RotationKey,(Float)(time - t1)/((Float)(t2 - t1)));
                
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
            // No scale key : Use intial scale of the bone
            m_CurrentPRSKey.m_ScaleKey=standprs->m_ScaleKey;
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
                AMPoint3&		        p=skeys[k1].m_ScaleValue;
                AMPoint3&		        q=skeys[k2].m_ScaleValue;
			
				AMPoint3*				p0=NULL;
				AMPoint3*				q1=NULL;

				if(k1)
				{
					p0=&(skeys[k1-1].m_ScaleValue);
				}
				else if(mLoop)
				{
					p0=&(skeys[keycount-2].m_ScaleValue);
				}

				if(k2<keycount-1)
				{
					q1=&(skeys[k2+1].m_ScaleValue);
				}
				else if(mLoop)
				{
					q1=&(skeys[1].m_ScaleValue);
				}
				
                ATimeValue				t1=skeys[k1].m_Time;
                ATimeValue				t2=skeys[k2].m_Time;				
				BezierInterp(p0, &p, &q, q1, m_CurrentPRSKey.m_ScaleKey,(Float)(time - t1)/((Float)(t2 - t1)));
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
    keycount=m_pPRSInfo->m_NbPositionKeys;
    if(keycount < 2)
    {
        if(keycount == 0)
        {
            // No position key : Use intial position of the bone
            m_CurrentPRSKey.m_PositionKey=standprs->m_PositionKey;
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
    
				AMPoint3*				p0=NULL;
				AMPoint3*				q1=NULL;

				if(k1)
				{
					p0=&(pkeys[k1-1].m_Position);
				}
				if(k2<keycount-1)
				{
					q1=&(pkeys[k2+1].m_Position);
				}

				BezierInterp(p0, &p, &q, q1, m_CurrentPRSKey.m_PositionKey,(Float)(time - t1)/((Float)(t2 - t1)));
            }
            else
            {
                m_CurrentPRSKey.m_PositionKey = pkeys[0].m_Position;
            }
        }
    }
};


