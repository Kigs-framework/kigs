// **********************************************************************
// * FILE  : APRSBezierKeyStream.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE : PRS Keyframe animation for one bone with bezier interpolation
// * COMMENT : PC version
// *---------------------------------------------------------------------
// * DATES     : 24/05/2000
// **********************************************************************

#ifndef __APRSBEZIERKEYSTREAM_H__
#define __APRSBEZIERKEYSTREAM_H__

#include "astream.h"
#include "ABonesDefines.h"
#include "AnimationResourceInfo.h"
#include "APRSStream.h"
#include "APRSKeyStream.h"


// ****************************************
// * APRSBezierKeyStream  class
// * --------------------------------------
/*!
    The stream class used to store keyframe animation for bone animations
    \ingroup BoneAnimation
*/ 
// ****************************************

class APRSBezierKeyStream : public APRSKeyStream
{
public:
    	
	DECLARE_CLASS_INFO(APRSBezierKeyStream, APRSKeyStream, Animation);
    DECLARE_CONSTRUCTOR(APRSBezierKeyStream);
    virtual ~APRSBezierKeyStream();

protected:

  
    // ******************************
    // * UpdateData
    // *-----------------------------
    /*! Updated data according to the the local time
        interpolate between keys and set stand data if no keys
    */  
    // ******************************
    
    void UpdateData(LocalToGlobalBaseType* standdata) override;
    
	static void	BezierInterp(const AMPoint3* p0,const AMPoint3* p1,const AMPoint3* p2,const AMPoint3* p3, AMPoint3& result,const Float& t);
	static void	BezierInterp(const AMQuat* q0,const AMQuat* q1,const AMQuat* q2,const AMQuat* q3, AMQuat& result,const Float& t);
    
};

#endif // __APRSBEZIERKEYSTREAM_H__


