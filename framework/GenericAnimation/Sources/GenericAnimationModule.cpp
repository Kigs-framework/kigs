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

GenericAnimationModule* gGenericAnimationModule = nullptr;

IMPLEMENT_CLASS_INFO(GenericAnimationModule)

//! constructor
GenericAnimationModule::GenericAnimationModule(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
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
void GenericAnimationModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
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

	kstl::map<kstl::string, CoreRawBuffer*>::iterator it;
	for(it=mResourceInfoMap.begin();it!=mResourceInfoMap.end();++it)
	{
		CoreRawBuffer* result=((*it).second);
		result->Destroy();
	}

	mResourceInfoMap.clear();
}    

//! module update
void GenericAnimationModule::Update(const Timer& timer, void* addParam)
{
	ManagePostAdd();
	BaseUpdate(timer, addParam);
}    


AnimationResourceInfo*	GenericAnimationModule::LoadAnimation(const kstl::string& fileName)
{
	if(mResourceInfoMap.find(fileName) != mResourceInfoMap.end())
	{
		return (AnimationResourceInfo*)mResourceInfoMap[fileName]->buffer();
	}

	CoreRawBuffer* result=0;

	SP<FilePathManager>	pathManager=KigsCore::GetSingleton("FilePathManager");

	SmartPointer<FileHandle> fullfilenamehandle;

	if (pathManager)
	{
		fullfilenamehandle = pathManager->FindFullName(fileName);
	}

	if(fullfilenamehandle)
	{
		u64 size;
		result =ModuleFileManager::LoadFile(fullfilenamehandle.get(),size);

		if(result)
		{
			mResourceInfoMap[fileName]=result;
		}
	}
	return (AnimationResourceInfo*)result->buffer();
}



void	GenericAnimationModule::UnLoad(const kstl::string& fileName)
{
	if(mResourceInfoMap.find(fileName) == mResourceInfoMap.end())
	{
		return;
	}

	CoreRawBuffer* result=mResourceInfoMap[fileName];

	result->Destroy();

	mResourceInfoMap.erase(mResourceInfoMap.find(fileName));

}

void	GenericAnimationModule::UnLoad(AnimationResourceInfo* info)
{
	kstl::map<kstl::string, CoreRawBuffer*>::iterator it;

	CoreRawBuffer* result =0;
	for(it=mResourceInfoMap.begin();it!=mResourceInfoMap.end();++it)
	{
		if(((AnimationResourceInfo*)(*it).second->buffer())==info)
		{
			result = (*it).second;
			mResourceInfoMap.erase(it);
			break;
		}
	}

	if (result)
	{
		result->Destroy();
	}
}
