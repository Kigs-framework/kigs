//#include "PrecompiledHeaders.h"
#include "ModuleSoundManager.h"
#include "AudioSource.h"
#include "AudioBuffer.h"
#include "AudioListener.h"

IMPLEMENT_CLASS_INFO(AudioSource)


AudioSource::AudioSource(const kstl::string& name,CLASS_NAME_TREE_ARG)
: CoordinateSystem(name,PASS_CLASS_NAME_TREE_ARG),
myPitch(*this,false,LABEL_AND_ID(Pitch),KFLOAT_CONST(1.0)),
myGain(*this,false,LABEL_AND_ID(Gain),KFLOAT_CONST(1.0)),
myIsLooping(*this,false,LABEL_AND_ID(IsLooping),false),
myIsPlaying(*this,false,LABEL_AND_ID(IsPlaying),false),
myState(*this,false,LABEL_AND_ID(State),0),
m_ReferenceDistance(*this,false,LABEL_AND_ID(Reference Distance),KFLOAT_CONST(1.0f)),
m_MaxDistance(*this,false,LABEL_AND_ID(Maximum Distance),KFLOAT_CONST(1000.0f)),
m_RollOfFactor(*this,false,LABEL_AND_ID(Roll Off Factor),KFLOAT_CONST(1.0f))
{
	myBuffer=0;
	// retreive listener stack
	kstl::set<CoreModifiable*>	instances;
	CoreModifiable::GetInstances("ModuleSoundManager",instances);

	// we know only one instance is available
	mySoundManager=(ModuleSoundManager*)(*instances.begin());

}     


AudioSource::~AudioSource() {}


bool AudioSource::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if(item->isSubType(AudioBuffer::myClassID))
	{
		if(myBuffer)
		{
			myBuffer->Empty(this);
			removeItem(myBuffer PASS_LINK_NAME(linkName));
		}
		
		myBuffer=(AudioBuffer*)item;
		myBuffer->Stream(this);
	}
	return CoordinateSystem::addItem(item,pos PASS_LINK_NAME(linkName));
}

bool AudioSource::removeItem(CoreModifiable* item DECLARE_LINK_NAME)
{
	if(item->isSubType(AudioBuffer::myClassID))
	{
		int PresentCount=0;
		for (size_t i=0; i<getItems().size(); i++)
			if (getItems()[i].myItem==item)
				PresentCount++;
		if(PresentCount==0)
			return false;

		if(myBuffer == (AudioBuffer*)item)
		{
			myBuffer->Empty(this);
			myBuffer=0;
		}
	}
	return CoordinateSystem::removeItem(item  PASS_LINK_NAME(linkName));
}

bool	AudioSource::Draw(TravState* state)
{
	if(mySoundManager->CurrentListenerIsActive())
	{
		Protected1Draw(state);
	}
	return true;
}
