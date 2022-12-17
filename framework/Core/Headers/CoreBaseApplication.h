#ifndef _COREBASEAPPLICATION_H_
#define _COREBASEAPPLICATION_H_

#include "CoreModifiable.h"
#include "Platform/Core/PlatformBaseApplication.h"
#include <vector>
#include "kstlstring.h"
#include "Timer.h"
#include "MiniInstanceFactory.h"

class	ModuleBase;
class   AnonymousModule;

//! useful macro to create / retreive / destroy modules 
#define	CoreCreateModule(moduleType,params)		KigsCore::LoadKigsModule<moduleType>(#moduleType,params)
#define	CoreGetModule(moduleType)				KigsCore::GetModule(#moduleType)
#define CoreDestroyModule(moduleType)			{SP<ModuleBase> m = KigsCore::GetModule(#moduleType); if(m){ m->Close(); KigsCore::RemoveModule(#moduleType); }}



// ****************************************
// * CoreBaseApplication class
// * --------------------------------------
/**
* \file	CoreBaseApplication.h
* \class	CoreBaseApplication
* \ingroup Core
* \brief	 Base class for applications.
*
* Inherit CoreModifiable so can receive and send notification, have CoreModifiable attributes...
*/
// ****************************************

class CoreBaseApplication : public	PlatformBaseApplication, public CoreModifiable
{
public:
	enum msg{
		RESERVE1,
		RESERVE2,
		SHOP
	};

	DECLARE_ABSTRACT_CLASS_INFO(CoreBaseApplication, CoreModifiable, KigsCore)

	/**
	* \brief	constructor
	* \fn 		CoreBaseApplication(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	CoreBaseApplication(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	//! destructor
	virtual ~CoreBaseApplication();

	void	InitApp(const char* baseDataPath, bool	InitBaseModule);
	void	UpdateApp();
	void	CloseApp();

	void	Sleep();
	void	Resume();
//	using   CoreModifiable::Message;
	void	Message(int /* mtype */,int /* Params */);

	//!	return true if application can exit
	bool			NeedExit()
	{
		return mNeedExit;
	}

	void	ExternAskExit()
	{
		if(!mNeedExit)
		{
			mNeedExit = ProtectedExternAskExit();
		}
	}
	

	inline void SetFocus(bool aHasFocus) { mHasFocus = aHasFocus; }
	inline bool HasFocus()               { return mHasFocus; }


	void	SetUpdateSleepTime(unsigned int ms)
	{
		mUpdateSleepTime=ms;
	}

	unsigned int	GetUpdateSleepTime()
	{
		return mUpdateSleepTime;
	}

	// args management
	void			PushArg(const std::string& arg);

	unsigned int	GetArgCount() const
	{
		return ((unsigned int)mArgs.size());
	}

	const std::string*	HasArg(const std::string& arg, bool startWith=false) const;

	SP<Timer>			GetApplicationTimer()
	{
		return mApplicationTimer;
	}

	void			AddAutoUpdate(CoreModifiable*	toUpdate,double frequency=-1.0);
	void			RemoveAutoUpdate(CoreModifiable* toUpdate);

private:
	void			PrivateAddAutoUpdate(CoreModifiable* toUpdate, double frequency);
	void			PrivateRemoveAutoUpdate(CoreModifiable* toUpdate);
	void			ManageDelayedAutoUpdateModification();
public:
	void			ChangeAutoUpdateFrequency(CoreModifiable* toUpdate, double frequency = -1.0);

	// to be ckecked only during update
	bool			BackKeyWasPressed()
	{
		return mBackKeyState == 2;
	}

	void	setBuildVersion(float v)
	{
		mBuildVersion = v;
	}
	 
	float getBuildVersion()
	{
		return mBuildVersion;
	}
	const std::vector<std::tuple<CoreModifiable*,double,double>> GetAutoUpdateList() { return mAutoUpdateList; }

protected:


	CoreBaseApplication& operator=(const CoreBaseApplication&) = delete;

	virtual void	ProtectedPreInit() {};

	//! this Init is to overload
	virtual void	ProtectedInit()=0;

	//! this update is to overload
	virtual void	ProtectedUpdate()=0;

	//! this close is to overload
	virtual void	ProtectedClose()=0;

	virtual void	ProtectedMessage(int /* mtype */,int /* Params */){}

	//! to overload if needed
	virtual void	ProtectedSleep(){;}
	virtual void	ProtectedResume(){;}
	virtual bool	ProtectedExternAskExit() { return true; }

	void DoAutoUpdate();

	maBool			mNeedExit = BASE_ATTRIBUTE(NeedExit,false);
	bool			mHasFocus;

	volatile bool	mAlreadyInUpdate;

	bool			mInitBaseModules;

	SP<Timer>			mApplicationTimer;

	unsigned int	mUpdateSleepTime;

	std::vector<std::string>			mArgs;
	// list of auto updated elements + update frequency and last update time
	std::vector<std::tuple<CoreModifiable*,double,double>>		mAutoUpdateList;
	std::vector<std::tuple<CoreModifiable*, double,int>>		mAutoUpdateToAddRemoveList;

	unsigned int	mBackKeyState;

	bool mAutoUpdateDone = false;

	std::recursive_mutex mAutoUpdateMutex;

	float		mBuildVersion = 1.0;
};

#endif