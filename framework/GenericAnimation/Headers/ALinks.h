// **********************************************************************
// * FILE  : ALinks.h
// * GROUP : Animation module
// *---------------------------------------------------------------------
// * PURPOSE :  Used to know what streams and what system are used by an 
// *            animation.
// * COMMENT : 
// *---------------------------------------------------------------------
// * DATES     : 25/05/2000
// **********************************************************************

#ifndef __ALINKS_H__
#define __ALINKS_H__

#include "AMDefines.h"
#include "ABaseSystem.h"
#include "ABaseStream.h"

// +---------
// | Declare some usefull classes
// +---------

class   AnimationResourceInfo;

// +---------
/*!  ALinks declaration
     \ingroup Animation
*/
// +--------

class   ALinks
{
public:

    friend class AObject;
 
    // ******************************
    // * Structors
    // *-----------------------------
    /*! Construtor from a AnimationResourceInfo
    */
    // ******************************

    ALinks(AnimationResourceInfo* info,IntU32 streamcount)
		: mSystem(0)
    {
        m_pAnimResourceInfo=info;
        m_StreamArray.resize(streamcount);
        m_StreamCount=streamcount;
    };

    // ******************************
    // * Structors
    // *-----------------------------
    /*! Destructor
    */
    // ******************************

    ~ALinks()
    {
        m_StreamArray.clear();
    }

    IntU32  GetStreamCount()
    {
        return m_StreamCount;
    }

protected:

    // ******************************
    // * Add / Remove functions 
    // *-----------------------------
    // * - 
    // * - 
    // ******************************

    // ******************************
    // * AddStream
    // *-----------------------------
    /*! Add a stream in the stream list
    */
    // ******************************

    void    AddStream(SP<ABaseStream> stream,IntU32 index)
    {
        m_StreamArray[index]=stream;
        stream->mResourceInfo=m_pAnimResourceInfo;
        
    };

    // ******************************
    // * SetSystem
    // *-----------------------------
    /*! Set the system associated to this ALinks
    */
    // ******************************

    void    SetSystem(SP<ABaseSystem> system)
    {
        mSystem=system;
    };

    // ******************************
    // * Get methods
    // *-----------------------------
    // * - 
    // * - 
    // ******************************

    // ******************************
    // * GetStreamArray
    // *-----------------------------
    /*! return the stream array 
    */
    // ******************************

    const std::vector<SP<ABaseStream>>&   GetStreamArray()
    {
        return m_StreamArray;
    }
    // ******************************
    // * GetSystem
    // *-----------------------------
    /*! return a pointer on the associated system
    */
    // ******************************

    SP<ABaseSystem>    GetSystem()
    {
        return mSystem;
    };

    // ******************************
    // * GetAnimResourceInfo
    // *-----------------------------
    /*! return a pointer on the AnimationResourceInfo
    */
    // ******************************

    AnimationResourceInfo* GetAnimResourceInfo() const
    {
        return m_pAnimResourceInfo;
    };

    AnimationResourceInfo*       m_pAnimResourceInfo;
    SP<ABaseSystem>                 mSystem;
    std::vector<SP<ABaseStream>>    m_StreamArray;
    IntU32						    m_StreamCount;

};

#endif //__ALINKS_H__


