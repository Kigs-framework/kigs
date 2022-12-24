#include "PrecompiledHeaders.h"

#include "ModuleBase.h"
#include "Core.h"
#include "XMLIncludes.h"
#include "ModuleFileManager.h"
#include "Timer.h"

using namespace Kigs::Core;
using namespace Kigs::Xml;

#if __has_include("Platform/Core/ModuleBase.inl")
#include "Platform/Core/ModuleBase.inl"
#else
DynamicModuleHandleAndPointer PlatformLoadDLL(const std::string&) { return {}; }
void PlatformCloseDLL(void* handle){}
#endif


//! ModuleDescription static members
int ModuleDescription::mCurrentReadDepth=0;
bool ModuleDescription::mIsGoodFile;

//! implement static members for ModuleBase
IMPLEMENT_CLASS_INFO(ModuleBase);

//! utility method: return the list of all dll found in the XML file
std::vector<std::string>&    ModuleDescription::GetDllList()
{
	return mDllList;
}

//! init a module description by reading dll list in given XML file
bool ModuleDescription::Init(const std::string& fileName)
{
	//! load description file
	XML*	xml=(XML*)XML::ReadFile(fileName,0);
	mIsInitialised=false;
	mIsGoodFile=true;

	if(xml)
	{
		XMLNodeBase* root= (XMLNodeBase*)xml->getRoot();

		//! check file validity
		if(root->getType() != XML_NODE_ELEMENT)
		{
			mIsGoodFile=false;
		}
		else
		{
			if(!root->compareName("ModuleDescription"))
			{
				mIsGoodFile=false;
			}
			else
			{
				//! get module name
				XMLAttributeBase* attribute=root->getAttribute("name");
				if(attribute)
				{
					mModuleName=attribute->getString();
				}
				else
				{
					mIsGoodFile=false;
				}
			}
		}

		if(mIsGoodFile)
		{
			//! get all dll names
			for ( int i = 0; i < root->getChildCount( ); ++i )
			{
				XMLNodeBase* dllnode=root->getChildElement( i );
				if(dllnode)
				{
					if( (dllnode->getName() == "fileName")&&(dllnode->getChildCount()) )
					{
						XMLNodeBase*	textnode=dllnode->getChildElement( 0 );
#if defined WIN64
						std::string str = textnode->getString()+"64.dlk";
#elif defined WIN32
						std::string str = textnode->getString()+"32.dlk";
#else
						std::string str = textnode->getString()+".dlk";
#endif
						mDllList.push_back(str);
					}
				}
			}
			mIsInitialised=true;
		}

		delete xml;
	}

	return mIsInitialised;
}

//! init for a module : find the XML description file, and try to load and init all associated dll
void    ModuleBase::BaseInit(KigsCore* core,const std::string& moduleName, const std::vector<CoreModifiableAttribute*>* params)
{
	KIGS_MESSAGE("Init Base Module : "+moduleName);
	mCore = core;
	KigsCore::ModuleInit(core,this);
	RegisterToCore();

	//! load dll and init
	ModuleDescription description;

	std::string filename=moduleName;
	filename+=".xml";

	bool isok=description.Init(filename);

	if(isok)
	{
		std::vector<std::string>& dlllist=description.GetDllList();

		std::vector<std::string>::iterator it;

		//! for each found dll call ModuleInit
		for(it=dlllist.begin();it!=dlllist.end();++it)
		{
			auto toAdd = PlatformLoadDLL(*it);
			if (toAdd.mInstance)
			{
				mDynamicModuleList.push_back(toAdd);
			}
		}
	}
	else
	{
		//! no dynamic module
		//! just register static one
	}
	
	InitParametersFromList(params);
}

#ifdef _KIGS_ONLY_STATIC_LIB_
void    ModuleBase::RegisterDynamic(SP<ModuleBase> dynamic)
{
	DynamicModuleHandleAndPointer toAdd;
	toAdd.mHandle=0;
	toAdd.mInstance=dynamic;
	mDynamicModuleList.push_back(toAdd);
}
#endif

//! call all dll "ModuleClose" method and free all dll loaded
void    ModuleBase::BaseClose()
{
	EmptyItemList();
	std::vector<DynamicModuleHandleAndPointer>::iterator it;

	for(it=mDynamicModuleList.begin();it!=mDynamicModuleList.end();++it)
	{
		it->mInstance->Close();
		PlatformCloseDLL(it->mHandle);
	}
	mDynamicModuleList.clear();
}

//! call all dll "ModuleUpdate" method
void    ModuleBase::BaseUpdate(const Timer& timer, void* addParam)
{
	for(auto m : mDynamicModuleList)
	{
		m.mInstance->CallUpdate(timer,addParam);
	}
}
