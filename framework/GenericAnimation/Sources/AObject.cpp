// ****************************************************************************
// * NAME: AObject.cpp
// * GROUP: Animation module
// * --------------------------------------------------------------------------
// * PURPOSE: 
// * 
// * COMMENT: 
// * --------------------------------------------------------------------------
// ****************************************************************************

#include "GenericAnimationModule.h"

#include "AObject.h"
#include "ABaseSystem.h"
#include "ALinks.h"
#include "ABaseStream.h"
#include "AnimationResourceInfo.h"
#include "ABaseChannel.h"
#include "ALinearInterp.h"

#include "AObjectResource.h"
#include "Bones/AObjectSkeletonResource.h"

#include "TravState.h"
#include "Timer.h"

bool sortSystems::operator() (const SP<ABaseSystem>& lhs, const SP<ABaseSystem>& rhs) const
{
	if (lhs->getPriority() == rhs->getPriority())
		return lhs < rhs;
	
	return lhs->getPriority() < rhs->getPriority();
}

IMPLEMENT_CLASS_INFO(AObject)

//! constructor
IMPLEMENT_CONSTRUCTOR(AObject)
, mObject(nullptr)
{
	mSystemSet.clear();
}

/*!******************************
// * AttachSystem
// *-----------------------------
// * - search the corresponding system and create hierarchy from it
// * - 
// ******************************/

void    AObject::AttachSystem(SP<ABaseSystem> system)
{
	
	kstl::set<SP<ABaseSystem>>::iterator itbegin = mSystemSet.begin();
	kstl::set<SP<ABaseSystem>>::iterator itend = mSystemSet.end();
	
	while (itbegin != itend)
	{
		if(system->isSubType((*itbegin)->mClassID)) // same class ID ? probably not enough
		{
			system->SetHierarchyFromSystem((*itbegin));
			break;
		}
		++itbegin;
	}
};

void		AObject::addUser(CoreModifiable* user)
{
	mObject = user->SharedFromThis();
	CoreModifiable::addUser(user);
}
void		AObject::removeUser(CoreModifiable* user)
{
	if (mObject == user)
	{
		mObject = nullptr;
	}
	else
	{
		KIGS_WARNING("AObject : removeUser with bad user", 1);
	}
	CoreModifiable::removeUser(user);
}


// ******************************
// * Animate
// *-----------------------------
// * - call animate for all the systems
// * - 
// ******************************

void    AObject::Animate(ATimeValue t) 
{
	// +-----
	// | fading management
	// +-----
	
	IntU32   fade_count=mFadeList.size();
	if (fade_count)
	{
		kstl::vector<ALinearInterp*>::iterator it=mFadeList.begin();
		
		while(fade_count--)
		{
			ALinearInterp*  fade=*it;
			const KigsID& info1=fade->GetData();
			
			bool    finished;
			Float coef=fade->GetFade(t,finished);
			
			// this animation has reached a 0 weight
			if(finished == true)
			{
				if(coef <= KFLOAT_CONST(0.0f))
				{
					StopAnimation(info1);
				}
				it=mFadeList.erase(it);
				delete (fade);
			}
			
			else
			{
				++it;
			}
			
			SetAnimationWeight(info1,coef);
			
		}
	}
	
	for(auto s : mSystemSet)
	{
		s->Animate(t);
	}
};

void			AObject::InitModifiable()
{
	if (_isInit) // already init ? do nothing
	{
		return;
	}
	
	CoreModifiable::InitModifiable();
	
	if (_isInit)
	{
		// TODO ?
	}
}

// ******************************
// * AddAnimation
// *-----------------------------
// * -  Add the given animation to the object, if needed, create a system, channels and streams.   
// * - 
// ******************************

