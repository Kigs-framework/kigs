// **********************************************************************
// * FILE  : APRSControlStream.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS Keyframe animation for one bone
// * COMMENT : 
// *---------------------------------------------------------------------
// * DATES     : 24/05/2000
// **********************************************************************

#ifndef __APRSCONTROLSTREAM_H__
#define __APRSCONTROLSTREAM_H__

#include "astream.h"
#include "ABonesDefines.h"
#include "AnimationResourceInfo.h"
#include "APRSStream.h"


// ****************************************
// * PRSControllerKey  class
// * --------------------------------------
/*!
    Used by  APRSControlStream, handle a rotation, a translation and a scale
    \ingroup BoneAnimation
*/ 
// ****************************************

class PRSControllerKey
{
public:
	AMQuat  			m_RotationKey;
	AMPoint3			m_PositionKey;
	AMPoint3			m_ScaleKey;
    bool                m_HasPosition;
    bool                m_HasRotation;
    bool                m_HasScale;
};

// ****************************************
// * APRSControlStream  class
// * --------------------------------------
/*!
    stream with just one PRS value 
    \ingroup BoneAnimation
*/ 
// ****************************************

class APRSControlStream : public APRSStream
{
public:
	DECLARE_CLASS_INFO(APRSControlStream, APRSStream, Animation);
    DECLARE_CONSTRUCTOR(APRSControlStream);
    
    // ******************************
    // * LERPData
    // *-----------------------------
    /*! add the given PRS data to the stream PRSControllerKey
    */
    // ******************************

    void	LERPData(LocalToGlobalBaseType* data,Float t) override;

    
    // ******************************
    // * InitFromResource
    // *-----------------------------
    /*! init the stream with the given PRSControllerKey ( in the AnimationResourceInfo )
    */
    // ******************************
    
    void InitFromResource(AnimationResourceInfo* info,IntU32 streamindex) override;

    static AnimationResourceInfo* CreateAnimationResourceInfo(IntU32 group_id);
    static AnimationResourceInfo* CreateAnimationResourceInfo(IntU32 group_id_count,IntU32* group_id_list);
    virtual ~APRSControlStream();

protected:
    // ******************************
    // * UpdateData
    // *-----------------------------
    /*! update the current prs with the PRSControllerKey info
    */
    // ******************************
    
    void    UpdateData(LocalToGlobalBaseType * standdata) override;
    
    // +---------
    // | protected members
    // +---------
    
    PRSControllerKey*                       m_pPRSInfo;
};

#endif // __APRSKEYSTREAM_H__


