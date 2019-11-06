#include "PrecompiledHeaders.h"

// ****************************************************************************
// * NAME: ASystem.cpp
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE: 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// ****************************************************************************

#include "ABaseSystem.h"
#include "ABaseChannel.h"
#include "Bones/AObjectSkeletonResource.h"
#include "ABaseStream.h"


IMPLEMENT_CLASS_INFO(ABaseSystem)
ABaseSystem::ABaseSystem(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
	, m_Priority(*this, true, LABEL_AND_ID(Priority), 100)
{
	m_LinksCount = 0;
	m_pChannelTab = NULL;
	m_RecurseAnimate = true;
	m_OnlyLocalSkeletonUpdate = false;
	m_ChannelsCount = 0;

	m_UseAnimationLocalToGlobal = false;
	m_UpdateLocalToGlobalWhenLoop = false;
}

// ******************************
// * Hierarchy management
// *-----------------------------
// * - create channel tree, then call Init()
// * - 
// ******************************


void    ABaseSystem::SetHierarchy(AObjectSkeletonResource* hierarchy)
{

    if(hierarchy == NULL)
    {
        return;
    }

    // +---------
    // | construct channel tree
    // +---------

	kstl::string ClassName = GetChannelType();
	 
    if(ClassName != "")
    {
        IntU32 grp_count=hierarchy->GetGroupCount();

		if (grp_count == 0)
		{
			return;
		}

        m_ChannelsCount=grp_count;
        m_pChannelTab=new ABaseChannel*[grp_count];

        IntU32 i;
        for(i=0;i<grp_count;++i)
        {
			char	charindex[10];
			sprintf(charindex,"%d",i);

			kstl::string channelName=getName();
			channelName+="_AnimationChannel_";
			channelName+=charindex;

            m_pChannelTab[i]=(ABaseChannel*) KigsCore::GetInstanceOf(channelName,_S_2_ID(ClassName));
            m_pChannelTab[i]->SetGroupID(hierarchy->getUID(i));
			m_pChannelTab[i]->SetBoneID(hierarchy->getID(i) - 1);

			m_pChannelTab[i]->SetStandData(hierarchy->getStandData(i));

            if(hierarchy->getFatherID(i) == 0)
            {
                mp_Root=m_pChannelTab[i];
            }
            m_pChannelTab[i]->SetSystem(this);
        }

		ABaseChannel::AutoChannelTree(m_pChannelTab,hierarchy);

        SortChannels();
    }
 
    InitSystem();

};


void    ABaseSystem::SetHierarchyFromSystem(ABaseSystem* sys)
{
    // +---------
    // | construct channel tree
    // +---------

	kstl::string ClassName= GetChannelType();
	 
    if(ClassName != "")
    {
        IntU32 grp_count=sys->m_ChannelsCount;
        m_ChannelsCount=grp_count;

        m_pChannelTab=new ABaseChannel*[grp_count];

        IntU32 i;
        for(i=0;i<grp_count;++i)
        {
			ABaseChannel* other_channel=sys->m_pChannelTab[i];
			char	charindex[10];
			sprintf(charindex,"%d",i);

			kstl::string channelName=getName();
			channelName+="_AnimationChannel_";
			channelName+=charindex;

            m_pChannelTab[i]=(ABaseChannel*) KigsCore::GetInstanceOf(channelName,_S_2_ID(ClassName));
            m_pChannelTab[i]->SetGroupID(other_channel->GetGroupID());
			m_pChannelTab[i]->SetBoneID(other_channel->GetBoneID());

            if(other_channel->mp_FatherNode == NULL)
            {
                mp_Root=m_pChannelTab[i];
            }
            m_pChannelTab[i]->SetSystem(this);
        }
    
        // create channel hierarchy

        ABaseChannel::AutoChannelTree(m_pChannelTab,sys);

    }
 
	InitSystem();
};

// ******************************
// * GetChannel
// *-----------------------------
// * - return the channel acting on the given group ID
// * - 
// ******************************

ABaseChannel*   ABaseSystem::GetChannelByUID(IntU32 g_id)
{
	ABaseChannel* found=NULL;

    if(m_ChannelsCount < 2)
    {
        if(m_ChannelsCount)
        {
            if(m_pChannelTab[0]->GetGroupID() == g_id)
            {
                found=m_pChannelTab[0];
            }
        }
        return found;
    }

    IntU32   index1=0;
    IntU32   index2=m_ChannelsCount-1;
    
    while(1)
    {
        if(m_pChannelTab[index1]->GetGroupID() == g_id)
        {
            return     m_pChannelTab[index1];
        }
        if(m_pChannelTab[index2]->GetGroupID() == g_id)
        {
            return     m_pChannelTab[index2];
        }

        if((index2-index1) < 2)
        {
            return NULL;
        }
        
        IntU32   new_index=(index1+index2)/2;

        if(m_pChannelTab[new_index]->GetGroupID() > g_id)
        {
            index2=new_index;

        }
        else
        {
            index1=new_index;
        }
    }

    return NULL;
};

// ******************************
// * DeleteChannelTree
// *-----------------------------
// * - delete all the channels
// * - 
// ******************************

void ABaseSystem::DeleteChannelTree()
{
	GetRootChannel()->Destroy();
};

// ******************************
// * SortChannels
// *-----------------------------
// * - sort channels by group id
// * - 
// ******************************

void    ABaseSystem::SortChannels()
{
    // +-----
    // | sort the channels from the one with smaller group ID to the
    // | one with greater group ID
    // +-----

    if(m_ChannelsCount<2)
    {
        return;
    }

    IntU32   i,j;
    IntU32   g_id1,g_id2,index;

    for(i=0;i<m_ChannelsCount-1;++i)
    {
        g_id1=m_pChannelTab[i]->GetGroupID();
        index=i;

        // find smaller index

        for(j=i+1;j<m_ChannelsCount;++j)
        {
            g_id2=m_pChannelTab[j]->GetGroupID();
            if(g_id1>g_id2)
            {
                index=j;
                g_id1=g_id2;
            }
        }

        if(index != i)
        {
            // swap 
            ABaseChannel* tmp=m_pChannelTab[index];
            m_pChannelTab[index]=m_pChannelTab[i];
            m_pChannelTab[i]=tmp;
        }
    }
};


IntU32* ABaseSystem::GetSonGroupIDList(ABaseChannel* first, IntU32& count)
{
	IntU32*  result = new IntU32[m_ChannelsCount];
	count = 0;

	first->GetSonGroupIDList(result, count);

	return result;
};


ABaseStream*   ABaseSystem::GetValidStream()
{
	IntU32 i;

	for (i = 0; i<m_ChannelsCount; ++i)
	{
		if (m_pChannelTab[i]->m_pFirstStream != NULL)
		{
			return m_pChannelTab[i]->m_pFirstStream;
		}
	}

	return NULL;
};