void    AObject::AddAnimation(const std::string& fname)
{
	
	// +---------
	// | create the animation and update all the elements :
	// | streams, systems ...
	// +---------
	
	if(mALinksTable.find(fname) != mALinksTable.end())
	{
		// this animation is already in the table
		return;
	}

	AnimationResourceInfo* info = gGenericAnimationModule->LoadAnimation(fname);
	if (!info)
		return;

	ALinks* tmp_links=new ALinks(info,info->GetStreamCount());
	
	mALinksTable[fname]=tmp_links;
	
	kstl::string ClassName;
	kstl::string StreamClassName;

	StreamClassName = info->getStreamType();
	
	if(StreamClassName == "")
	{
		auto it = mALinksTable.find(fname);
		mALinksTable.erase(it);
		delete tmp_links;
		return; 
	}
	
	kstl::string streamName=getName();
	streamName += "_AnimationStream";
	//streamName += "_" + kstl::to_string(GetAnimCount());
	
	SP<ABaseStream> stream=KigsCore::GetInstanceOf(streamName,StreamClassName);
	
	kstl::string system_type=stream->GetSystemType();
	
	// +---------
	// | search for a system handling this streams 
	// +---------
	bool need_channel_init=false;
	
	SP<ABaseSystem>    system = 0;
	for(auto s: mSystemSet)
	{
		if (s->isSubType(system_type))
		{
			system = s;
			break;
		}
	}
	
	if( !system )
	{
		// +---------
		// | create a new system to handle this animation
		// +---------
		
		kstl::string systemName=getName();
		systemName += "_AnimationSystem";
		
		system=KigsCore::GetInstanceOf(systemName, system_type);
		
		if(system != nullptr)
		{
			
			AddSystem( system );
			
			// +---------
			// | set the hierarchy
			// +---------
			
			if(system->UseOwnHierarchy() == true)
			{
				// search AObjectSkeletonResource in sons
				std::vector<CMSP>	instances;
				GetSonInstancesByType("AObjectSkeletonResource", instances);
				
				if (instances.size())
				{
					system->SetHierarchy(instances[0]);
				}
			}
			else
			{
				AttachSystem(system);
			}
			need_channel_init=true;
		}
		else
		{
			
			
			auto it = mALinksTable.find(fname);
			mALinksTable.erase(it);
			delete tmp_links;
			
			return; 
		}
	}
	
	tmp_links->SetSystem(system);
	
	// +---------
	// | search for the good channel and add the streams
	// +---------
	
	
	IntU32 g_id=info->GetStreamGroupID(0);
	
	SP<ABaseChannel> channel=system->GetChannelByUID(g_id);
	bool    one_stream_is_ok=false;
	
	if(channel)
	{
		channel->addItem(stream);
		tmp_links->AddStream(stream,0);
		stream->InitFromResource(info,0);
		one_stream_is_ok=true;
	}
	else
	{
		tmp_links->m_StreamCount--;
	}
	
	IntU32		i;
	IntU32		streamindex = one_stream_is_ok ? 1 : 0;
	for(i = 1; i < info->GetStreamCount(); ++i)
	{
		
		// +---------
		// | search for the good channel and add the streams
		// +---------
		
		streamName = getName();
		streamName += "_AnimationStream";
		//streamName += "_" + kstl::to_string(GetAnimCount());
		
		stream=KigsCore::GetInstanceOf(streamName, StreamClassName);
		
		g_id=info->GetStreamGroupID(i);
		
		channel=system->GetChannelByUID(g_id);
		
		if(channel != nullptr)
		{
			channel->addItem(stream);
			tmp_links->AddStream(stream,streamindex);
			++streamindex;
			stream->InitFromResource(info,i);
			one_stream_is_ok=true;
		}
		else
		{
			tmp_links->m_StreamCount--;
		}
	}
	if(one_stream_is_ok)
	{
		system->AddLinks();
		
		if(need_channel_init)
		{
			system->InitLocalToGlobalData();
		}
	}
	else
	{
		auto it = mALinksTable.find(fname);
		mALinksTable.erase(it);
		delete(tmp_links);
		if(system->GetValidStream() == nullptr)
		{
			RemoveSystem( system );
		}
	}
};

// ******************************
// * RemoveAnimation
// *-----------------------------
// * - delete an animation from this object, also delete the corresponding streams
// * - and if needed destroy the corresponding system
// ******************************

