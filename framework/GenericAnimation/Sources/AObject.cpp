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

bool sortSystems::operator() (const ABaseSystem* lhs, const ABaseSystem* rhs) const
{
	if (lhs->getPriority() == rhs->getPriority())
		return lhs < rhs;
	
	return lhs->getPriority() < rhs->getPriority();
}

IMPLEMENT_CLASS_INFO(AObject)

//! constructor
IMPLEMENT_CONSTRUCTOR(AObject)
, m_pObject(nullptr)
{
	m_ObjectResourceMap.clear();
	m_pSystemSet.clear();
}

/*!******************************
// * AttachSystem
// *-----------------------------
// * - search the corresponding system and create hierarchy from it
// * - 
// ******************************/

void    AObject::AttachSystem(ABaseSystem* system)
{
	
	kstl::set<ABaseSystem*>::iterator itbegin = m_pSystemSet.begin();
	kstl::set<ABaseSystem*>::iterator itend = m_pSystemSet.end();
	
	while (itbegin != itend)
	{
		if(system->isSubType((*itbegin)->myClassID)) // same class ID ? probably not enough
		{
			system->SetHierarchyFromSystem((*itbegin));
			break;
		}
		++itbegin;
	}
};

void		AObject::addUser(CoreModifiable* user)
{
	if (m_pObject == 0)
	{
		m_pObject = user;
	}
	else
	{
		KIGS_WARNING("AObject : addUser already done", 1);
	}
	Drawable::addUser(user);
}
void		AObject::removeUser(CoreModifiable* user)
{
	if (m_pObject == user)
	{
		m_pObject = 0;
	}
	else
	{
		KIGS_WARNING("AObject : removeUser with bad user", 1);
	}
	Drawable::removeUser(user);
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
	
	IntU32   fade_count=m_FadeList.size();
	if (fade_count)
	{
		kstl::vector<ALinearInterp*>::iterator it=m_FadeList.begin();
		
		while(fade_count--)
		{
			ALinearInterp*  fade=*it;
			AnimationResourceInfo* info1=fade->GetData();
			
			bool    finished;
			Float coef=fade->GetFade(t,finished);
			
			// this animation has reached a 0 weight
			if(finished == true)
			{
				if(coef <= KFLOAT_CONST(0.0f))
				{
					StopAnimation(info1);
				}
				it=m_FadeList.erase(it);
				delete (fade);
			}
			
			else
			{
				++it;
			}
			
			SetAnimationWeight(info1,coef);
			
		}
	}
	kstl::set<ABaseSystem*>::iterator itbegin = m_pSystemSet.begin();
	kstl::set<ABaseSystem*>::iterator itend = m_pSystemSet.end();
	
	while(itbegin != itend)
	{
		(*itbegin)->Animate(t);
		++itbegin;
	}
};

