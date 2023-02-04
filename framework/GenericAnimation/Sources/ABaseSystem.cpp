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

using namespace Kigs::Anim;

IMPLEMENT_CLASS_INFO(ABaseSystem)
ABaseSystem::ABaseSystem(const std::string& name, CLASS_NAME_TREE_ARG)
	: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
	, mPriority(*this, "Priority", 100)
{
	mLinksCount = 0;
	mRecurseAnimate = true;
	mOnlyLocalSkeletonUpdate = false;
	mChannelsCount = 0;

	mUseAnimationLocalToGlobal = false;
	mUpdateLocalToGlobalWhenLoop = false;
}

// ******************************
// * Hierarchy management
// *-----------------------------
// * - create channel tree, then call Init()
// * - 
// ******************************


void    ABaseSystem::SetHierarchy(SP<AObjectSkeletonResource> hierarchy)
{

    if(!hierarchy)
    {
        return;
    }

    // +---------
    // | construct channel tree
    // +---------

	std::string ClassName = GetChannelType();
	 
    if(ClassName != "")
    {
        IntU32 grp_count=hierarchy->GetGroupCount();

		if (grp_count == 0)
		{
			return;
		}

        mChannelsCount=grp_count;
        mChannelTab.resize(grp_count);

        IntU32 i;
        for(i=0;i<grp_count;++i)
        {
			char	charindex[10];
			sprintf(charindex,"%d",i);

			std::string channelName=getName();
			channelName+="_AnimationChannel_";
			channelName+=charindex;

            mChannelTab[i]=KigsCore::GetInstanceOf(channelName, ClassName);
            mChannelTab[i]->SetGroupID(hierarchy->getUID(i));
			mChannelTab[i]->SetBoneID(hierarchy->getID(i) - 1);

			mChannelTab[i]->SetStandData(hierarchy->getStandData(i));

            if(hierarchy->getFatherID(i) == 0)
            {
                mRoot=mChannelTab[i];
            }
            mChannelTab[i]->SetSystem(SharedFromThis());
        }

		ABaseChannel::AutoChannelTree(mChannelTab,hierarchy);

        SortChannels();
    }
 
    InitSystem();

};


void    ABaseSystem::SetHierarchyFromSystem(SP<ABaseSystem> sys)
{
    // +---------
    // | construct channel tree
    // +---------

	std::string ClassName= GetChannelType();
	 
    if(ClassName != "")
    {
        IntU32 grp_count=sys->mChannelsCount;
        mChannelsCount=grp_count;

        mChannelTab.resize(grp_count);

        IntU32 i;
        for(i=0;i<grp_count;++i)
        {
			ABaseChannel* other_channel=sys->mChannelTab[i].get();
			char	charindex[10];
			sprintf(charindex,"%d",i);

			std::string channelName=getName();
			channelName+="_AnimationChannel_";
			channelName+=charindex;

            mChannelTab[i]= KigsCore::GetInstanceOf(channelName, ClassName);
            mChannelTab[i]->SetGroupID(other_channel->GetGroupID());
			mChannelTab[i]->SetBoneID(other_channel->GetBoneID());

            if(other_channel->mFatherNode == NULL)
            {
                mRoot=mChannelTab[i];
            }
            mChannelTab[i]->SetSystem(SharedFromThis());
        }
    
        // create channel hierarchy

        ABaseChannel::AutoChannelTree(mChannelTab,sys);

    }
 
	InitSystem();
};

// ******************************
// * GetChannel
// *-----------------------------
// * - return the channel acting on the given group ID
// * - 
// ******************************

SP<ABaseChannel>   ABaseSystem::GetChannelByUID(IntU32 g_id)
{
	SP<ABaseChannel> found=nullptr;

    if(mChannelsCount < 2)
    {
        if(mChannelsCount)
        {
            if(mChannelTab[0]->GetGroupID() == g_id)
            {
                found=mChannelTab[0];
            }
        }
        return found;
    }

    IntU32   index1=0;
    IntU32   index2=mChannelsCount-1;
    
    while(1)
    {
        if(mChannelTab[index1]->GetGroupID() == g_id)
        {
            return     mChannelTab[index1];
        }
        if(mChannelTab[index2]->GetGroupID() == g_id)
        {
            return     mChannelTab[index2];
        }

        if((index2-index1) < 2)
        {
            return NULL;
        }
        
        IntU32   new_index=(index1+index2)/2;

        if(mChannelTab[new_index]->GetGroupID() > g_id)
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
    mRoot = nullptr;
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

    if(mChannelsCount<2)
    {
        return;
    }

    IntU32   i,j;
    IntU32   g_id1,g_id2,index;

    for(i=0;i<mChannelsCount-1;++i)
    {
        g_id1=mChannelTab[i]->GetGroupID();
        index=i;

        // find smaller index

        for(j=i+1;j<mChannelsCount;++j)
        {
            g_id2=mChannelTab[j]->GetGroupID();
            if(g_id1>g_id2)
            {
                index=j;
                g_id1=g_id2;
            }
        }

        if(index != i)
        {
            // swap 
            SP<ABaseChannel> tmp=mChannelTab[index];
            mChannelTab[index]=mChannelTab[i];
            mChannelTab[i]=tmp;
        }
    }
};


IntU32* ABaseSystem::GetSonGroupIDList(SP<ABaseChannel> first, IntU32& count)
{
	IntU32*  result = new IntU32[mChannelsCount];
	count = 0;

	first->GetSonGroupIDList(result, count);

	return result;
};


SP<ABaseStream>   ABaseSystem::GetValidStream()
{
	IntU32 i;

	for (i = 0; i<mChannelsCount; ++i)
	{
		if (mChannelTab[i]->mFirstStream != NULL)
		{
			return mChannelTab[i]->mFirstStream;
		}
	}

	return NULL;
};