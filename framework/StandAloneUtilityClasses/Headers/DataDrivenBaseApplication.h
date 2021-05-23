#ifndef _DATADRIVENBASEAPPLICATION_H_
#define _DATADRIVENBASEAPPLICATION_H_

#include "CoreBaseApplication.h"
#include "maCoreItem.h"
#include "AttributePacking.h"
#include "maReference.h"

typedef	kstl::string	State_t;
const State_t	State_None = "";
const State_t	State_Empty = "Empty";
#define MAX_STATE_STACK	10

enum SkipTransitionFlag
{
	SkipTransition_NoSkip = 0,
	SkipTransition_Prev = 1,
	SkipTransition_Next = 2,
	SkipTransition_Both = SkipTransition_Prev | SkipTransition_Next,
};

struct LayerMouseInfo;
class DataDrivenSequenceManager;

// ****************************************
// * DataDrivenSequence class
// * --------------------------------------
/**
 * \class	DataDrivenSequence
 * \file	DataDrivenBaseApplication.h
 * \ingroup Core
 * \brief	Data driven application sequence management.
 */
 // ****************************************
class DataDrivenSequence : public CoreModifiable
{
public:
	friend class DataDrivenSequenceManager;
	friend class DataDrivenTransition;

	DECLARE_CLASS_INFO(DataDrivenSequence, CoreModifiable, Core)
	DataDrivenSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~DataDrivenSequence();

	DataDrivenSequenceManager*	getManager();

protected:
	void	InitModifiable() override;
	void	UninitModifiable() override;

	virtual void	saveParams(kstl::map<unsigned int, kstl::string>& params);
	virtual void	restoreParams(const kstl::map<unsigned int, kstl::string>& params);

	maBool			mKeepParamsOnStateChange;
	maReference		mSequenceManager;
};

// ****************************************
// * DataDrivenTransition class
// * --------------------------------------
/**
 * \class	DataDrivenTransition
 * \file	DataDrivenBaseApplication.h
 * \ingroup Core
 * \brief	Data driven application transition between sequences.
 */
 // ****************************************

class DataDrivenTransition : public CoreModifiable
{
public:

	DECLARE_CLASS_INFO(DataDrivenTransition, CoreModifiable, Core)

	DataDrivenTransition(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	//! destructor
	virtual ~DataDrivenTransition();
	DataDrivenSequenceManager*	getManager();

protected:

	void setInTransition(bool intransition);

	void InitModifiable() override;

	void Update(const Timer&  timer, void* addParam) override;

	CMSP	mPreviousSequence = nullptr;
	maCoreItem		mPreviousAnim;
	maCoreItem		mNextAnim;

	bool			mIsFirstUpdate;

	kstl::vector<CMSP>	mPrevLauncherList;
	kstl::vector<CMSP>	mNextLauncherList;
	maReference						mSequenceManager;
};

// ****************************************
// * DataDrivenSequenceManager class
// * --------------------------------------
/**
 * \class	DataDrivenSequenceManager
 * \file	DataDrivenBaseApplication.h
 * \ingroup Core
 * \brief	Data driven application sequence / transition management.
 */
 // ****************************************

class DataDrivenSequenceManager : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(DataDrivenSequenceManager, CoreModifiable, Core);
	DECLARE_CONSTRUCTOR(DataDrivenSequenceManager);

	//Called when you want to change the state
	void	RequestStateChange(State_t NewState);

	void RequestBackToPreviousState();

	inline State_t GetState() { return mStateStack.back(); }
	inline CMSP	GetCurrentSequence() { return mCurrentSequence; }
	
	friend class DataDrivenTransition;
	friend class DataDrivenSequence;

	virtual bool AllowRender() { return true; }
	virtual SkipTransitionFlag SkipTransitionMode() { return SkipTransitions; }
	virtual void ReloadState();

	SkipTransitionFlag SkipTransitions = SkipTransition_NoSkip;

