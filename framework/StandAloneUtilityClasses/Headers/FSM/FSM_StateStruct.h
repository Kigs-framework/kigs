#pragma once

#include "CoreModifiable.h"
#include "Core.h"

// contains a function ID or a pointer to FSM_State
class FSM_State;
class FSM;

enum  STATE_ACTION_TYPE
{
	Update = 0,
	Begin,
	End
};


class FSM_StateActionPack
{
public:
	FSM_StateActionPack()
	{
		m_actions[0] = m_actions[1] = m_actions[2] = 0u;
	}
	KigsID	m_actions[3];

};

struct TransitionStruct
{
	kstl::string	m_NotificationName;
	KigsID	m_TransitionStateID;
	KigsID	m_GuardMethodID;
};

// manage states : store transition map, state instances or state action methods or coreitem
class FSM_StateStruct
{
protected:

	bool	m_isSubFSM;

public:
	FSM_StateStruct(const kstl::string& statename) : m_State(0), m_isSubFSM(false)
	{
		m_StateID = statename;
	}
	~FSM_StateStruct();

	void	invokeAction(FSM* caller, void* statedata);
	void	invokeBeginAction(FSM* caller, void* statedata);
	void	invokeEndAction(FSM* caller, void* statedata);

	kstl::unordered_map<KigsID, TransitionStruct>& getTransitions()
	{
		return m_Transitions;
	}

	kstl::unordered_map<KigsID, TransitionStruct>& getSonTransitions();


	KigsID	getStateID()
	{
		return m_StateID;
	}

	bool	addInstance(FSM_State* state_instance)
	{
		// instance or action pack already set
		if ((m_State != 0) || m_isSubFSM)
		{
			return false;
		}

		m_State = state_instance;
		m_isSubFSM = true;
		return true;
	}

	bool	addAction(STATE_ACTION_TYPE atype, const kstl::string& methodName)
	{
		// instance or action pack already set
		if (m_isSubFSM)
		{
			return false;
		}

		if (m_Actions == 0)
		{
			m_Actions = new FSM_StateActionPack();
		}

		m_Actions->m_actions[atype]= methodName;
		return true;
	}

	bool isSubFSM()
	{
		return m_isSubFSM;
	}

//protected:
	union
	{
		FSM_State*				m_State;
		FSM_StateActionPack*	m_Actions;
	};

	
	// unsigned int is notificationID
	kstl::unordered_map<KigsID, TransitionStruct>	m_Transitions;

	KigsID								m_StateID;
};

