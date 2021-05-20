#include "FSM/FSM_StateStruct.h"
#include "FSM/FSM_State.h"
#include "FSM/FSM.h"

bool	FSM_StateStruct::addInstance(FSM_State* state_instance)
{
	// instance or action pack already set
	if ((m_State != 0) || m_isSubFSM)
	{
		return false;
	}

	m_State = state_instance->SharedFromThis();
	m_isSubFSM = true;
	return true;
}

void	FSM_StateStruct::invokeAction(FSM* caller, void* statedata)
{
	if (!m_isSubFSM) // method
	{
		if (m_Actions)
		{
			if (m_Actions->m_actions[Update] != 0u)
			{
				kstl::vector<CoreModifiableAttribute*> emptyparams;
				caller->CallMethod(m_Actions->m_actions[Update], emptyparams, statedata);
			}
		}
	}
	else
	{
		if (m_State)
		{
			// first update state fsm
			m_State->updateFSM();
			m_State->InvokeAction(statedata);
		}
	}
}

void	FSM_StateStruct::invokeBeginAction(FSM* caller, void* statedata)
{
	if (!m_isSubFSM) // method
	{
		if (m_Actions)
		{
			if (m_Actions->m_actions[Begin] != 0u)
			{
				kstl::vector<CoreModifiableAttribute*> emptyparams;
				caller->CallMethod(m_Actions->m_actions[Begin], emptyparams, statedata);
			}
		}
	}
	else
	{
		if (m_State)
		{
			caller->aggregateWith(m_State->SharedFromThis());
			m_State->InvokeBeginAction(statedata);
		}
	}
}
void	FSM_StateStruct::invokeEndAction(FSM* caller, void* statedata)
{
	if (!m_isSubFSM) // method
	{
		if (m_Actions)
		{
			if (m_Actions->m_actions[End] != 0u)
			{
				kstl::vector<CoreModifiableAttribute*> emptyparams;
				caller->CallMethod(m_Actions->m_actions[End], emptyparams, statedata);
			}
		}
	}
	else
	{
		if (m_State)
		{
			m_State->InvokeEndAction(statedata);
			caller->removeAggregateWith(m_State->SharedFromThis());
		}
	}
}

FSM_StateStruct::~FSM_StateStruct()
{
	if (m_State)
	{
		if (m_isSubFSM)
		{
			m_State.reset();
		}
		else
		{
			delete m_Actions;
		}
	}
}

kstl::unordered_map<KigsID, TransitionStruct>& FSM_StateStruct::getSonTransitions()
{
	if (m_isSubFSM)
	{
		return m_State->m_CurrentState->getTransitions();
	}
	return m_Transitions;
}