void    AObject::RemoveAnimation(const KigsID& info)
{
	// +---------
	// | first look for the slot in the list 
	// +---------
	
	if(mALinksTable.find(info) == mALinksTable.end())
	{
		return;
	}
	
	ALinks* tmp_links=mALinksTable[info];
	
	if(tmp_links == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	SP<ABaseSystem> sys=tmp_links->GetSystem();
	
	const std::vector<SP<ABaseStream>>& stream_array=tmp_links->GetStreamArray();
	
	IntU32    i;
	
	for(i=0;i<tmp_links->GetStreamCount();++i)
	{
		SP<ABaseChannel> channel=stream_array[i]->GetChannel();
		channel->removeItem(stream_array[i]);
	}
	
	
	sys->RemoveLinks();
	
	auto it = mALinksTable.find(info);
	mALinksTable.erase(it);
	delete (tmp_links);
	
	// +---------
	// | check is system should be deleted
	// +---------
	
	if(sys->GetLinksCount() == 0)
	{
		RemoveSystem(sys);
	}
};

// ******************************
// * Local To Global Data management
// *-----------------------------
// * - 
// * - 
// ******************************

// ----------------------------------------------------------------------------

// *******************
// * SetLocalToGlobalMode
// * -----------------
// * - Three differents modes :
// *    
// *    - FromAnimation : the local to global data is only given by the animation itself
// *      ( from the root channel ), the module ask for a starting local to global data
// * 
// *    - FromAnimationWithLoop : the local to global data is given by the animation, but
// *       the starting local to global data is updated when animation loop
// * 
// *    - FromEngine : the animation local to global data is not used, the data is asked to
// *        the engine.
// * 
// ******************************

void    AObject::SetLocalToGlobalMode(const KigsID& system_type, int mode)
{	
	for(auto s: mSystemSet)
	{
		if (s->isSubType(system_type))
		{
			if (mode == FromAnimation)
			{
				s->UseAnimationLocalToGlobalData(true);
				s->UpdateLocalToGlobalWhenLoop(false);
			}
			else if (mode == FromAnimationWithLoop)
			{
				s->UseAnimationLocalToGlobalData(true);
				s->UpdateLocalToGlobalWhenLoop(true);
			}
			else if (mode == FromEngine)
			{
				s->UseAnimationLocalToGlobalData(false);
			}
			
			break;
		}
	}
	
};

// ----------------------------------------------------------------------------




// *******************
// * LinkTo
// * -----------------
// * 
// * 
// *******************

void    AObject::LinkTo(unsigned int   system_type,SP<AObject> other_object,IntU32 g_id)
{
	SP<ABaseSystem>  sys1 = GetSystemByType(system_type);
	SP<ABaseSystem>  sys2 = other_object->GetSystemByType(system_type);
	
	if((sys1 != nullptr) && (sys2 != nullptr))
	{
		SP<ABaseChannel>   channel=sys2->GetChannelByUID(g_id);
		if(channel)
			sys1->LinkTo(channel);
	}
};

void    AObject::UnLink(unsigned int system_type)
{
	SP<ABaseSystem>  sys1= GetSystemByType(system_type);
	
	if(sys1)
	{
		sys1->UnLink();
	}
};

// ******************************
// * GetSystem
// *-----------------------------
// * - 
// * - 
// ******************************

// ----------------------------------------------------------------------------

// *******************
// * GetSystemByType
// * -----------------
// * if a system using this super classid is used by this channel, return this system
// * 
// *******************

SP<ABaseSystem>    AObject::GetSystemByType(unsigned int system_type)
{
	for(auto s: mSystemSet)
	{
		if (s->isSubType(system_type))
		{
			return s;
		}
	}
	
	return nullptr;
};




// ----------------------------------------------------------------------------

// *******************
// * GetSystembyIndex
// * -----------------
// * return the system used by this object corresponding to index.
// * 
// *******************

SP<ABaseSystem>    AObject::GetSystembyIndex(IntU32 index)
{
	IntU32   i=0;
	for(auto s: mSystemSet)
	{
		if (i == index)
		{
			return s;
		}
		++i;
	}
	
	return nullptr;
};

// ******************************
// * GetAnimationByIndex
// *-----------------------------
// * - return an AnimationResourceInfo corresponding to the given index
// * - 
// ******************************

AnimationResourceInfo*     AObject::GetAnimationByIndex(IntU32 index)
{
	IntU32   j=0;
	for(auto it = mALinksTable.begin(); it != mALinksTable.end();++it)
	{
		if(j == index)
		{
			ALinks* tmp_links=(ALinks*)(*it).second;
			
			return tmp_links->GetAnimResourceInfo();
		}
		++j;
	}
	return nullptr;
};

// ******************************
// * StartAnimation
// *-----------------------------
// * - start playing the given animation
// * - 
// ******************************

void    AObject::StartAnimation(const KigsID& info,ATimeValue  t,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &t, &AObject::StartAnimationFor);
	}
};