	bool IsInTransition() { return mInTransition; }

protected:
	static bool	IsParent(CoreModifiable* toCheck, CoreModifiable* p);
	static CoreModifiable*	getParentSequence(CoreModifiable* s);

	CoreModifiable* mSceneGraph;

	void InitModifiable() override;
	void Update(const Timer&, void*) override;
	void UninitModifiable() override;

	void ProtectedInitSequence(const kstl::string& sequence);
	void ProtectedCloseSequence(const kstl::string& sequence);
	
	void setInTransition(bool intransition) { mInTransition = intransition; }
	void CheckEndTransition();

	// to be overloaded if needed
	virtual void SetState(State_t NewState);


	// sequence info & methods
	State_t			mRequestedState;

	kstl::vector<State_t>		mStateStack;

	DECLARE_VIRTUAL_METHOD(ChangeSequence);
	DECLARE_VIRTUAL_METHOD(StackSequence);
	DECLARE_VIRTUAL_METHOD(BackSequence);

	COREMODIFIABLE_METHODS(ChangeSequence, StackSequence, BackSequence);


	void WrapChangeSequence(CoreModifiable*, usString);
	void WrapStackSequence(CoreModifiable*, usString);

	void SetSequence(CoreModifiable*, const std::string&);
	void PushSequence(CoreModifiable*, const std::string&);

	WRAP_METHODS(WrapChangeSequence, WrapStackSequence, SetSequence, PushSequence, RequestStateChange);

	maString			mStartingSequence = BASE_ATTRIBUTE(StartingSequence, "");



	CMSP		mCurrentSequence = nullptr;
	bool				mInTransition;
	
	//LayerMouseInfo * theMouseInfo = nullptr;

	kstl::map<kstl::string, kstl::map<unsigned int, kstl::string> >	mSequenceParametersMap;

};

extern double GlobalAppDT;


// ****************************************
// * DataDrivenBaseApplication class
// * --------------------------------------
/**
 * \class	DataDrivenBaseApplication
 * \file	DataDrivenBaseApplication.h
 * \ingroup Core
 * \brief	Data driven application base class.
 */
 // ****************************************
class DataDrivenBaseApplication : public	CoreBaseApplication
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(DataDrivenBaseApplication, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(DataDrivenBaseApplication);

	inline SP<DataDrivenSequenceManager> GetSequenceManager() { return mSequenceManager; }

	friend class DataDrivenSequenceManager;
	friend class DataDrivenTransition;
protected:



	void	setInTransition(DataDrivenTransition* transition, bool active);


	virtual void ProtectedInitSequence(const kstl::string& sequence) {};
	virtual void ProtectedCloseSequence(const kstl::string& sequence) {};

	// Create default modules
	void ProtectedPreInit() override;
	// Load GlobalConfig, AppInit, create window and start first sequence
	void ProtectedInit() override;
	void ProtectedUpdate() override;
	void ProtectedClose() override;

	virtual void CreateSequenceManager();


	DECLARE_VIRTUAL_METHOD(Exit);
	COREMODIFIABLE_METHODS(Exit);
	// base data driven app modules and classes
	CoreModifiable*		mGUI;
	CoreModifiable*		mRenderer;
	CoreModifiable*		mSceneGraph;
	CoreModifiable*		mInputModule;
	CoreModifiable*		m2DLayers;
	CoreModifiable*		mLuaModule;
	CoreModifiable*		mRenderingScreen;

#ifdef KIGS_TOOLS
	bool mCanUpdateNextFrame = true;
#endif

	float				mApplicationScaling = 1.0f;
	bool				mPreviousShortcutEnabled;

	// anonymous class used to store global parameters 
	CMSP					mGlobalConfig;

	SP<DataDrivenSequenceManager>	mSequenceManager;

	// manage transition state for all sequence manager
	kigs::unordered_map<DataDrivenTransition*, DataDrivenTransition*>						mInTransition;
};
#endif //_DATADRIVENBASEAPPLICATION_H_
