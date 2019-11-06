#include "PrecompiledHeaders.h"

// ****************************************************************************
// * NAME: AGroupIndexList.cpp
// * GROUP: animation module
// * --------------------------------------------------------------------------
// * PURPOSE: 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// ****************************************************************************

#include "AGroupIndexList.h"


// ******************************
// * AddGroup
// *-----------------------------
/* Add a group in the list, with the given id and interval count
*/ 
// ******************************


void    AGroupIndexList::AddGroup(IntU32 id,IntU32   intervalcount)
{
    AGroupInfo* found;
    if((found=GetGroupInfo(id)) != NULL)
    {
        found->m_UseCount++;
    }
    else
    {
        IntU8* tmp=new IntU8[sizeof(AGroupInfo)+sizeof(AIndexInterval)*intervalcount];

        AGroupInfo* tmp_group=(AGroupInfo*)tmp;

        tmp_group->m_GID=id;
        tmp_group->m_IntervalCount=intervalcount;
        tmp_group->m_UseCount=1;
        tmp_group->m_pNext=NULL;


        AddGroupInfo(tmp_group);
    }

};

// ******************************
// * RemoveGroup
// *-----------------------------
/* Remove the group with given group id
*/ 
// ******************************

void    AGroupIndexList::RemoveGroup(IntU32 id)
{
    AGroupInfo* read=m_pFirstGroup;
    AGroupInfo* prev=NULL;
    while(read != NULL)
    {
        if(read->m_GID == id)
        {
            read->m_UseCount--;
            if(read->m_UseCount == 0)
            {
                IntU8* tmp=(IntU8*)read;
                if(prev != NULL)
                {
                    prev->m_pNext=read->m_pNext;
                }
                else
                {
                    m_pFirstGroup=read->m_pNext;
                }
                delete[] tmp;
            }
        }

        prev=read;
        read=read->m_pNext;
    }
};

// ******************************
// * SetGroupInterval
// *-----------------------------
/* set the given interval for the given group
*/ 
// ******************************

void                AGroupIndexList::SetGroupInterval(IntU32 g_id,IntU32 index,IntU32 min,IntU32 max)
{
    AGroupInfo* found=GetGroupInfo(g_id);
    if(found != NULL)
    {
        AIndexInterval* interv=(AIndexInterval*)(((IntU8*)found)+sizeof(AGroupInfo));
        interv[index].SetInterval(min,max);
    }
};

// ******************************
// * GetGroupIntervals
// *-----------------------------
/* return a pointer on first interval
*/ 
// ******************************

AIndexInterval*     AGroupIndexList::GetGroupIntervals(IntU32 g_id)
{
    AGroupInfo* found=GetGroupInfo(g_id);
    if(found != NULL)
    {
        AIndexInterval* interv=(AIndexInterval*)(((IntU8*)found)+sizeof(AGroupInfo));
        return interv;
    }
    return NULL;
};


// ******************************
// * GetGroupIntervalCount
// *-----------------------------
/* return the interval count for the given group
*/ 
// ******************************

IntU32    AGroupIndexList::GetGroupIntervalCount(IntU32 g_id)
{
    AGroupInfo* found=GetGroupInfo(g_id);
    if(found != NULL)
    {
        return found->m_IntervalCount;
    }
    return 0;
};


// ******************************
// * GetGroupInfo
// *-----------------------------
/*  return a pointer on the group info with given id
*/ 
// ******************************

AGroupIndexList::AGroupInfo*  AGroupIndexList::GetGroupInfo(IntU32 g_id)
{
    AGroupInfo* read=m_pFirstGroup;
    while(read != NULL)
    {
        if(read->m_GID == g_id)
        {
            return read;
        }
        else if(read->m_GID > g_id)
        {
            return NULL;
        }
        read=read->m_pNext;
    }
    return NULL;
};

// ******************************
// * AddGroup
// *-----------------------------
/* add the given groupinfo in the list, the list is sorted by id
*/ 
// ******************************

void    AGroupIndexList::AddGroupInfo(AGroupInfo* to_add)
{
    if(m_pFirstGroup == NULL)
    {
        m_pFirstGroup=to_add;
    }
    else
    {
        AGroupInfo*    read=m_pFirstGroup;

        if(to_add->m_GID < read->m_GID)
        {
            to_add->m_pNext=m_pFirstGroup;
            m_pFirstGroup=to_add;
        }
        else
        {

            AGroupInfo*    next=read->m_pNext;
            while(1)
            {
                if(next == NULL)
                {
                    read->m_pNext=to_add;
                    break;
                }
                else
                {
                    if(to_add->m_GID < next->m_GID)
                    {
                        to_add->m_pNext=next;
                        read->m_pNext=to_add;
                        break;
                    }
                }

                read=next;
                next=read->m_pNext;
            }

        }
    }

};


