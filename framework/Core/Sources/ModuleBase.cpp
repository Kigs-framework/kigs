#include "PrecompiledHeaders.h"

#include "ModuleBase.h"
#include "Core.h"
#include "XMLIncludes.h"
#include "ModuleFileManager.h"
#include "Timer.h"

#if __has_include("Platform/Core/ModuleBase.inl")
#include "Platform/Core/ModuleBase.inl"
#else
DynamicModuleHandleAndPointer PlatformLoadDLL(const std::string&) { return {}; }
void PlatformCloseDLL(void* handle){}
#endif


//! ModuleDescription static members
int ModuleDescription::myCurrentReadDepth=0;
bool ModuleDescription::myIsGoodFile;

//! implement static members for ModuleBase
IMPLEMENT_CLASS_INFO(ModuleBase);

//! utility method: return the list of all dll found in the XML file
kstl::vector<kstl::string>&    ModuleDescription::GetDllList()
{
	return myDllList;
}

//! init a module description by reading dll list in given XML file
bool ModuleDescription::Init(const kstl::string& fileName)
{
	//! load description file
	XML*	xml=(XML*)XML::ReadFile(fileName,0);
	myIsInitialised=false;
	myIsGoodFile=true;

	if(xml)
	{
		XMLNode* root= (XMLNode*)xml->getRoot();

		//! check file validity
		if(root->getType() != XML_NODE_ELEMENT)
		{
			myIsGoodFile=false;
		}
		else
		{
			if(root->getName() != "ModuleDescription")
			{
				myIsGoodFile=false;
			}
			else
			{
				//! get module name
				XMLAttribute* attribute=root->getAttribute("name");
				if(attribute)
				{
					myModuleName=attribute->getString();
				}
				else
				{
					myIsGoodFile=false;
				}
			}
		}

		if(myIsGoodFile)
		{
			//! get all dll names
			for ( int i = 0; i < root->getChildCount( ); ++i )
			{
				XMLNode* dllnode=root->getChildElement( i );
				if(dllnode)
				{
					if( (dllnode->getName() == "fileName")&&(dllnode->getChildCount()) )
					{
						XMLNode*	textnode=dllnode->getChildElement( 0 );
#if defined WIN64
						kstl::string str = textnode->getString()+"64.dlk";
#elif defined WIN32
						kstl::string str = textnode->getString()+"32.dlk";
#else
						kstl::string str = textnode->getString()+".dlk";
#endif
						myDllList.push_back(str);
					}
				}
			}
			myIsInitialised=true;
		}

		delete xml;
	}

	return myIsInitialised;
}

//! init for a module : find the XML description file, and try to load and init all associated dll
void    ModuleBase::BaseInit(KigsCore* core,const kstl::string& moduleName, const kstl::vector<CoreModifiableAttribute*>* params)
{
	KIGS_MESSAGE("Init Base Module : "+moduleName);
	myCore = core;
	KigsCore::ModuleInit(core,this);
	RegisterToCore();

	//! load dll and init
	ModuleDescription description;

	kstl::string filename=moduleName;
	filename+=".xml";

	bool isok=description.Init(filename);

	if(isok)
	{
		kstl::vector<kstl::string>& dlllist=description.GetDllList();

		kstl::vector<kstl::string>::iterator it;

		//! for each found dll call ModuleInit
		for(it=dlllist.begin();it!=dlllist.end();++it)
		{
			auto toAdd = PlatformLoadDLL(*it);
			if (toAdd.myInstance)
			{
				myDynamicModuleList.push_back(toAdd);
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
void    ModuleBase::RegisterDynamic(ModuleBase* dynamic)
{
	DynamicModuleHandleAndPointer toAdd;
	toAdd.myHandle=0;
	toAdd.myInstance=dynamic;
	myDynamicModuleList.push_back(toAdd);
}
#endif

//! call all dll "ModuleClose" method and free all dll loaded
void    ModuleBase::BaseClose()
{
	EmptyItemList();
	kstl::vector<DynamicModuleHandleAndPointer>::iterator it;

	for(it=myDynamicModuleList.begin();it!=myDynamicModuleList.end();++it)
	{
		it->myInstance->Close();
		it->myInstance->Destroy();
		PlatformCloseDLL(it->myHandle);
	}
	myDynamicModuleList.clear();
}

//! call all dll "ModuleUpdate" method
void    ModuleBase::BaseUpdate(const Timer& timer, void* addParam)
{
	for(auto m : myDynamicModuleList)
	{
		m.myInstance->CallUpdate(timer,addParam);
	}
}
