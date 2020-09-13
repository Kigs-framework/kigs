#include "PrecompiledHeaders.h"
#include "CoreBaseApplication.h"
#include "Core.h"
#include "CoreIncludes.h"
#include "TimerIncludes.h"
#include "ModuleFileManager.h"
#include "TimeProfiler.h"

//#define PRINT_REFTRACING

IMPLEMENT_CLASS_INFO(CoreBaseApplication)

// 
CoreBaseApplication::CoreBaseApplication(const std::string& name, CLASS_NAME_TREE_ARG) :  PlatformBaseApplication(), CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
	mHasFocus         = true;
	mNeedExit         = false;
	mInitBaseModules  = false;
	mAlreadyInUpdate  = false;
	mApplicationTimer = 0;
	mUpdateSleepTime  = 0;
	mBackKeyState     = 0;
	KigsCore::SetCoreApplication(this);
	mAutoUpdateList.clear();
	mEditor		   = NULL;

	
}

CoreBaseApplication::~CoreBaseApplication()
{
	
}

void CoreBaseApplication::AddAutoUpdate(CoreModifiable*	toUpdate)
{
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	// check if not already there
	std::vector<CoreModifiable*>::const_iterator itAutoUpdate;
	for(itAutoUpdate = mAutoUpdateList.begin();itAutoUpdate != mAutoUpdateList.end();++itAutoUpdate)
	{
		if((*itAutoUpdate) == toUpdate)
		{
			// already in the list, do nothing
			return;
		}
	}
	toUpdate->flagAsAutoUpdateRegistered();
	mAutoUpdateList.push_back(toUpdate);

}

void CoreBaseApplication::RemoveAutoUpdate(CoreModifiable*	toUpdate)
{
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	// search in list
	std::vector<CoreModifiable*>::iterator itAutoUpdate;
	if(!mAutoUpdateList.empty())
	{
		for(itAutoUpdate = mAutoUpdateList.begin();itAutoUpdate != mAutoUpdateList.end();++itAutoUpdate)
		{
			if((*itAutoUpdate) == toUpdate)
			{
				mAutoUpdateList.erase(itAutoUpdate);
				toUpdate->unflagAsAutoUpdateRegistered();
				// no duplicate here, so can exit directly
				return;
			}
		}
	}
	
}

void	CoreBaseApplication::PushArg(const std::string& arg)
{
	mArgs.push_back(arg);
}

const std::string*	CoreBaseApplication::HasArg(const std::string& arg, bool startWith) const
{
	std::vector<std::string>::const_iterator	itstart = mArgs.begin();
	std::vector<std::string>::const_iterator	itend = mArgs.end();

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
	mInitBaseModules = InitBaseModule;

	mBackKeyState = 0;

	if (mInitBaseModules)
	{
		CoreCreateModule(ModuleFileManager, 0);

		if (baseDataPath)
		{
			// init path
			// get file manager singleton
			SP<FilePathManager>&	pathManager = KigsCore::Singleton<FilePathManager>();

			pathManager->AddToPath(baseDataPath, "*");
		}
		
		CoreCreateModule(ModuleTimer, 0);

		mApplicationTimer = KigsCore::GetInstanceOf("ApplicationTimer", "Timer");
		mApplicationTimer->Init();
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
			std::string path = val;
#ifdef _KIGS_ID_RELEASE_
			path += "\\Release\\";
#else
			path += "\\Debug\\";
#endif
			mEditor = new AnonymousModule(path + "\\libIMEditor32.dll", 0);
		}
		else
		{
			mEditor = new AnonymousModule("libIMEditor32.dll", 0);
		}

		
		std::vector<CoreModifiableAttribute*> params;	
		maBool* embedded = new maBool(*mEditor, true, LABEL_AND_ID(EmbeddedEditor), HasArg("UseEmbeddedEditor"));
		//maBool* autoStart = new maBool(*mEditor, true, LABEL_AND_ID(AutoStart), HasArg("EditorAutoStart"));

		params.push_back(embedded);
		//params.push_back(autoStart);
		mEditor->Init(KigsCore::Instance(), &params);
		AddAutoUpdate(mEditor);
	}
#endif
#endif
}
void CoreBaseApplication::DoAutoUpdate()
{
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	unsigned int i = 0;
	while (i<mAutoUpdateList.size())
	{
		mAutoUpdateList[i]->CallUpdate((const Timer&)*mApplicationTimer.get(), 0);
		i++;
	}
	mAutoUpdateDone = true;
}

void	CoreBaseApplication::UpdateApp()
{
	// avoid double update mEntry call
	if(!mAlreadyInUpdate)
	{
		mAlreadyInUpdate=true;

		DECLAREPROFILE(GLOBAL);
		STARTPROFILE(GLOBAL);
		if(mInitBaseModules)
		{
			CoreGetModule(ModuleTimer)->CallUpdate((const Timer&)mApplicationTimer,0);
		}

		// update 'auto update' instances
		// dont't use an iterator as some modifiable update should add new instance to the list (ie :Controller can add NotificationCenter)
		
		mAutoUpdateDone = false;
		
		// key state is set to 2 is set only during one update
		if (PlatformBaseApplication::CheckBackKeyPressed())
		{
			mBackKeyState = 1;
		}
		else if (mBackKeyState == 1)
		{
			mBackKeyState = 2;
		}

		// before real app update, manage async requests
		KigsCore::ManageAsyncRequests();
		PlatformBaseApplication::Update();
		CallUpdate((const Timer&)*(mApplicationTimer.get()),0);
		ProtectedUpdate();
		if (!mAutoUpdateDone)
			DoAutoUpdate();

		if (mBackKeyState == 2)
		{
			mBackKeyState = 0;
		}

		KIGS_DUMP_MESSAGES;

		// if needed, call sleep in timer
		if(mUpdateSleepTime)
		{
			mApplicationTimer->Sleep(mUpdateSleepTime);
		}
	
		KigsCore::Instance()->ManagePostDestruction();
		ENDPROFILE(GLOBAL)
		mAlreadyInUpdate=false;
	}
}

void	CoreBaseApplication::CloseApp()
{
	CLOSEPROFILER;
	
	ProtectedClose();
	if(mInitBaseModules)
	{
		// destroy timer
		mApplicationTimer = nullptr;

		CoreDestroyModule(ModuleTimer);
		CoreDestroyModule(ModuleFileManager);
	}

	if (mEditor) {
		mEditor->Close();
		mEditor->Destroy();
	}
#ifdef PRINT_REFTRACING
	CoreModifiable::debugPrintfFullTree();
#endif
	PlatformBaseApplication::Close();

	KigsCore::SetCoreApplication(nullptr);
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