// ******************************
// * SetAnimationPos
// *-----------------------------
// * set the animation time for the given % of animation length
// * 
// ******************************

void    AObject::SetAnimationPos(const KigsID& info,Float  percent,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &percent, &AObject::SetAnimationPosFor);
	}
};

/*!******************************
// * StartRepeatAnimation
// *-----------------------------
// * - start playing animation n times
// * - 
// ******************************/

struct repeatAnimationParamStruct
{
	ATimeValue  t;
	int			repeatCount;
};

void    AObject::StartRepeatAnimation(const KigsID& info,ATimeValue  t,IntU32 n,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		repeatAnimationParamStruct params;
		params.repeatCount = n;
		params.t = t;
		DoForEachStream((*found).second, g_id, g_count, &params, &AObject::StartRepeatAnimationFor);
	}
};

// ******************************
// * StopAnimation
// *-----------------------------
// * - stop playing the given animation 
// * - 
// ******************************

void    AObject::StopAnimation(const KigsID& info,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, nullptr, &AObject::StopAnimationFor);
	}
};

// ******************************
// * StopAllAnimations
// *-----------------------------
// * - stop playing all the animations
// * - 
// ******************************

void    AObject::StopAllAnimations(IntU32* g_id,IntU32 g_count)
{
	
	for(auto it=mALinksTable.begin();it!=mALinksTable.end();++it)
	{
		ALinks* tmp_links=(ALinks*)(*it).second;
		DoForEachStream(tmp_links, g_id,g_count,nullptr,&AObject::StopAnimationFor);
	}
};

// ******************************
// * ResumeAnimation
// *-----------------------------
// * - restart the animation where it was stopped
// * - 
// ******************************

void    AObject::ResumeAnimation(const KigsID& info,ATimeValue  t,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &t, &AObject::ResumeAnimationFor);
	}
};

// ******************************
// * SetAnimationWeight
// *-----------------------------
// * - change the weight of an animation
// * - 
// ******************************

void    AObject::SetAnimationWeight(const KigsID& info,Float weight,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &weight, &AObject::SetAnimationWeightFor);
	}
};

// ******************************
// * SetAnimationSpeed
// *-----------------------------
// * - change the speed of an animation
// * - 
// ******************************

void    AObject::SetAnimationSpeed(const KigsID& info,Float speed,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &speed, &AObject::SetAnimationSpeedFor);
	}
};


// ******************************
// * MulAnimationWeight
// *-----------------------------
// * - multiply the weight of an animation
// * - 
// ******************************

void    AObject::MulAnimationWeight(const KigsID& info,Float weight,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &weight, &AObject::MulAnimationWeightFor);
	}
};

// ******************************
// * MulAnimationSpeed
// *-----------------------------
// * - multiply the speed of an animation
// * - 
// ******************************

void    AObject::MulAnimationSpeed(const KigsID& info,Float speed,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &speed, &AObject::MulAnimationSpeedFor);
	}
};

// ******************************
// * FadeAnimationTo
// *-----------------------------
// * - fade from one animation to another, both animations are supposed to be playing
// * - 
// ******************************

