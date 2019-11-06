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

// data driven utils classes (sequences and transitions)
class DataDrivenSequence : public CoreModifiable
{
public:
	friend class DataDrivenSequenceManager;
	friend class DataDrivenTransition;

	DECLARE_CLASS_INFO(DataDrivenSequence, CoreModifiable, Core)

	DataDrivenSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	DataDrivenSequenceManager*	getManager();


protected:
	void	InitModifiable() override;
	void	UninitModifiable() override;

	virtual void	saveParams(kstl::map<unsigned int, kstl::string>& params);
	virtual void	restoreParams(const kstl::map<unsigned int, kstl::string>& params);

	maBool			myKeepParamsOnStateChange;
	maReference		myManager;


	virtual ~DataDrivenSequence();
};

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

	CoreModifiable*	myPreviousSequence = nullptr;
	maCoreItem		myAnimPrev;
	maCoreItem		myAnimNext;

	bool			myIsFirstUpdate;

	kstl::vector<CoreModifiable*>	myPrevLauncherList;
	kstl::vector<CoreModifiable*>	myNextLauncherList;
	maReference						myManager;
};

// external sequence manager

class DataDrivenSequenceManager : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(DataDrivenSequenceManager, CoreModifiable, Core);
	DECLARE_CONSTRUCTOR(DataDrivenSequenceManager);

	//Called when you want to change the state
	void	RequestStateChange(State_t NewState);

	WRAP_METHOD(RequestStateChange);

	void RequestBackToPreviousState();

	inline State_t			GetState() { return m_StateStack.back(); }
	inline CoreModifiable*	GetCurrentSequence() { return myCurrentSequence; }
	//inline CoreModifiable*	GetPreviousSequence() { return myPreviousSequence; }
	
	friend class DataDrivenTransition;
	friend class DataDrivenSequence;

	virtual bool AllowRender() { return true; }
	virtual SkipTransitionFlag SkipTransitionMode() { return SkipTransitions; }
	virtual void ReloadState();

	SkipTransitionFlag SkipTransitions = SkipTransition_NoSkip;

	bool IsInTransition() { return myInTransition; }

protected:
	static bool	IsParent(CoreModifiable* toCheck, CoreModifiable* p);
	static CoreModifiable*	getParentSequence(CoreModifiable* s);

	CoreModifiable*		mySceneGraph;

	void InitModifiable() override;
	void Update(const Timer&, void*) override;
	void UninitModifiable() override;

	void ProtectedInitSequence(const kstl::string& sequence);
	void ProtectedCloseSequence(const kstl::string& sequence);
	
	void setInTransition(bool intransition) { myInTransition = intransition; }
	void CheckEndTransition();

	// to be overloaded if needed
	virtual void SetState(State_t NewState);


	// sequence info & methods
	State_t			m_RequestedState;

	kstl::vector<State_t>		m_StateStack;

	DECLARE_VIRTUAL_METHOD(ChangeSequence);
	DECLARE_VIRTUAL_METHOD(StackSequence);
	DECLARE_VIRTUAL_METHOD(BackSequence);

	void WrapChangeSequence(CoreModifiable*, usString);
	void WrapStackSequence(CoreModifiable*, usString);

	void SetSequence(CoreModifiable*, const std::string&);
	void PushSequence(CoreModifiable*, const std::string&);

	WRAP_METHODS(WrapChangeSequence, WrapStackSequence, SetSequence, PushSequence);

	maString			myStartingSequence = BASE_ATTRIBUTE(StartingSequence, "");


	//CoreModifiable*		myPreviousSequence;
	CoreModifiable*		myCurrentSequence = nullptr;
	bool				myInTransition;
	
	//LayerMouseInfo * theMouseInfo = nullptr;

	kstl::map<kstl::string, kstl::map<unsigned int, kstl::string> >	mySequenceParametersMap;

	virtual ~DataDrivenSequenceManager();

};

extern double GlobalAppDT;

//! base class for data driven applications
// inherit CoreBaseApplication
class DataDrivenBaseApplication : public	CoreBaseApplication
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(DataDrivenBaseApplication, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(DataDrivenBaseApplication);

	inline CoreModifiable*	GetExitConfirmationPopup() { return myConfirmExitPopup; }
	inline DataDrivenSequenceManager*	GetSequenceManager() { return m_SequenceManager; }

	friend class DataDrivenSequenceManager;
	friend class DataDrivenTransition;
protected:
	//! destructor
	virtual ~DataDrivenBaseApplication();


	void	setInTransition(DataDrivenTransition* transition, bool active);

	void removeConfirmationPopup(CoreModifiable* sequence);
	void addConfirmationPopup(CoreModifiable* sequence);

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

	// base data driven app modules and classes
	CoreModifiable*		myGUI;
	CoreModifiable*		myRenderer;
	CoreModifiable*		mySceneGraph;
	CoreModifiable*		myInputModule;
	CoreModifiable*		my2DLayers;
	CoreModifiable*		myLuaModule;
	CoreModifiable*		myRenderingScreen;

#ifdef KIGS_TOOLS
	bool mCanUpdateNextFrame = true;
#endif
	CoreModifiable*		myConfirmExitPopup;


	bool				myPreviousShortcutEnabled;

	// anonymous class used to store global parameters 
	CoreModifiable*		m_GlobalConfig;

	DataDrivenSequenceManager*	m_SequenceManager;

	// manage transition state for all sequence manager
	kstl::unordered_map<DataDrivenTransition*, DataDrivenTransition*>						myInTransition;
};
#endif //_DATADRIVENBASEAPPLICATION_H_
