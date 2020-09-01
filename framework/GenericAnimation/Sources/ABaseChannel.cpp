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


IMPLEMENT_CLASS_INFO(ABaseChannel)
ABaseChannel::ABaseChannel(const kstl::string& name, CLASS_NAME_TREE_ARG)
: CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)

{
	mFirstStream = NULL;
	mSystem = NULL;
	mFatherNode = 0;
	mSonCount = 0;
}


bool	ABaseChannel::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	//! if item is a stream, then add it to the streams list 
	if(item->isSubType(ABaseStream::mClassID))
	{
		ABaseStream* stream=(ABaseStream*)item.get();
		AddStream(stream);
	}
	
	return CoreModifiable::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool	ABaseChannel::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	//! if item is a stream, then try to remove it from streams list
	if(item->isSubType(ABaseStream::mClassID))
	{		
		ABaseStream* stream=(ABaseStream*)item.get();
		RemoveStream(stream);
	}
	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}


// ******************************
// * AddStream
// *-----------------------------
//!* - Search the stream list to insert this new stream
// * - 
// ******************************

void        ABaseChannel::AddStream(ABaseStream* stream)
{
	
    //+---------
    //! if the stream list is empty, just set this stream in first place
    //+---------
	
    if(mFirstStream == NULL)
    {
        mFirstStream=stream;
    }
    else
    {
		ABaseStream*    read=mFirstStream;
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
			
			ABaseStream*    next=read->GetNextStream();
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
	
    stream->SetChannel(this);
};

// ******************************
// * RemoveStream
// *-----------------------------
//!* - search the stream list and delete the given stream from the list
//!* - the stream is not really deleted, if the stream is not found, return 
// ******************************

void        ABaseChannel::RemoveStream(ABaseStream* stream)
{
    //+---------
    //! if the list is empty, return
    //+---------
	
    if(mFirstStream == NULL)
    {
        return;
    }
    else
    {
        //+---------
        //! else search for the stream to substract
        //+---------
		
		ABaseStream*    read=mFirstStream;
		ABaseStream*    last=NULL;
        while(read->GetPriority()>stream->GetPriority())
        {
			ABaseStream*    next=read->GetNextStream();
            if(next==NULL)
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
			
			ABaseStream*    next=read->GetNextStream();
            if(next==NULL)
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

void    ABaseChannel::AutoChannelTree(SP<ABaseChannel>* channels, AObjectSkeletonResource* hierarchy)
{
    IntU32 i,j;
    IntU32 grp_count=hierarchy->GetGroupCount();
	
	ABaseChannel**  tmpsonchannels=new ABaseChannel*[grp_count];
	
    for(i=0;i<grp_count;++i)
    {
        for(j=0;j<grp_count;++j)
        {
            if(hierarchy->getID(i) == hierarchy->getFatherID(j) )
            {
                channels[j]->mFatherNode=channels[i].get();
                tmpsonchannels[channels[i]->mSonCount++]=channels[j].get();
            }
        }
		
        if(channels[i]->mSonCount != 0)
        {
            for(j=0;j<(IntU32)channels[i]->mSonCount;++j)
            {
                CMSP toAdd(tmpsonchannels[j], GetRefTag{});
				channels[i]->addItem(toAdd);
				
            }
        }
    }
	
    delete[] tmpsonchannels;
	
};

void    ABaseChannel::AutoChannelTree(SP<ABaseChannel>* channels,ABaseSystem* sys)
{
    IntU32 i,j;
    IntU32 grp_count=sys->mChannelsCount;
	
	ABaseChannel**  tmpsonchannels=new ABaseChannel*[grp_count];
	
    for(i=0;i<grp_count;++i)
    {
		ABaseChannel* other_channel=sys->mChannelTab[i].get();	
        for(j=0;j<grp_count;++j)
        {
			ABaseChannel* father_channel=(ABaseChannel*)sys->mChannelTab[j]->mFatherNode;
			
            if(father_channel)
            {
                if(other_channel->GetGroupID() == father_channel->GetGroupID() )
                {
                    channels[j]->mFatherNode=channels[i].get();
                    tmpsonchannels[channels[i]->mSonCount++]=channels[j].get();
                }  
            }
        }
        
        if(channels[i]->mSonCount != 0)
        {
            for(j=0;j<(IntU32)channels[i]->mSonCount;++j)
            {
                CMSP toAdd(tmpsonchannels[j], GetRefTag{});
				channels[i]->addItem(toAdd);
            }
        }
    }
	
    delete[] tmpsonchannels;
	
};