void    AObject::FadeAnimationTo(const KigsID& info1, const KigsID& info2,ATimeValue  fade_length,ATimeValue  t)
{
	
	if(mALinksTable.find(info1) == mALinksTable.end())
	{
		return;
	}
	ALinks* tmp_links=mALinksTable[info1];
	
	if(tmp_links == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	SP<ABaseStream> stream=(tmp_links->GetStreamArray())[0];
	Float w=stream->GetWeight();
	
	// animations can not be faded twice at the same time, so stop 
	// other fades 
	
	kstl::vector<ALinearInterp*>::iterator it=mFadeList.begin();
	
	while (it != mFadeList.end())
	{
		ALinearInterp*  tmpFade=*it;
		
		if ( (tmpFade->GetData() == info2) || (tmpFade->GetData() == info1))
		{
			it=mFadeList.erase(it);
			delete (tmpFade);
		}
		else
		{
			++it;
		}
	}   
	
	// add fade for first animation
	
	ALinearInterp* fade=new ALinearInterp(w,KFLOAT_CONST(0.0f),t,fade_length,info1);
	mFadeList.push_back(fade);
	
	SetAnimationWeight(info2,KFLOAT_CONST(0.0f));
	
	// add fade for second animation
	
	fade=new ALinearInterp(KFLOAT_CONST(0.0),KFLOAT_CONST(1.0f),t,fade_length,info2);
	mFadeList.push_back(fade);
};

// ******************************
// * SynchroniseAnimations
// *-----------------------------
// * - set speed of animation2 so that animation1 reach is local time synchro1 at the same time
// *  as animation2 reach is local time synchro2
// ******************************

void    AObject::SynchroniseAnimations(const KigsID& info1, const KigsID& info2,ATimeValue  synchro1,ATimeValue  synchro2)
{
	// +---------
	// | first look for the slot in the list 
	// +---------
	
	if(mALinksTable.find(info1) == mALinksTable.end())
	{
		return;
	}
	if(mALinksTable.find(info2) == mALinksTable.end())
	{
		return;
	}
	
	ALinks* links1=mALinksTable[info1];
	
	if(links1 == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	ALinks* links2=mALinksTable[info2];
	
	if(links2 == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	
	SP<ABaseStream> stream1=(links1->GetStreamArray())[0];
	SP<ABaseStream> stream2=(links2->GetStreamArray())[0];
	
	if(stream1->mSpeed == KFLOAT_CONST(0.0f))
	{
		SetAnimationSpeed(info1,KFLOAT_CONST(1.0f));
	}
	
	Float coef=(Float)synchro1+((Float)(stream1->mStartTime-stream2->mStartTime)*stream1->mSpeed);
	if(coef != KFLOAT_CONST(0.0f))
	{
		SetAnimationSpeed(info2,(Float)synchro2*stream1->mSpeed/coef);
	}
	
};

// ******************************
// * HasAnimationLoop
// *-----------------------------
// * - 
// * - 
// ******************************

bool    AObject::HasAnimationLoop(const KigsID& info1)
{
	
	if(mALinksTable.find(info1) == mALinksTable.end())
	{
		return false;
	}
	
	ALinks* links1=mALinksTable[info1];
	
	if(links1 == nullptr)
	{
		// this animation is not in the table
		return false;
	}
	
	SP<ABaseStream> stream1=(links1->GetStreamArray())[0];
	if(stream1)
	{
		return stream1->HasLoop();
	}
	
	return false;
};

// ******************************
// * AnimationIsSet
// *-----------------------------
// * - 
// * - 
// ******************************

bool    AObject::AnimationIsSet(const KigsID& info1)
{
	if(mALinksTable.find(info1) == mALinksTable.end())
	{
		return false;
	}
	
	return true;
}


// ******************************
// * SetLoop
// *-----------------------------
// * - 
// * - 
// ******************************

void    AObject::SetLoop(const KigsID& info,bool loop,IntU32* g_id,IntU32 g_count)
{
	const auto& found = mALinksTable.find(info);
	if (found != mALinksTable.end())
	{
		DoForEachStream((*found).second, g_id, g_count, &loop, &AObject::SetLoopFor);
	}
};

// ******************************
// * HasAnimationReachEnd
// *-----------------------------
// * - 
// * - 
// ******************************

bool    AObject::HasAnimationReachEnd(const KigsID& info1)
{
	
	if(mALinksTable.find(info1) == mALinksTable.end())
	{
		return false;
	}
	
	ALinks* links1=mALinksTable[info1];
	
	if(links1 == nullptr)
	{
		// this animation is not in the table
		return false;
	}
	
	SP<ABaseStream> stream1=(links1->GetStreamArray())[0];
	if(stream1)
	{
		return stream1->HasReachedEnd();
	}
	
	return false;
};


// ******************************
// * AddSystem
// *-----------------------------
// * - add a new system
// * - 
// ******************************

void    AObject::AddSystem(SP<ABaseSystem> system)
{
	system->SetAObject(SharedFromThis());
	mSystemSet.insert(system);
};

// ******************************
// * RemoveSystem
// *-----------------------------
// * - if the given system is in the list, remove it
// * -
// ******************************

void    AObject::RemoveSystem(SP<ABaseSystem> system)
{
	mSystemSet.erase(system);
};

// ******************************
// * DoForEachStream
// *-----------------------------
// * - call the given methods for all streams given in info
// * - 
// ******************************

void    AObject::DoForEachStream(ALinks* info,IntU32* g_id,IntU32 g_count,void* params,void (AObject::*callfunc)(SP<ABaseStream> stream,void* param))
{

	if(info == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	if(g_id != nullptr)
	{
		IntU32   id_count=g_count;
		IntU32*  id_list=0;
		
		if(g_count == 0)
		{
			// construct g_id list from the given g_id
			SP<ABaseChannel> channel= info->GetSystem()->GetChannelByUID(g_id[0]);
			if(channel)
			{
				id_list= info->GetSystem()->GetSonGroupIDList(channel,id_count);
			}
		}
		else
		{
			id_list=g_id;
		}
		
		IntU32    i;
		
		for(i=0;i< info->GetStreamCount();++i)
		{
			if(IsInGroupIDList((info->GetStreamArray())[i]->GetChannel()->GetGroupID(),id_list,id_count))
			{
				(this->*callfunc)((info->GetStreamArray())[i],params);
			}
		}
		if(g_count == 0)
		{
			delete[] id_list;
		}
		
	}
	else
	{
		IntU32    i;
		
		for(i=0;i< info->GetStreamCount();++i)
		{
			(this->*callfunc)((info->GetStreamArray())[i],params);
		}
	}
};


// ******************************
// * StartAnimation
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::StartAnimationFor(SP<ABaseStream> stream,void* param)
{
	stream->SetStartTime(*((ATimeValue*)param));
	stream->SetLocalTime(0);
	stream->Start();
};

// ******************************
// * SetAnimationPos
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::SetAnimationPosFor(SP<ABaseStream> stream,void* param)
{
	Float newlocalt = ((Float)stream->GetStreamLength()*(*(Float*)param));
	Float time = (Float)stream->GetStartTime()+	(Float)stream->GetLocalTime()/stream->GetSpeed();
	
	ATimeValue starttime=(ATimeValue)(time-newlocalt/stream->GetSpeed());
	
	stream->SetStartTime(starttime);
};

// ******************************
// * StartRepeatAnimation
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::StartRepeatAnimationFor(SP<ABaseStream> stream,void* param)
{
	repeatAnimationParamStruct& params = *(repeatAnimationParamStruct*)param;
	
	stream->SetStartTime(params.t);
	stream->SetLocalTime(0);
	stream->Start();
	stream->SetRepeatCount((IntU32)params.repeatCount);
	
};

// ******************************
// * StopAnimation
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::StopAnimationFor(SP<ABaseStream> stream,void* /* param */)
{
	stream->Stop();
};

// ******************************
// * ResumeAnimation
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::ResumeAnimationFor(SP<ABaseStream> stream,void* param)
{
	stream->SetStartTime((*((ATimeValue*)param))-stream->GetLocalTime());
	IntS32 tmp_count=stream->GetRepeatCount();
	stream->Start();
	stream->SetRepeatCount((IntU32)tmp_count);
	
};

// ******************************
// * SetAnimationWeight
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::SetAnimationWeightFor(SP<ABaseStream> stream,void* param)
{
	stream->SetWeight(*((Float*)param));
};

// ******************************
// * SetAnimationSpeed
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::SetAnimationSpeedFor(SP<ABaseStream> stream,void* param)
{
	stream->SetSpeed(*((Float*)param));
};

// ******************************
// * MulAnimationWeight
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::MulAnimationWeightFor(SP<ABaseStream> stream,void* param)
{
	Float w=stream->GetWeight();
	stream->SetWeight((*((Float*)param))*w);
};

// ******************************
// * MulAnimationSpeed
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::MulAnimationSpeedFor(SP<ABaseStream> stream,void* param)
{
	Float w=stream->GetSpeed();
	stream->SetSpeed((*((Float*)param))*w);
};

/*!******************************
// * SetLoop
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************/

void    AObject::SetLoopFor(SP<ABaseStream> stream,void* param)
{
	stream->SetLoop(*((bool*)param));
};


bool	AObject::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType(AObjectResource::mClassID))
	{
		// only one skeleton per AObject
		std::vector<CMSP>	instances;
		GetSonInstancesByType("AObjectSkeletonResource", instances);
		if(instances.size())
		{
			removeItem(instances[0]);
		}
	}
	return CoreModifiable::addItem(item, pos PASS_LINK_NAME(linkName));
}



void AObject::Update(const Timer& timer, void* addParam)
{
	for(auto s: mSystemSet)
	{
		s->SetupDraw();
	}
	Animate(timer.GetTime());
}
