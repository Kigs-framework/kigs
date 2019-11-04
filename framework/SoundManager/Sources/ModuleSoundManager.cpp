#include "ModuleSoundManager.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include "TravState.h"
#include "Timer.h"
#include "Core.h"

#ifdef _KIGS_ONLY_STATIC_LIB_
#include "Platform/Sound/Sound.h"
#endif

IMPLEMENT_CLASS_INFO(ModuleSoundManager)


ModuleSoundManager::ModuleSoundManager(const kstl::string& name,CLASS_NAME_TREE_ARG) :
ModuleBase(name,PASS_CLASS_NAME_TREE_ARG),
m_DistanceModel(*this,false,LABEL_AND_ID(DistanceModel),"Exponent","Linear","Inverse","None"),
m_DistanceClamp(*this,false,LABEL_AND_ID(Clamp),true)
{
	
}

ModuleSoundManager::~ModuleSoundManager() {}    


void ModuleSoundManager::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core, "ModuleSoundManager", params);

	DECLARE_FULL_CLASS_INFO(core,AudioListenerActivator,AudioListenerActivator,SoundManager)

	#ifdef _KIGS_ONLY_STATIC_LIB_
	RegisterDynamic(PlatformSoundModuleInit(core,params));
	#endif
}


void ModuleSoundManager::Close()
{
    BaseClose();
}    


void ModuleSoundManager::Update(const Timer& timer, void* addParam)
{
	// draw all source sons without need of a scene graph
	TravState* state= (TravState*)(KigsCore::GetInstanceOf("SceneTravState",_S_2_ID("TravState")));
	state->SetTime(timer.GetTime());
	
	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for(it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).myItem->isSubType(AudioSource::myClassID))
		{
			((AudioSource*)(*it).myItem)->Protected1Draw(state);
		}
	}
	state->Destroy();
	
    BaseUpdate(timer,addParam);
}    

kstl::vector<AudioListener*>&	ModuleSoundManager::GetListenerStack()
{
	return MyListenerStack;
}

bool	ModuleSoundManager::CurrentListenerIsActive()
{
	unsigned long currentsize=MyListenerStack.size();
	if(currentsize)
	{
		return MyListenerStack[currentsize-1]->ListenerIsActive();
	}
	return false;
}
