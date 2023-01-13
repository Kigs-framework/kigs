#include "Timer.h"
#include "AObject.h"
#include "Core.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "CoreModifiable.h"
#include <stdio.h>
#include "GenericAnimationModule.h"
#include "Bones/APRSKeyStream.h"
#include "Bones/ABoneSystem.h"
#include "Bones/ABoneChannel.h"
#include "Bones/APRSControlStream.h"
#include "Bones/AObjectSkeletonResource.h"

using namespace Kigs::Anim;

GenericAnimationModule* Kigs::Anim::gGenericAnimationModule = nullptr;

IMPLEMENT_CLASS_INFO(GenericAnimationModule)

//! constructor
GenericAnimationModule::GenericAnimationModule(const std::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{

	mResourceInfoMap.clear();

	mPostAddShaderList.clear();

	gGenericAnimationModule = this;
}


//! destructor
GenericAnimationModule::~GenericAnimationModule()
{
	gGenericAnimationModule = nullptr;
}    

//! module init, register FilePathManager
void GenericAnimationModule::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"Animation",params);  
	
	DECLARE_FULL_CLASS_INFO(core,AObject,AObject,Animation)
	DECLARE_FULL_CLASS_INFO(core, ABoneSystem, ABoneSystem, Animation)
	DECLARE_FULL_CLASS_INFO(core, ABoneChannel, ABoneChannel, Animation)
	DECLARE_FULL_CLASS_INFO(core, APRSKeyStream, APRSKeyStream, Animation)
	DECLARE_FULL_CLASS_INFO(core, APRSControlStream, APRSControlStream, Animation)
	DECLARE_FULL_CLASS_INFO(core, AObjectSkeletonResource, AObjectSkeletonResource, Animation)


}

//! module close
void GenericAnimationModule::Close()
{

    BaseClose();
	mResourceInfoMap.clear();
}    

//! module update
void GenericAnimationModule::Update(const Time::Timer& timer, void* addParam)
{
	ManagePostAdd();
	BaseUpdate(timer, addParam);
}    


AnimationResourceInfo*	GenericAnimationModule::LoadAnimation(const std::string& fileName)
{
	if(mResourceInfoMap.find(fileName) != mResourceInfoMap.end())
	{
		return (AnimationResourceInfo*)mResourceInfoMap[fileName]->buffer();
	}

	SP<CoreRawBuffer> result=0;

	SP<File::FilePathManager>	pathManager=KigsCore::GetSingleton("FilePathManager");

	SmartPointer<File::FileHandle> fullfilenamehandle;

	if (pathManager)
	{
		fullfilenamehandle = pathManager->FindFullName(fileName);
	}

	if(fullfilenamehandle)
	{
		u64 size;
		result = File::ModuleFileManager::LoadFile(fullfilenamehandle.get(),size);

		if(result)
		{
			mResourceInfoMap[fileName]=result;
		}
	}
	return (AnimationResourceInfo*)result->buffer();
}



void	GenericAnimationModule::UnLoad(const std::string& fileName)
{
	if(mResourceInfoMap.find(fileName) == mResourceInfoMap.end())
	{
		return;
	}

	mResourceInfoMap.erase(mResourceInfoMap.find(fileName));

}

void	GenericAnimationModule::UnLoad(AnimationResourceInfo* info)
{
	std::map<std::string, SP<CoreRawBuffer>>::iterator it;

	CoreRawBuffer* result =0;
	for(it=mResourceInfoMap.begin();it!=mResourceInfoMap.end();++it)
	{
		if(((AnimationResourceInfo*)(*it).second->buffer())==info)
		{
			mResourceInfoMap.erase(it);
			break;
		}
	}
}
