#include "PrecompiledHeaders.h"

// ****************************************************************************
// * NAME: AChannel.cpp
// * GROUP: Animation Module
// * --------------------------------------------------------------------------
// * PURPOSE: 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// ****************************************************************************

#include "ABaseChannel.h"
#include "ABaseStream.h"

#include "Bones/AObjectSkeletonResource.h"
#include "ABaseSystem.h"

using namespace Kigs::Anim;

IMPLEMENT_CLASS_INFO(ABaseChannel)
ABaseChannel::ABaseChannel(const std::string& name, CLASS_NAME_TREE_ARG)
: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)

{
	mFirstStream = nullptr;
	mSystem = nullptr;
	mFatherNode = 0;
	mSonCount = 0;
}


bool	ABaseChannel::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	//! if item is a stream, then add it to the streams list 
	if(item->isSubType(ABaseStream::mClassID))
	{
		AddStream(item);
	}
	
	return CoreModifiable::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool	ABaseChannel::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	//! if item is a stream, then try to remove it from streams list
	if(item->isSubType(ABaseStream::mClassID))
	{		
		RemoveStream(item);
	}
	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}


// ******************************
// * AddStream
// *-----------------------------
//!* - Search the stream list to insert this new stream
// * - 
// ******************************

void        ABaseChannel::AddStream(SP<ABaseStream> stream)
{
	
    //+---------
    //! if the stream list is empty, just set this stream in first place
    //+---------
	
    if(!mFirstStream)
    {
        mFirstStream=stream;
    }
    else
    {
		SP<ABaseStream>    read=mFirstStream;
        if(stream->GetPriority()>=read->GetPriority())
        {
            //+---------
            //! if this stream has the highest priority
            //! insert in first place
            //+---------
			
            stream->ForceNextStream(mFirstStream);
            mFirstStream=stream;
        }
        else
        {
            //+---------
            //! else search where to insert this stream
            //+---------
			
			SP<ABaseStream>    next=read->GetNextStream();
            while(1)
            {
                if(next == NULL)
                {
                    read->SetNextStream(stream);
                    break;
                }
                else
                {
                    if(stream->GetPriority()>=next->GetPriority())
                    {
                        read->SetNextStream(stream);
                        break;
                    }
                }
				
                read=next;
                next=read->GetNextStream();
            }
        }
    }
	
    stream->SetChannel(SharedFromThis());
};

// ******************************
// * RemoveStream
// *-----------------------------
//!* - search the stream list and delete the given stream from the list
//!* - the stream is not really deleted, if the stream is not found, return 
// ******************************

void        ABaseChannel::RemoveStream(SP<ABaseStream> stream)
{
    //+---------
    //! if the list is empty, return
    //+---------
	
    if(!mFirstStream)
    {
        return;
    }
    else
    {
        //+---------
        //! else search for the stream to substract
        //+---------
		
		SP<ABaseStream>    read=mFirstStream;
		SP<ABaseStream>    last=nullptr;
        while(read->GetPriority()>stream->GetPriority())
        {
			SP<ABaseStream>    next=read->GetNextStream();
            if(!next)
            {
                return;
            }
            last=read;
            read=next;
        }
		
        while(read->GetPriority() == stream->GetPriority())
        {
            if(read == stream)
            {
                if(last == NULL)
                {
                    // the first stream is deleted
                    mFirstStream=read->GetNextStream();
                    return;
                }
				
                last->DelNextStream();
                return;
            }
			
			SP<ABaseStream>    next=read->GetNextStream();
            if(!next)
            {
                return;
            }
            last=read;
            read=next;
        }
    }
};



// ----------------------------------------------------------------------------

// *******************
// * AutoChannelTree
// * -----------------
// * create the channel tree with the given channel array, and the given hierarchy
// * 
// *******************

void    ABaseChannel::AutoChannelTree(const std::vector<SP<ABaseChannel>>& channels, SP<AObjectSkeletonResource> hierarchy)
{
    IntU32 i,j;
    IntU32 grp_count=hierarchy->GetGroupCount();
	
    std::vector<SP<ABaseChannel>>  tmpsonchannels;
    tmpsonchannels.resize(grp_count);
	
    for(i=0;i<grp_count;++i)
    {
        for(j=0;j<grp_count;++j)
        {
            if(hierarchy->getID(i) == hierarchy->getFatherID(j) )
            {
                channels[j]->mFatherNode=channels[i];
                tmpsonchannels[channels[i]->mSonCount++]=channels[j];
            }
        }
		
        if(channels[i]->mSonCount != 0)
        {
            for(j=0;j<(IntU32)channels[i]->mSonCount;++j)
            {
				channels[i]->addItem(tmpsonchannels[j]);
            }
        }
    }
	
	
};

void    ABaseChannel::AutoChannelTree(const std::vector<SP<ABaseChannel>>& channels,SP<ABaseSystem> sys)
{
    IntU32 i,j;
    IntU32 grp_count=sys->mChannelsCount;
	
    std::vector<SP<ABaseChannel>>  tmpsonchannels;
    tmpsonchannels.resize(grp_count);

    for(i=0;i<grp_count;++i)
    {
		SP<ABaseChannel> other_channel=sys->mChannelTab[i];	
        for(j=0;j<grp_count;++j)
        {
			SP<ABaseChannel> father_channel=sys->mChannelTab[j]->mFatherNode;
			
            if(father_channel)
            {
                if(other_channel->GetGroupID() == father_channel->GetGroupID() )
                {
                    channels[j]->mFatherNode=channels[i];
                    tmpsonchannels[channels[i]->mSonCount++]=channels[j];
                }  
            }
        }
        
        if(channels[i]->mSonCount != 0)
        {
            for(j=0;j<(IntU32)channels[i]->mSonCount;++j)
            {
				channels[i]->addItem(tmpsonchannels[j]);
            }
        }
    }

	
};





