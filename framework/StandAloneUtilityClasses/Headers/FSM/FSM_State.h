#pragma once

#include "FSM/FSM.h"
#include "FSM/FSM_StateStruct.h"


// for sub state, a state is itself a FSM
class FSM_State : public FSM
{
public:

	DECLARE_CLASS_INFO(FSM_State, FSM, FSM)

	FSM_State(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void	InvokeBeginAction(void* statedata = 0);
	void	InvokeAction(void* statedata = 0);
	void	InvokeEndAction(void* statedata = 0);

protected:

	virtual ~FSM_State()
	{

	}

	friend class FSM_StateStruct;

	virtual void	protectedInvokeBeginAction(void* statedata = 0);
	virtual void	protectedInvokeAction(void* statedata = 0);
	virtual void	protectedInvokeEndAction(void* statedata = 0);


};

