// **********************************************************************
// * FILE  : APRSKeyStream.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS Keyframe animation for one bone
// * COMMENT : PC version
// *---------------------------------------------------------------------
// * DATES     : 24/05/2000
// * AUTHORS   : Stéphane Capo (scapo@fr.infogrames.com)
// * COPYRIGHT : (c) Infogrames/TecLab 2000
// **********************************************************************

#ifndef __APRSKEYSTREAM_H__
#define __APRSKEYSTREAM_H__

#include "AStream.h"
#include "ABonesDefines.h"
#include "AnimationResourceInfo.h"
#include "APRSStream.h"

//#include "AFastAlloc.h"

// ****************************************
// * KeyTime  class
// * --------------------------------------
/*!
    a time value
    \ingroup BoneAnimation
*/ 
// ****************************************

class   KeyTime
{
public :
    ATimeValue			m_Time;		// milliseconds
};

// ****************************************
// * PositionKey  class
// * --------------------------------------
/*!
    a time value and a position ( Point3 )
    \ingroup BoneAnimation
*/ 
// ****************************************

class PositionKey :public KeyTime
{
public:
    AMPoint3			m_Position;
};

// ****************************************
// * RotationKey  class
// * --------------------------------------
/*!
    a time value and a rotation ( Quaternion )
    \ingroup BoneAnimation
*/ 
// ****************************************

class RotationKey :public KeyTime
{
public:

    AMQuat  			m_Orientation;
};

// ****************************************
// * ScaleKey  class
// * --------------------------------------
/*!
    a time value and a scale ( Point3 )
    \ingroup BoneAnimation 
*/ 
// ****************************************

class ScaleKey :public KeyTime
{
public:

    AMPoint3			m_ScaleValue;
};

// the keys are just after this PRSInfo header in memory

// ****************************************
// * PRSInfo  class
// * --------------------------------------
/*!
    resource format of PRS keys
    \ingroup BoneAnimation
*/ 
// ****************************************

class PRSInfo
{
public:
    IntU32								m_NbPositionKeys;
    IntU32								m_NbRotationKeys;
    IntU32								m_NbScaleKeys;
    IntU32								m_Length;	//milliseconds
    

    // ****************************************
    // * GetPositionKeys
    // * --------------------------------------
    /*!
        return a pointer on the Position key array in the resource
    */ 
    // ****************************************

    PositionKey*	GetPositionKeys()
    {
        if(m_NbPositionKeys)
        {
            char* tmp=((char*)this)+sizeof(PRSInfo);
            PositionKey* result=(PositionKey*) tmp;
            return result;
        }
        return NULL;
    };

    // ****************************************
    // * GetRotationKeys
    // * --------------------------------------
    /*!
        return a pointer on the Rotation key array in the resource
    */ 
    // ****************************************

    RotationKey*    GetRotationKeys()
    {
        if(m_NbRotationKeys)
        {
            char* tmp=((char*)this)+sizeof(PRSInfo)+sizeof(PositionKey)*m_NbPositionKeys;
            RotationKey* result=(RotationKey*) tmp;
            return result;
        }
        return NULL;
    };

    // ****************************************
    // * GetScaleKeys
    // * --------------------------------------
    /*!
        return a pointer on the scale key array in the resource
    */ 
    // ****************************************

    ScaleKey*		GetScaleKeys()
    {
        if(m_NbScaleKeys)
        {
            char* tmp=((char*)this)+sizeof(PRSInfo)+sizeof(PositionKey)*m_NbPositionKeys+sizeof(RotationKey)*m_NbRotationKeys;
            ScaleKey* result=(ScaleKey*) tmp;
            return result;
        }
        return NULL;
    };
};

// ****************************************
// * APRSKeyStream  class
// * --------------------------------------
/*!
    The stream class used to store keyframe animation for bone animations
    \ingroup BoneAnimation
*/ 
// ****************************************

class APRSKeyStream : public APRSStream
{
public:
	DECLARE_CLASS_INFO(APRSKeyStream, APRSStream, Animation);
    DECLARE_CONSTRUCTOR(APRSKeyStream);
    
    // ******************************
    // * InitFromResource
    // *-----------------------------
    /*! init the stream with the given resource 
        set up the pointer on keyframe info ...
    */
    // ******************************
    
    void InitFromResource(AnimationResourceInfo* info,IntU32 streamindex) override;


    
protected:
    virtual ~APRSKeyStream();
    // ******************************
    // * UpdateData
    // *-----------------------------
    /*! Updated data according to the the local time
        interpolate between keys and set stand data if no keys
    */  
    // ******************************
    
    void    UpdateData(LocalToGlobalBaseType * standdata) override;
    
    // +---------
    // | protected members
    // +---------
    
    PRSInfo*                            m_pPRSInfo;
  
};


#endif // __APRSKEYSTREAM_H__


