#include "PrecompiledHeaders.h"
#include "CoreBaseApplication.h"
#include "Core.h"
#include "CoreIncludes.h"
#include "TimerIncludes.h"
#include "ModuleFileManager.h"
#include "CoreAutoRelease.h"
#include "TimeProfiler.h"

//#define PRINT_REFTRACING

IMPLEMENT_CLASS_INFO(CoreBaseApplication)

// 
CoreBaseApplication::CoreBaseApplication(const kstl::string& name, CLASS_NAME_TREE_ARG) :  PlatformBaseApplication(), CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
	myHasFocus         = true;
	myNeedExit         = false;
	myInitBaseModules  = false;
	myAlreadyInUpdate  = false;
	myApplicationTimer = 0;
	myUpdateSleepTime  = 0;
	myBackKeyState     = 0;
	KigsCore::SetCoreApplication(this);
	myAutoUpdateList.clear();
	myEditor		   = NULL;

	
}

CoreBaseApplication::~CoreBaseApplication()
{
	
}

void CoreBaseApplication::AddAutoUpdate(CoreModifiable*	toUpdate)
{
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	// check if not already there
	kstl::vector<CoreModifiable*>::const_iterator itAutoUpdate;
	for(itAutoUpdate = myAutoUpdateList.begin();itAutoUpdate != myAutoUpdateList.end();++itAutoUpdate)
	{
		if((*itAutoUpdate) == toUpdate)
		{
			// already in the list, do nothing
			return;
		}
	}
	toUpdate->flagAsAutoUpdateRegistered();
	myAutoUpdateList.push_back(toUpdate);

}

void CoreBaseApplication::RemoveAutoUpdate(CoreModifiable*	toUpdate)
{
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	// search in list
	kstl::vector<CoreModifiable*>::iterator itAutoUpdate;
	if(!myAutoUpdateList.empty())
	{
		for(itAutoUpdate = myAutoUpdateList.begin();itAutoUpdate != myAutoUpdateList.end();++itAutoUpdate)
		{
			if((*itAutoUpdate) == toUpdate)
			{
				myAutoUpdateList.erase(itAutoUpdate);
				toUpdate->unflagAsAutoUpdateRegistered();
				// no duplicate here, so can exit directly
				return;
			}
		}
	}
	
}

void	CoreBaseApplication::PushArg(const kstl::string& arg)
{
	myArgs.push_back(arg);
}

const kstl::string*	CoreBaseApplication::HasArg(const kstl::string& arg, bool startWith) const
{
	kstl::vector<kstl::string>::const_iterator	itstart = myArgs.begin();
	kstl::vector<kstl::string>::const_iterator	itend = myArgs.end();

	while (itstart != itend)
	{
		if (startWith)
		{
			if ((*itstart).substr(0, arg.size()) == arg)
			{
				return &(*itstart);
			}
		}
		else
		{
			if ((*itstart) == arg)
			{
				return &(*itstart);
			}
		}
		++itstart;
	}

	return nullptr;
}


