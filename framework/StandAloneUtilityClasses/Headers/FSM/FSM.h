#pragma once

#include "CoreModifiable.h"
#include "FSM/FSM_EventQueue.h"
#include "FSM/FSM_StateStruct.h"
#include "CoreModifiableAttribute.h"
#include "maCoreItem.h"
#include "AttributePacking.h"

// to be called by project using FSM

extern void	Init_FSM_Management();



// manager class used as a singleton to update all FSM 

class FSM;

class FSM_Manager : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(FSM_Manager, CoreModifiable, FSM)
	DECLARE_INLINE_CONSTRUCTOR(FSM_Manager) {}
	virtual ~FSM_Manager();

	void	registerFSM(FSM* newfsm);
	void	unregisterFSM(FSM* newfsm);

	virtual void Update(const Timer&  timer, void* addParam);

protected:
	virtual void	InitModifiable();

	kstl::vector<FSM*>	m_FSM_List;
};

class FSM : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(FSM, CoreModifiable, FSM);
	DECLARE_CONSTRUCTOR(FSM);
	virtual ~FSM();

	WRAP_METHODS(sendEventName, sendEventNameWithData)

	void	updateFSM();

	void	beginStateMap();
	void	beginStateDescription(const kstl::string& stateName);
	// add a state instance 
	void	addStateInstance(FSM_State* state);
	// add an action by name 
	void	addStateAction(STATE_ACTION_TYPE type, const kstl::string& actionName);
	
	void	addTransition(const kstl::string& notificationName, const kstl::string& transition, const kstl::string& guardMethod="");

	void	endStateDescription();
	void	endStateMap();

	KigsID	getCurrentStateID()
	{
		if (m_CurrentState)
		{
			return m_CurrentState->getStateID();
		}
		return 0u;
	}

	void initFromDescription(CoreItemSP description);

	void sendEventName(kstl::string notifname)
	{
		sendEvent(notifname);
	}
	

	void sendEventNameWithData(kstl::string notifname,void* eventdata)
	{
		sendEvent(notifname, eventdata);
	}
	

	void sendEvent(KigsID notifID, void* eventdata=0);

	void initStateFromJSONFile(const kstl::string& filename);

	kstl::unordered_map<KigsID, FSM_StateStruct*>	m_StateMap;

protected:

	virtual void	InitModifiable();

	void	eraseStateMap();
	void	initStateFromDescription(CoreItemSP statedescription);
	void	initTransitionsFromDescription(CoreItemSP statedescription);
	void	initFSM_StateFromDescription(CoreItemSP statedescription);

	FSM_EventQueue	m_EventQueue;

	
	FSM_StateStruct*							m_CurrentState;


	enum  FSM_INIT_STAGE_FLAG
	{
		STAGE_STATE_MAP		= 1,
		STAGE_STATE			= 2,
		STAGE_INIT_DONE		= 4,
	};
	unsigned int								m_InitStageFlag;

	bool	isInStage(FSM_INIT_STAGE_FLAG totest)
	{
		return ((m_InitStageFlag&(unsigned int)totest) != 0);
	}

	FSM*	findFSMForStateID(unsigned int stateID)
	{
		auto statefound = m_StateMap.find(stateID);
		if (statefound != m_StateMap.end())
		{
			return this;
		}
		
		if (myParentFSM)
		{
			statefound = myParentFSM->m_StateMap.find(stateID);
			if (statefound != myParentFSM->m_StateMap.end())
			{
				return myParentFSM;
			}
		}
		return this;
	}

	friend class FSM_StateStruct;
	void	initState(FSM_StateStruct* state,void* eventData);
	void	closeState(FSM_StateStruct* state);

	DECLARE_METHOD(EventReceived);

	maString	myInitState;
	maCoreItem	myDescription;
	bool		myIsAutoactivated;
	bool		myIsAlive;

	void setParentFSM(FSM* parentfsm)
	{
		myParentFSM = parentfsm;
	}

	FSM*		myParentFSM;
};

