#pragma once

#include "CoreModifiable.h"
#include <queue>

class FSM_Event
{
public:
	FSM_Event()
	{

	}

	FSM_Event(KigsID id, void* edata) : m_ID(id), m_eventData(edata)
	{

	}

	operator KigsID()
	{
		return m_ID;
	}

	void*	getEventData()
	{
		return m_eventData;
	}

protected:
	KigsID	m_ID;
	void*			m_eventData;
};


class FSM_EventQueue
{
public:

	// push new event
	void	push(const FSM_Event& e)
	{
		m_EventList.push(e);
	}

	// retreive older and remove from queue
	FSM_Event	pop()
	{
		FSM_Event e = m_EventList.front();
		m_EventList.pop();

		return e;
	}

	// retreive older 
	FSM_Event	front()
	{
		FSM_Event e = m_EventList.front();
		return e;
	}

	bool	isEmpty()
	{
		return (m_EventList.size() == 0);
	}

protected:
	std::queue<FSM_Event>	m_EventList;
};
