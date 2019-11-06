#include "FSM/FSM_State.h"
#include "CoreBaseApplication.h"

IMPLEMENT_CLASS_INFO(FSM_State);

FSM_State::FSM_State(const kstl::string& name, CLASS_NAME_TREE_ARG) : FSM(name, PASS_CLASS_NAME_TREE_ARG)
{
	// as a state, I will be activated by my parent 
	myIsAutoactivated = false;
}

void	FSM_State::protectedInvokeAction(void* statedata)
{
	
}

void	FSM_State::protectedInvokeBeginAction(void* statedata)
{

}

void	FSM_State::protectedInvokeEndAction(void* statedata)
{

}


void	FSM_State::InvokeBeginAction(void* statedata)
{
	kstl::string initstate = myInitState;
	if (initstate != "")
	{
		unsigned int stateID = CharToID::GetID(initstate);
		auto foundState = m_StateMap.find(stateID);

		if (foundState != m_StateMap.end())
		{
			initState((*foundState).second, 0);
		}
	}

	myIsAlive = true;
	protectedInvokeBeginAction(statedata);
}
void	FSM_State::InvokeAction(void* statedata)
{
	protectedInvokeAction(statedata);
}
void	FSM_State::InvokeEndAction(void* statedata)
{
	protectedInvokeEndAction(statedata);
	closeState(m_CurrentState);
	myIsAlive = false;
}