void	CoreBaseApplication::InitApp(const char* baseDataPath, bool	InitBaseModule)
{
	PlatformBaseApplication::Init();
	CoreModifiable::Init();
	myInitBaseModules = InitBaseModule;

	myBackKeyState = 0;

	if (myInitBaseModules)
	{
		CoreCreateModule(ModuleFileManager, 0);

		if (baseDataPath)
		{
			// init path
			// get file manager singleton
			SP<FilePathManager>	pathManager = KigsCore::GetSingleton("FilePathManager");

			pathManager->AddToPath(baseDataPath, "*");
		}
		
		CoreCreateModule(ModuleTimer, 0);

		myApplicationTimer = KigsCore::GetInstanceOf("ApplicationTimer", "Timer");
		myApplicationTimer->Init();
	}

	ProtectedPreInit();
	ProtectedInit();
	
#ifdef WIN32
#ifndef WUP
	if (HasArg("UseEditor") || HasArg("UseEmbeddedEditor"))
	{

		// search for environment variable 
		char * val;
		val = getenv("KIGS_EDITOR");

		if (val != NULL) {
			kstl::string path = val;
#ifdef _KIGS_ID_RELEASE_
			path += "\\Release\\";
#else
			path += "\\Debug\\";
#endif
			myEditor = new AnonymousModule(path + "\\libIMEditor32.dll", 0);
		}
		else
		{
			myEditor = new AnonymousModule("libIMEditor32.dll", 0);
		}

		
		kstl::vector<CoreModifiableAttribute*> params;	
		maBool* embedded = new maBool(*myEditor, true, LABEL_AND_ID(EmbeddedEditor), HasArg("UseEmbeddedEditor"));
		//maBool* autoStart = new maBool(*myEditor, true, LABEL_AND_ID(AutoStart), HasArg("EditorAutoStart"));

		params.push_back(embedded);
		//params.push_back(autoStart);
		myEditor->Init(KigsCore::Instance(), &params);
		AddAutoUpdate(myEditor);
	}
#endif
#endif
}
void CoreBaseApplication::DoAutoUpdate()
{
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	unsigned int i = 0;
	while (i<myAutoUpdateList.size())
	{
		myAutoUpdateList[i]->CallUpdate((const Timer&)*myApplicationTimer.get(), 0);
		i++;
	}
	myAutoUpdateDone = true;
}

void	CoreBaseApplication::UpdateApp()
{
	// avoid double update entry call
	if(!myAlreadyInUpdate)
	{
		myAlreadyInUpdate=true;

		DECLAREPROFILE(GLOBAL);
		STARTPROFILE(GLOBAL);
		if(myInitBaseModules)
		{
			CoreGetModule(ModuleTimer)->CallUpdate((const Timer&)myApplicationTimer,0);
		}

		// update 'auto update' instances
		// dont't use an iterator as some modifiable update should add new instance to the list (ie :Controller can add NotificationCenter)
		
		myAutoUpdateDone = false;
		
		// key state is set to 2 is set only during one update
		if (PlatformBaseApplication::CheckBackKeyPressed())
		{
			myBackKeyState = 1;
		}
		else if (myBackKeyState == 1)
		{
			myBackKeyState = 2;
		}

		// before real app update, manage async requests
		KigsCore::ManageAsyncRequests();
		PlatformBaseApplication::Update();
		CallUpdate((const Timer&)*(myApplicationTimer.get()),0);
		ProtectedUpdate();
		if (!myAutoUpdateDone)
			DoAutoUpdate();

		if (myBackKeyState == 2)
		{
			myBackKeyState = 0;
		}

		KIGS_DUMP_MESSAGES;

		// if needed, call sleep in timer
		if(myUpdateSleepTime)
		{
			myApplicationTimer->Sleep(myUpdateSleepTime);
		}
		// kill autoreleased
		CoreAutoRelease* autorelease=KigsCore::Instance()->getCoreAutoRelease();
		if(autorelease)
		{
			autorelease->doAutoRelease();
		}
		KigsCore::Instance()->ManagePostDestruction();
		ENDPROFILE(GLOBAL)
		myAlreadyInUpdate=false;
	}
}

void	CoreBaseApplication::CloseApp()
{
	CLOSEPROFILER;
	
	ProtectedClose();
	if(myInitBaseModules)
	{
		// destroy timer
		myApplicationTimer = nullptr;

		CoreDestroyModule(ModuleTimer);
		CoreDestroyModule(ModuleFileManager);
	}

	if (myEditor) {
		myEditor->Close();
		myEditor->Destroy();
	}
#ifdef PRINT_REFTRACING
	CoreModifiable::debugPrintfFullTree();
#endif
	PlatformBaseApplication::Close();
}

void	CoreBaseApplication::Sleep()
{
	PlatformBaseApplication::Sleep();
	ProtectedSleep();
}

void	CoreBaseApplication::Resume()
{
	ProtectedResume();
	PlatformBaseApplication::Resume();
}


void	CoreBaseApplication::Message(int mtype ,int Params )
{
	PlatformBaseApplication::Message(mtype,Params);
	ProtectedMessage(mtype,Params);
}