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
        m_StreamArray= new ABaseStream*[streamcount];
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
        delete[] m_StreamArray;
    };

    IntU32  GetStreamCount()
    {
        return m_StreamCount;
    };

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

    void    AddStream(ABaseStream* stream,IntU32 index)
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

    void    SetSystem(ABaseSystem* system)
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

    ABaseStream**   GetStreamArray()
    {
        return m_StreamArray;
    }
    // ******************************
    // * GetSystem
    // *-----------------------------
    /*! return a pointer on the associated system
    */
    // ******************************

    ABaseSystem*    GetSystem()
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

    AnimationResourceInfo*		m_pAnimResourceInfo;
    ABaseSystem*                mSystem;
    ABaseStream**               m_StreamArray;
    IntU32						m_StreamCount;

};

#endif //__ALINKS_H__