void			AObject::InitModifiable()
{
	if (_isInit) // already init ? do nothing
	{
		return;
	}
	
	Drawable::InitModifiable();
	
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

void    AObject::AddAnimation(AnimationResourceInfo* info)
{
	
	// +---------
	// | create the animation and update all the elements :
	// | streams, systems ...
	// +---------
	
	if(m_ALinksTable.find(info) != m_ALinksTable.end())
	{
		// this animation is already in the table
		return;
	}
	
	
	ALinks* tmp_links=new ALinks(info,info->GetStreamCount());
	
	m_ALinksTable[info]=tmp_links;
	
	kstl::string ClassName;
	kstl::string StreamClassName;

	StreamClassName = info->getStreamType();
	
	if(StreamClassName == "")
	{
		auto it = m_ALinksTable.find(info);
		m_ALinksTable.erase(it);
		delete tmp_links;
		return; 
	}
	
	kstl::string streamName=getName();
	streamName += "_AnimationStream";
	//streamName += "_" + kstl::to_string(GetAnimCount());
	
	ABaseStream* stream=(ABaseStream*)KigsCore::GetInstanceOf(streamName,StreamClassName);
	
	kstl::string system_type=stream->GetSystemType();
	
	// +---------
	// | search for a system handling this streams 
	// +---------
	bool need_channel_init=false;
	kstl::set<ABaseSystem*>::iterator itbegin = m_pSystemSet.begin();
	kstl::set<ABaseSystem*>::iterator itend = m_pSystemSet.end();
	ABaseSystem*    system = 0;
	while (itbegin != itend)
	{
		if ((*itbegin)->isSubType(system_type))
		{
			system = (*itbegin);
			break;
		}
		++itbegin;
	}
	
	if( system == nullptr )
	{
		// +---------
		// | create a new system to handle this animation
		// +---------
		
		kstl::string systemName=getName();
		systemName += "_AnimationSystem";
		
		system=(ABaseSystem*) KigsCore::GetInstanceOf(systemName, system_type);
		
		if(system != nullptr)
		{
			
			AddSystem( system );
			
			// +---------
			// | set the hierarchy
			// +---------
			
			if(system->UseOwnHierarchy() == true)
			{
				// search AObjectSkeletonResource in sons
				auto itfound = m_ObjectResourceMap.find("AObjectSkeletonResource");
				
				if (itfound != m_ObjectResourceMap.end())
				{
					system->SetHierarchy((AObjectSkeletonResource*)(*itfound).second);
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
			stream->Destroy();
			
			auto it = m_ALinksTable.find(info);
			m_ALinksTable.erase(it);
			delete tmp_links;
			
			return; 
		}
	}
	
	tmp_links->SetSystem(system);
	
	// +---------
	// | search for the good channel and add the streams
	// +---------
	
	
	IntU32 g_id=info->GetStreamGroupID(0);
	
	ABaseChannel* channel=system->GetChannelByUID(g_id);
	bool    one_stream_is_ok=false;
	
	if(channel != nullptr)
	{
		channel->addItem(stream);
		stream->Destroy();
		tmp_links->AddStream(stream,0);
		stream->InitFromResource(info,0);
		one_stream_is_ok=true;
	}
	else
	{
		stream->Destroy();
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
		
		stream=(ABaseStream*)KigsCore::GetInstanceOf(streamName, StreamClassName);
		
		g_id=info->GetStreamGroupID(i);
		
		channel=system->GetChannelByUID(g_id);
		
		if(channel != nullptr)
		{
			channel->addItem(stream);
			stream->Destroy();
			tmp_links->AddStream(stream,streamindex);
			++streamindex;
			stream->InitFromResource(info,i);
			one_stream_is_ok=true;
		}
		else
		{
			stream->Destroy();
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
		auto it = m_ALinksTable.find(info);
		m_ALinksTable.erase(it);
		delete(tmp_links);
		if(system->GetValidStream() == nullptr)
		{
			RemoveSystem( system );
			system->Destroy();
		}
	}
};

// ******************************
// * RemoveAnimation
// *-----------------------------
// * - delete an animation from this object, also delete the corresponding streams
// * - and if needed destroy the corresponding system
// ******************************

void    AObject::RemoveAnimation(AnimationResourceInfo* info)
{
	// +---------
	// | first look for the slot in the list 
	// +---------
	
	if(m_ALinksTable.find(info) == m_ALinksTable.end())
	{
		return;
	}
	
	ALinks* tmp_links=m_ALinksTable[info];
	
	if(tmp_links == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	ABaseSystem* sys=tmp_links->GetSystem();
	
	ABaseStream** stream_array=tmp_links->GetStreamArray();
	
	IntU32    i;
	
	for(i=0;i<tmp_links->GetStreamCount();++i)
	{
		ABaseChannel* channel=stream_array[i]->GetChannel();
		channel->removeItem((stream_array[i]));
	}
	
	
	sys->RemoveLinks();
	
	auto it = m_ALinksTable.find(info);
	m_ALinksTable.erase(it);
	delete (tmp_links);
	
	// +---------
	// | check is system should be deleted
	// +---------
	
	if(sys->GetLinksCount() == 0)
	{
		RemoveSystem(sys);
		sys->Destroy();  
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

void    AObject::SetLocalToGlobalMode(unsigned int system_type, int mode)
{
	kstl::set<ABaseSystem*>::iterator itbegin = m_pSystemSet.begin();
	kstl::set<ABaseSystem*>::iterator itend = m_pSystemSet.end();
	
	while (itbegin != itend)
	{
		if ((*itbegin)->isSubType(system_type))
		{
			if (mode == FromAnimation)
			{
				(*itbegin)->UseAnimationLocalToGlobalData(true);
				(*itbegin)->UpdateLocalToGlobalWhenLoop(false);
			}
			else if (mode == FromAnimationWithLoop)
			{
				(*itbegin)->UseAnimationLocalToGlobalData(true);
				(*itbegin)->UpdateLocalToGlobalWhenLoop(true);
			}
			else if (mode == FromEngine)
			{
				(*itbegin)->UseAnimationLocalToGlobalData(false);
			}
			
			break;
		}
		++itbegin;
	}
	
};

// ----------------------------------------------------------------------------




// *******************
// * LinkTo
// * -----------------
// * 
// * 
// *******************

void    AObject::LinkTo(unsigned int   system_type,AObject* other_object,IntU32 g_id)
{
	ABaseSystem*  sys1 = GetSystemByType(system_type);
	ABaseSystem*  sys2 = other_object->GetSystemByType(system_type);
	
	if((sys1 != nullptr) && (sys2 != nullptr))
	{
		ABaseChannel*   channel=sys2->GetChannelByUID(g_id);
		if(channel)
			sys1->LinkTo(channel);
	}
};

void    AObject::UnLink(unsigned int system_type)
{
	ABaseSystem*  sys1= GetSystemByType(system_type);
	
	if(sys1 != nullptr)
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

ABaseSystem*    AObject::GetSystemByType(unsigned int system_type)
{
	kstl::set<ABaseSystem*>::iterator itbegin = m_pSystemSet.begin();
	kstl::set<ABaseSystem*>::iterator itend = m_pSystemSet.end();
	
	while (itbegin != itend)
	{
		if ((*itbegin)->isSubType(system_type))
		{
			return (*itbegin);
		}
		++itbegin;
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

ABaseSystem*    AObject::GetSystembyIndex(IntU32 index)
{
	
	IntU32   i=0;
	kstl::set<ABaseSystem*>::iterator itbegin = m_pSystemSet.begin();
	kstl::set<ABaseSystem*>::iterator itend = m_pSystemSet.end();
	
	while (itbegin != itend)
	{
		if (i == index)
		{
			return (*itbegin);
		}
		++i;
		++itbegin;
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
	for(auto it = m_ALinksTable.begin(); it != m_ALinksTable.end();++it)
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

void    AObject::StartAnimation(AnimationResourceInfo* info,ATimeValue  t,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&t,&AObject::StartAnimationFor);
};


// ******************************
// * SetAnimationPos
// *-----------------------------
// * set the animation time for the given % of animation length
// * 
// ******************************

void    AObject::SetAnimationPos(AnimationResourceInfo* info,Float  percent,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&percent,&AObject::SetAnimationPosFor);
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

void    AObject::StartRepeatAnimation(AnimationResourceInfo* info,ATimeValue  t,IntU32 n,IntU32* g_id,IntU32 g_count)
{
	repeatAnimationParamStruct params;
	params.repeatCount = n;
	params.t = t;
	DoForEachStream(info,g_id,g_count,&params,&AObject::StartRepeatAnimationFor);
};

// ******************************
// * StopAnimation
// *-----------------------------
// * - stop playing the given animation 
// * - 
// ******************************

void    AObject::StopAnimation(AnimationResourceInfo* info,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,nullptr,&AObject::StopAnimationFor);
};

// ******************************
// * StopAllAnimations
// *-----------------------------
// * - stop playing all the animations
// * - 
// ******************************

void    AObject::StopAllAnimations(IntU32* g_id,IntU32 g_count)
{
	
	for(auto it=m_ALinksTable.begin();it!=m_ALinksTable.end();++it)
	{
		ALinks* tmp_links=(ALinks*)(*it).second;
		DoForEachStream(tmp_links->GetAnimResourceInfo(), g_id,g_count,nullptr,&AObject::StopAnimationFor);
	}
};

// ******************************
// * ResumeAnimation
// *-----------------------------
// * - restart the animation where it was stopped
// * - 
// ******************************

void    AObject::ResumeAnimation(AnimationResourceInfo* info,ATimeValue  t,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&t,&AObject::ResumeAnimationFor);
};

// ******************************
// * SetAnimationWeight
// *-----------------------------
// * - change the weight of an animation
// * - 
// ******************************

void    AObject::SetAnimationWeight(AnimationResourceInfo* info,Float weight,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&weight,&AObject::SetAnimationWeightFor);
};

// ******************************
// * SetAnimationSpeed
// *-----------------------------
// * - change the speed of an animation
// * - 
// ******************************

void    AObject::SetAnimationSpeed(AnimationResourceInfo* info,Float speed,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&speed,&AObject::SetAnimationSpeedFor);
};


// ******************************
// * MulAnimationWeight
// *-----------------------------
// * - multiply the weight of an animation
// * - 
// ******************************

void    AObject::MulAnimationWeight(AnimationResourceInfo* info,Float weight,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&weight,&AObject::MulAnimationWeightFor);
};

// ******************************
// * MulAnimationSpeed
// *-----------------------------
// * - multiply the speed of an animation
// * - 
// ******************************

void    AObject::MulAnimationSpeed(AnimationResourceInfo* info,Float speed,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&speed,&AObject::MulAnimationSpeedFor);
};

// ******************************
// * FadeAnimationTo
// *-----------------------------
// * - fade from one animation to another, both animations are supposed to be playing
// * - 
// ******************************

void    AObject::FadeAnimationTo(AnimationResourceInfo* info1,AnimationResourceInfo* info2,ATimeValue  fade_length,ATimeValue  t)
{
	
	if(m_ALinksTable.find(info1) == m_ALinksTable.end())
	{
		return;
	}
	ALinks* tmp_links=m_ALinksTable[info1];
	
	if(tmp_links == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	ABaseStream* stream=(tmp_links->GetStreamArray())[0];
	Float w=stream->GetWeight();
	
	// animations can not be faded twice at the same time, so stop 
	// other fades 
	
	kstl::vector<ALinearInterp*>::iterator it=m_FadeList.begin();
	
	while (it != m_FadeList.end())
	{
		ALinearInterp*  tmpFade=*it;
		
		if ( (tmpFade->GetData() == info2) || (tmpFade->GetData() == info1))
		{
			it=m_FadeList.erase(it);
			delete (tmpFade);
		}
		else
		{
			++it;
		}
	}   
	
	// add fade for first animation
	
	ALinearInterp* fade=new ALinearInterp(w,KFLOAT_CONST(0.0f),t,fade_length,info1);
	m_FadeList.push_back(fade);
	
	SetAnimationWeight(info2,KFLOAT_CONST(0.0f));
	
	// add fade for second animation
	
	fade=new ALinearInterp(KFLOAT_CONST(0.0),KFLOAT_CONST(1.0f),t,fade_length,info2);
	m_FadeList.push_back(fade);
};

// ******************************
// * SynchroniseAnimations
// *-----------------------------
// * - set speed of animation2 so that animation1 reach is local time synchro1 at the same time
// *  as animation2 reach is local time synchro2
// ******************************

void    AObject::SynchroniseAnimations(AnimationResourceInfo* info1,AnimationResourceInfo* info2,ATimeValue  synchro1,ATimeValue  synchro2)
{
	// +---------
	// | first look for the slot in the list 
	// +---------
	
	if(m_ALinksTable.find(info1) == m_ALinksTable.end())
	{
		return;
	}
	if(m_ALinksTable.find(info2) == m_ALinksTable.end())
	{
		return;
	}
	
	ALinks* links1=m_ALinksTable[info1];
	
	if(links1 == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	ALinks* links2=m_ALinksTable[info2];
	
	if(links2 == nullptr)
	{
		// this animation is not in the table
		return;
	}
	
	
	ABaseStream* stream1=(links1->GetStreamArray())[0];
	ABaseStream* stream2=(links2->GetStreamArray())[0];
	
	if(stream1->m_Speed == KFLOAT_CONST(0.0f))
	{
		SetAnimationSpeed(info1,KFLOAT_CONST(1.0f));
	}
	
	Float coef=(Float)synchro1+((Float)(stream1->m_StartTime-stream2->m_StartTime)*stream1->m_Speed);
	if(coef != KFLOAT_CONST(0.0f))
	{
		SetAnimationSpeed(info2,(Float)synchro2*stream1->m_Speed/coef);
	}
	
};

// ******************************
// * HasAnimationLoop
// *-----------------------------
// * - 
// * - 
// ******************************

bool    AObject::HasAnimationLoop(AnimationResourceInfo* info1)
{
	
	if(m_ALinksTable.find(info1) == m_ALinksTable.end())
	{
		return false;
	}
	
	ALinks* links1=m_ALinksTable[info1];
	
	if(links1 == nullptr)
	{
		// this animation is not in the table
		return false;
	}
	
	ABaseStream* stream1=(links1->GetStreamArray())[0];
	if(stream1 != nullptr)
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

bool    AObject::AnimationIsSet(AnimationResourceInfo* info1)
{
	if(m_ALinksTable.find(info1) == m_ALinksTable.end())
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

void    AObject::SetLoop(AnimationResourceInfo* info,bool loop,IntU32* g_id,IntU32 g_count)
{
	DoForEachStream(info,g_id,g_count,&loop,&AObject::SetLoopFor);
};

// ******************************
// * HasAnimationReachEnd
// *-----------------------------
// * - 
// * - 
// ******************************

bool    AObject::HasAnimationReachEnd(AnimationResourceInfo* info1)
{
	
	if(m_ALinksTable.find(info1) == m_ALinksTable.end())
	{
		return false;
	}
	
	ALinks* links1=m_ALinksTable[info1];
	
	if(links1 == nullptr)
	{
		// this animation is not in the table
		return false;
	}
	
	ABaseStream* stream1=(links1->GetStreamArray())[0];
	if(stream1 != nullptr)
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

void    AObject::AddSystem(ABaseSystem* system)
{
	
	system->SetAObject(this);
	m_pSystemSet.insert(system);
	
};

// ******************************
// * RemoveSystem
// *-----------------------------
// * - if the given system is in the list, remove it
// * -
// ******************************

void    AObject::RemoveSystem(ABaseSystem* system)
{
	m_pSystemSet.erase(system);
};

// ******************************
// * DoForEachStream
// *-----------------------------
// * - call the given methods for all streams given in info
// * - 
// ******************************

void    AObject::DoForEachStream(AnimationResourceInfo* info,IntU32* g_id,IntU32 g_count,void* params,void (AObject::*callfunc)(ABaseStream* stream,void* param))
{
	// +---------
	// | first look for the slot in the list 
	// +---------
	
	if(m_ALinksTable.find(info) == m_ALinksTable.end())
	{
		return;
	}
	
	ALinks* tmp_links=(ALinks*)m_ALinksTable[info];
	
	if(tmp_links == nullptr)
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
			ABaseChannel* channel=tmp_links->GetSystem()->GetChannelByUID(g_id[0]);
			if(channel != nullptr)
			{
				id_list=tmp_links->GetSystem()->GetSonGroupIDList(channel,id_count);
			}
		}
		else
		{
			id_list=g_id;
		}
		
		IntU32    i;
		
		for(i=0;i<tmp_links->GetStreamCount();++i)
		{
			if(IsInGroupIDList((tmp_links->GetStreamArray())[i]->GetChannel()->GetGroupID(),id_list,id_count))
			{
				(this->*callfunc)((tmp_links->GetStreamArray())[i],params);
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
		
		for(i=0;i<tmp_links->GetStreamCount();++i)
		{
			(this->*callfunc)((tmp_links->GetStreamArray())[i],params);
		}
	}
};


// ******************************
// * StartAnimation
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::StartAnimationFor(ABaseStream* stream,void* param)
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

void    AObject::SetAnimationPosFor(ABaseStream* stream,void* param)
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

void    AObject::StartRepeatAnimationFor(ABaseStream* stream,void* param)
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

void    AObject::StopAnimationFor(ABaseStream* stream,void* /* param */)
{
	stream->Stop();
};

// ******************************
// * ResumeAnimation
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::ResumeAnimationFor(ABaseStream* stream,void* param)
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

void    AObject::SetAnimationWeightFor(ABaseStream* stream,void* param)
{
	stream->SetWeight(*((Float*)param));
};

// ******************************
// * SetAnimationSpeed
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::SetAnimationSpeedFor(ABaseStream* stream,void* param)
{
	stream->SetSpeed(*((Float*)param));
};

// ******************************
// * MulAnimationWeight
// *-----------------------------
// * - method called by DoForEachStream
// * - 
// ******************************

void    AObject::MulAnimationWeightFor(ABaseStream* stream,void* param)
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

void    AObject::MulAnimationSpeedFor(ABaseStream* stream,void* param)
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

void    AObject::SetLoopFor(ABaseStream* stream,void* param)
{
	stream->SetLoop(*((bool*)param));
};


bool	AObject::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType(AObjectResource::myClassID))
	{
		// search if this resource type is already there
		auto itfound = m_ObjectResourceMap.find(item->getExactType());
		
		if (itfound != m_ObjectResourceMap.end())
		{
			removeItem((*itfound).second);
		}
		
		m_ObjectResourceMap[item->getExactType()] = (AObjectResource*)item;
	}
	return Drawable::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool	AObject::removeItem(CoreModifiable *item DECLARE_LINK_NAME)
{
	if(item->isSubType(AObjectResource::myClassID))
	{
		auto	itfound= m_ObjectResourceMap.find(item->getExactType());
		
		if (itfound != m_ObjectResourceMap.end())
		{
			m_ObjectResourceMap.erase(itfound);
		}
	}
	return Drawable::removeItem(item PASS_LINK_NAME(linkName));
}


void AObject::DoPreDraw(TravState * state)
{
	//! update 
	//Animate(((kfloat)state->GetTime()));
	kstl::set<ABaseSystem*>::iterator itbegin = m_pSystemSet.begin();
	kstl::set<ABaseSystem*>::iterator itend = m_pSystemSet.end();
	while(itbegin != itend)
	{
		(*itbegin)->SetupDraw();
		++itbegin;
	}
	
	//! then call Base DoPreDraw
	Drawable::DoPreDraw(state);
}

void AObject::Update(const Timer& timer, void* addParam)
{
	Animate(timer.GetTime());
}
