// ****************************************************************************
// * NAME: AGroupIndexList.h
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE: 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// * RELEASE: 
// ****************************************************************************

#ifndef __AGROUPINDEXLIST_H__
#define __AGROUPINDEXLIST_H__

#include "AMDefines.h"
#include "AIndexInterval.h"

// ----------------------------------------------------------------------------

// ****************************************
// * AGroupIndexList class
// * --------------------------------------
/*!  Class used to store indexes of values for each groups in an AEntitySet
     \ingroup Animation
*/ 
// ****************************************

class   AGroupIndexList
{
public:

    // ******************************
    // * Structors
    // *-----------------------------
    /*! Constructor 
    */ 
    // ******************************

    AGroupIndexList()
    { 
        m_pFirstGroup=NULL;
    };

    // ******************************
    // * Structors
    // *-----------------------------
    /*! Destructor
    */ 
    // ******************************

    ~AGroupIndexList()
    {
        while(m_pFirstGroup != NULL)
        {
            IntU8* tmp=(IntU8*)m_pFirstGroup;
            m_pFirstGroup=m_pFirstGroup->m_pNext;
            delete[] tmp;
        }
    };

    // ****************************************
    // * AGroupInfo class
    // * --------------------------------------
    /*! Class used by AGroupIndexList to store group id and corresponding intervals 
        \ingroup Animation
    */ 
    // ****************************************
    class   AGroupInfo
    {
    public:
        IntU32          m_GID;
        AGroupInfo*     m_pNext;
        IntU32          m_IntervalCount;
        IntU32          m_UseCount;
    };

    // ******************************
    // * AddGroup
    // *-----------------------------
    /*! Add a group in the list, with the given id and interval count
    */ 
    // ******************************

    void    AddGroup(IntU32 id,IntU32   intervalcount);

    // ******************************
    // * RemoveGroup
    // *-----------------------------
    /*! Remove the group with given group id
    */ 
    // ******************************

    void    RemoveGroup(IntU32 id);

    // ******************************
    // * SetGroupInterval
    // *-----------------------------
    /*! set the given interval for the given group
    */ 
    // ******************************

    void                SetGroupInterval(IntU32 g_id,IntU32 index,IntU32 min,IntU32 max);

    // ******************************
    // * GetGroupIntervals
    // *-----------------------------
    /*! return a pointer on first interval
    */ 
    // ******************************

    AIndexInterval*     GetGroupIntervals(IntU32 g_id);

    // ******************************
    // * GetGroupIntervalCount
    // *-----------------------------
    /*! return the interval count for the given group
    */ 
    // ******************************

    IntU32    GetGroupIntervalCount(IntU32 g_id);

   
protected:

    // ******************************
    // * GetGroupInfo
    // *-----------------------------
    /*! return a pointer on the group info with given id
    */ 
    // ******************************

    AGroupInfo*  GetGroupInfo(IntU32 g_id);

    // ******************************
    // * AddGroup
    // *-----------------------------
    /*! add the given groupinfo in the list, the list is sorted by id
    */ 
    // ******************************

    void    AddGroupInfo(AGroupInfo* to_add);

    AGroupInfo*         m_pFirstGroup;

};


#endif //__AGROUPINDEXLIST_H__


