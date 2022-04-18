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
}

CoreBaseApplication::~CoreBaseApplication()
{
	
}

void	CoreBaseApplication::PrivateAddAutoUpdate(CoreModifiable* toUpdate, double frequency)
{
	// no lock here as it's already locked by calling DoAutoUpdate

	for (const auto& el : mAutoUpdateList)
	{
		if (std::get<0>(el) == toUpdate)
		{
			// already in the list, do nothing
			return;
		}
	}
	toUpdate->flagAsAutoUpdateRegistered();
	mAutoUpdateList.push_back(std::make_tuple(toUpdate, frequency, -1.0));
}

void	CoreBaseApplication::PrivateRemoveAutoUpdate(CoreModifiable* toUpdate)
{
	// no lock here as it's already locked by calling DoAutoUpdate
		// search in list
	if (!mAutoUpdateList.empty())
	{
		std::vector<std::tuple<CoreModifiable*, double, double>>::iterator itAutoUpdate;
		for (itAutoUpdate = mAutoUpdateList.begin(); itAutoUpdate != mAutoUpdateList.end(); ++itAutoUpdate)
		{
			if (std::get<0>(*itAutoUpdate) == toUpdate)
			{
				mAutoUpdateList.erase(itAutoUpdate);
				// no duplicate here, so can exit directly
				return;
			}
		}
	}
}

void			CoreBaseApplication::ManageDelayedAutoUpdateModification()
{
	for (const auto& el : mAutoUpdateToAddRemoveList)
	{
		if (std::get<2>(el) == 1) // add
		{
			PrivateAddAutoUpdate(std::get<0>(el), std::get<1>(el));
		}
		else // remove
		{
			PrivateRemoveAutoUpdate(std::get<0>(el));
		}
	}
	mAutoUpdateToAddRemoveList.clear();
}

void CoreBaseApplication::AddAutoUpdate(CoreModifiable*	toUpdate, double frequency)
{
	toUpdate->flagAsAutoUpdateRegistered();
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	mAutoUpdateToAddRemoveList.push_back(std::make_tuple(toUpdate, frequency, 1)); // add
}

void CoreBaseApplication::RemoveAutoUpdate(CoreModifiable*	toUpdate)
{
	toUpdate->unflagAsAutoUpdateRegistered();
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	mAutoUpdateToAddRemoveList.push_back(std::make_tuple(toUpdate, -1.0, 0)); // remove
}

void	CoreBaseApplication::ChangeAutoUpdateFrequency(CoreModifiable* toUpdate, double frequency)
{
	std::lock_guard<std::recursive_mutex> lk{ mAutoUpdateMutex };
	for (auto& el : mAutoUpdateList)
	{
		if (std::get<0>(el) == toUpdate)
		{
			std::get<1>(el) = frequency;
			std::get<2>(el) = -1.0;
			return;
		}
	}
	// not found in autoUpdateList, search in mAutoUpdateToAddRemoveList
	for (auto& el : mAutoUpdateToAddRemoveList)
	{
		if (std::get<0>(el) == toUpdate)
		{
			std::get<1>(el) = frequency;
			return;
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
			SP<FilePathManager> pathManager = KigsCore::Singleton<FilePathManager>();

			pathManager->AddToPath(baseDataPath, "*");
		}
		
		CoreCreateModule(ModuleTimer, 0);

		mApplicationTimer = KigsCore::GetInstanceOf("ApplicationTimer", "Timer");
		mApplicationTimer->Init();
	}

	ProtectedPreInit();
	ProtectedInit();
}
void CoreBaseApplication::DoAutoUpdate()
{
	std::lock_guard<std::recursive_mutex> lk{mAutoUpdateMutex};
	ManageDelayedAutoUpdateModification();
	// copy list so that if an autoupdated object is removed during loop, the loop can work correctly
	

	const Timer& appTimer = (const Timer&)*mApplicationTimer.get();
	double currentTime = appTimer.GetTime();
	for(auto& el: mAutoUpdateList)
	{
		u32	callCount = 1;
		if (std::get<1>(el) > 0.0) // has frequency
		{
			if (std::get<2>(el) <= 0.0) // first time
			{
				callCount = 1;
				std::get<2>(el) = currentTime;
			}
			else
			{
				double dt = currentTime - std::get<2>(el); // elapsed time since last call
				callCount = (u32)(dt * std::get<1>(el));
				std::get<2>(el) += ((double)callCount) / std::get<1>(el);
				// for now, if need update, the call it only once
				if(callCount)
					callCount = 1;
			}
			
		}
		for (u32 i = 0; i < callCount; i++)
		{
			// when callCount>1 we should call each update at a different time but we suppose
			// callCount will be 0 or 1 most of the time here
			std::get<0>(el)->CallUpdate(appTimer, 0);
		}
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