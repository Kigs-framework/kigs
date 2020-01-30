#ifndef _COREBASEAPPLICATION_H_
#define _COREBASEAPPLICATION_H_

#include "CoreModifiable.h"
#include "Platform/Core/PlatformBaseApplication.h"
#include "kstlvector.h"
#include "kstlstring.h"
#include "Timer.h"

class	ModuleBase;
class   AnonymousModule;

//! useful macro to create / retreive / destroy modules 
#define	CoreCreateModule(moduleType,params)		KigsCore::LoadKigsModule<moduleType>(#moduleType,params)
#define	CoreGetModule(moduleType)				KigsCore::GetModule(#moduleType)
#define CoreDestroyModule(moduleType)			{ModuleBase* m = KigsCore::GetModule(#moduleType); m->Close(); m->Destroy();}

//! base class for applications
// inherit CoreModifiable so can receive and send notification, have parameters...
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
	* \fn 		CoreBaseApplication(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	CoreBaseApplication(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

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
		return myNeedExit;
	}

	void	ExternAskExit()
	{
		if(!myNeedExit)
		{
			ProtectedExternAskExit();
			myNeedExit=true;
		}
	}
	

	inline void SetFocus(bool aHasFocus) { myHasFocus = aHasFocus; }
	inline bool HasFocus()               { return myHasFocus; }


	void	SetUpdateSleepTime(unsigned int ms)
	{
		myUpdateSleepTime=ms;
	}

	unsigned int	GetUpdateSleepTime()
	{
		return myUpdateSleepTime;
	}

	// args management
	void			PushArg(const kstl::string& arg);

	unsigned int	GetArgCount() const
	{
		return ((unsigned int)myArgs.size());
	}

	const kstl::string*	HasArg(const kstl::string& arg, bool startWith=false) const;

	SP<Timer>&			GetApplicationTimer()
	{
		return myApplicationTimer;
	}

	void			AddAutoUpdate(CoreModifiable*	toUpdate);
	void			RemoveAutoUpdate(CoreModifiable*	toUpdate);

	// to be ckecked only during update
	bool			BackKeyWasPressed()
	{
		return myBackKeyState == 2;
	}

	void	setBuildVersion(float v)
	{
		myBuildVersion = v;
	}
	 
	float getBuildVersion()
	{
		return myBuildVersion;
	}

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
	virtual void	ProtectedExternAskExit(){;}

	void DoAutoUpdate();

	maBool			myNeedExit = BASE_ATTRIBUTE(NeedExit,false);
	bool			myHasFocus;

	volatile bool	myAlreadyInUpdate;

	bool			myInitBaseModules;

	SP<Timer>			myApplicationTimer;

	unsigned int	myUpdateSleepTime;

	kstl::vector<kstl::string>			myArgs;
	kstl::vector<CoreModifiable*>		myAutoUpdateList;

	unsigned int	myBackKeyState;

	AnonymousModule*					myEditor;

	bool myAutoUpdateDone = false;

	std::recursive_mutex mAutoUpdateMutex;

	float		myBuildVersion = 1.0;
};

#endif