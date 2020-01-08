#include "FSM/FSM.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "CoreModifiableAttribute.h"
#include "CoreBaseApplication.h"
#include "FSM/FSM_State.h"
#include "CoreItem.h"
#include "CoreMap.h"
#include "JSonFileParser.h"

void	Init_FSM_Management()
{
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), FSM, FSM, FSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), FSM_Manager, FSM_Manager, FSM);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), FSM_State, FSM_State, FSM);

	FSM_Manager* manager = (FSM_Manager*)KigsCore::GetSingleton("FSM_Manager");
	if (manager)
	{
		manager->Init();
	}
}


IMPLEMENT_CLASS_INFO(FSM);
IMPLEMENT_CLASS_INFO(FSM_Manager);


IMPLEMENT_CONSTRUCTOR(FSM)
, m_CurrentState(0)
, m_InitStageFlag(0)
, myInitState(*this, true,"InitState","")
, myDescription(*this, true, "Description", "")
, myIsAutoactivated(true)
, myIsAlive(false)
, myParentFSM(0)
{
	
}

void	FSM::InitModifiable()
{
	if (!IsInit())
	{
		CoreModifiable::InitModifiable();

		if (IsInit())
		{
			// init with description

			CoreItemSP ItemToEval = CoreItemSP(myDescription, GetRefTag{});
			
			if(!ItemToEval.isNil())
				initFromDescription(ItemToEval);

			kstl::string initstate = myInitState;
			if (initstate != "")
			{
				KigsID stateID = initstate;
				auto foundState = m_StateMap.find(stateID);

				if (myIsAutoactivated)
				{
					initState((*foundState).second, 0);

					// register FSM
					FSM_Manager* manager = (FSM_Manager*)KigsCore::GetSingleton("FSM_Manager");
					if (manager)
					{
						manager->registerFSM(this);
					}

					myIsAlive = true;
				}
			}
		}
	}
}

void	FSM::updateFSM()
{
	if ((m_CurrentState == 0) || (!myIsAlive)) // no state => not init
	{
		return;
	}
	bool currentStateActionWasCalled = false;

	while (!m_EventQueue.isEmpty())
	{
		FSM_Event	currentEvent = m_EventQueue.front();

	
		auto transitions = &m_CurrentState->getTransitions();
		auto foundTransition = transitions->find(currentEvent);

		// get transition of son FSM
		if (m_CurrentState->isSubFSM() && (foundTransition == transitions->end()))
		{
			transitions=&m_CurrentState->getSonTransitions();
			foundTransition = transitions->find(currentEvent);
		}

		// check if a transition is possible
		
		FSM_StateStruct* newState = 0;
		if (foundTransition == transitions->end())
		{
			KIGS_WARNING("transition not found", 1);
			m_EventQueue.pop();
			continue;
		}

		// get TransitionStruct
		TransitionStruct& currentTransition= (*foundTransition).second;
		
		// check guard
		if((currentTransition.m_GuardMethodID) != 0u) 
		{
			if (SimpleCall(currentTransition.m_GuardMethodID))  // if guard method returns true, then don't change state
			{
				m_EventQueue.pop();
				continue;
			}
		}
		
		auto foundState = m_StateMap.find(currentTransition.m_TransitionStateID);

		if(foundState == m_StateMap.end())
		{
			KIGS_WARNING("state not found", 1);
			m_EventQueue.pop();
			continue;
		}
		newState = (*foundState).second;
	
		if( newState != m_CurrentState)
		{
			m_EventQueue.pop();

			if (!currentStateActionWasCalled) // if current state update was not called, call it before state change
			{
				m_CurrentState->invokeAction(this, 0);
				currentStateActionWasCalled = true;
			}
			closeState(m_CurrentState);
			initState(newState, currentEvent.getEventData());
	

			break;	// if state change, exit event loop
		}
		else
		{
			if (!currentStateActionWasCalled) // event call current action : call it with event data
			{
				m_EventQueue.pop();
				m_CurrentState->invokeAction(this, currentEvent.getEventData());
				currentStateActionWasCalled = true;
			}
			else
			{
				break; // current state action already called in this loop, break and don't pop event
			}
		}
	}

	if (!currentStateActionWasCalled)
	{
		m_CurrentState->invokeAction(this, 0); // call current state if not done
	}
}

FSM::~FSM()
{
	if (myIsAutoactivated)
	{
		// unregister FSM
		FSM_Manager* manager = (FSM_Manager*)KigsCore::GetSingleton("FSM_Manager");
		if (manager)
		{
			manager->unregisterFSM(this);
		}

		myIsAlive = false;
	}

	eraseStateMap();
}


void FSM::eraseStateMap()
{
	for (auto& p : m_StateMap)
	{
		delete p.second;
	}
	m_StateMap.clear();
}

void	FSM::beginStateMap()
{
	if ((m_StateMap.size()!=0) || (m_InitStageFlag !=0))
	{
		KIGS_WARNING("begin state map on already init FSM", 1);
		eraseStateMap();
	}
	/*		
	STAGE_STATE_MAP = 1,
	STAGE_STATE = 2,
	STAGE_TRANSITION = 4,
	STAGE_INIT_DONE = 8,*/
	m_InitStageFlag |= (unsigned int)STAGE_STATE_MAP;
						
}
void	FSM::beginStateDescription(const kstl::string& stateName)
{
	if (isInStage(STAGE_STATE_MAP) && !isInStage(STAGE_STATE))
	{
		m_InitStageFlag |= (unsigned int)STAGE_STATE;
		m_CurrentState = new FSM_StateStruct(stateName);
	}
	else
	{
		KIGS_ERROR("Bad FSM initialisation", 1);
	}
}
// add a state instance 
void	FSM::addStateInstance(FSM_State* state)
{
	if (isInStage(STAGE_STATE_MAP) && isInStage(STAGE_STATE))
	{
		if (!m_CurrentState->addInstance(state))
		{
			KIGS_ERROR("Bad FSM initialisation", 1);
		}

		state->setParentFSM(this);
	}
	else
	{
		KIGS_ERROR("Bad FSM initialisation", 1);
	}
}

// add a maMethod action 
void	FSM::addStateAction(STATE_ACTION_TYPE type, const kstl::string& actionName)
{
	if (isInStage(STAGE_STATE_MAP) && isInStage(STAGE_STATE))
	{
		if (!m_CurrentState->addAction(type, actionName))
		{
			KIGS_ERROR("Bad FSM initialisation", 1);
		}
	}
	else
	{
		KIGS_ERROR("Bad FSM initialisation", 1);
	}
}


void	FSM::addTransition(const kstl::string& notificationName, const kstl::string& toStatetransition, const kstl::string& guardMethod)
{
	if (isInStage(STAGE_STATE_MAP) && isInStage(STAGE_STATE) )
	{
		unsigned int notifID= CharToID::GetID(notificationName);
		auto& transitions = m_CurrentState->getTransitions();
		TransitionStruct& currentTransition = transitions[notifID];
		
		currentTransition.m_NotificationName = notificationName;
		currentTransition.m_GuardMethodID = 0u;
		if (guardMethod != "")
		{
			currentTransition.m_GuardMethodID = CharToID::GetID(guardMethod);
		}
		currentTransition.m_TransitionStateID= CharToID::GetID(toStatetransition);
	}
	else
	{
		KIGS_ERROR("Bad FSM initialisation", 1);
	}
}

void	FSM::endStateDescription()
{
	if (isInStage(STAGE_STATE_MAP) && isInStage(STAGE_STATE))
	{
		m_InitStageFlag ^= (unsigned int)STAGE_STATE;

		// check if state is not already there
		if (m_StateMap.find(m_CurrentState->getStateID()) == m_StateMap.end())
		{
			m_StateMap[m_CurrentState->getStateID()] = m_CurrentState;
			m_CurrentState = 0;
		}
		else
		{
			KIGS_ERROR("Bad FSM initialisation", 1);
		}
	}
	else
	{
		KIGS_ERROR("Bad FSM initialisation", 1);
	}
}
void	FSM::endStateMap()
{
	if (m_InitStageFlag == (unsigned int)STAGE_STATE_MAP) // only stage map remaining
	{
		m_InitStageFlag ^= (unsigned int)STAGE_STATE_MAP;
		m_InitStageFlag|= (unsigned int)STAGE_INIT_DONE;
	}
	else
	{
		KIGS_ERROR("Bad FSM initialisation", 1);
	}
}

void	FSM::initState(FSM_StateStruct* state, void* eventData)
{
	// call begin action
	state->invokeBeginAction(this, eventData);

	// add notifications
	auto& transitions = state->getTransitions();
	for(auto& itc : transitions)
	{
		TransitionStruct& currentTransition = itc.second;
		KigsCore::GetNotificationCenter()->addObserver(this, "EventReceived", currentTransition.m_NotificationName);
	}
	// current state is the newstate now
	m_CurrentState = state;


}
void	FSM::closeState(FSM_StateStruct* state)
{
	// remove notifications
	auto& transitions = state->getTransitions();
	for (auto& itc : transitions)
	{
		TransitionStruct& currentTransition = itc.second;
		KigsCore::GetNotificationCenter()->removeObserver(this, currentTransition.m_NotificationName);
	}

	// call end action
	state->invokeEndAction(this, 0);
}

void FSM::initFromDescription(CoreItemSP description)
{
	if (description->size())
	{
	
		beginStateMap();

		CoreItemSP states = description["StateMap"];

		CoreItemIterator iterstate = states.begin();
		CoreItemIterator iterstateend = states.end();

		while(iterstate != iterstateend)
		{
			kstl::string key;
			iterstate.getKey(key);

			beginStateDescription(key);

			CoreItemSP currentstate = *iterstate;

			if (currentstate->size())
			{
				initStateFromDescription(currentstate);
			}

			endStateDescription();
			iterstate++;
		}

		endStateMap();

	}
}

void	FSM::initStateFromDescription(CoreItemSP statedescription)
{
	CoreItemIterator iterstate = statedescription.begin();
	CoreItemIterator iterstateend = statedescription.end();

	while (iterstate != iterstateend)
	{
		kstl::string	key;
		iterstate.getKey(key);

		if (key == "actions")
		{
			CoreItemSP actionarray = *iterstate;

			for (u32 i = 0; i < actionarray->size(); i++)
			{
				kstl::string action(actionarray[i]);
				if (action != "")
				{
					addStateAction((STATE_ACTION_TYPE)i, action);
				}
			}
			
		}
		else if (key == "transitions")
		{
			
			initTransitionsFromDescription(*iterstate);
			
		}
		else if(key == "instance") // embedded instance description
		{
			initFSM_StateFromDescription(*iterstate);
		}
		else if (key == "importXML") // extern file
		{
			kstl::string filename(*iterstate);
			FSM_State* newstate = (FSM_State*)CoreModifiable::Import(filename);
			if (newstate)
			{
				addStateInstance(newstate);
			}
		}
		else if (key == "importJSON") // extern file
		{
			initStateFromJSONFile((kstl::string)*iterstate);
		}
		else if (key == "reference") // reference in current FSM
		{
			kstl::string instancename(*iterstate);
			kstl::set<CoreModifiable*> instances;
			GetSonInstancesByName("FSM_State", instancename, instances);
			if (instances.size() == 1)
			{
				FSM_State* newstate = (FSM_State*)*(instances.begin());
				// get one ref before removing it from this
				newstate->GetRef();
				removeItem(newstate);

				// then add it as a state
				addStateInstance(newstate);

			}
		}
		
		iterstate++;
	}

}

void FSM::initStateFromJSONFile(const kstl::string& filename)
{
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(filename);

	initFSM_StateFromDescription(L_Dictionary);

}

void	FSM::initTransitionsFromDescription(CoreItemSP transitiondescription)
{
	CoreItemIterator itertrans = transitiondescription.begin();
	CoreItemIterator itertransend = transitiondescription.end();

	while (itertrans != itertransend)
	{
		kstl::string	key;
		itertrans.getKey(key);

		CoreItemSP transarray = *itertrans;

		if (transarray->size() > 1)
		{
			addTransition(key, (kstl::string)transarray[0], (kstl::string)transarray[1]);
		}
		else
		{
			addTransition(key, (kstl::string) transarray[0]);
		}

		itertrans++;
	}
}

void	FSM::initFSM_StateFromDescription(CoreItemSP statedescription)
{

	CoreItemSP	stateInfos = statedescription["infos"];

	kstl::string	statetype,statename,initstate;
	
	statetype = (kstl::string)stateInfos[0];
	statename = (kstl::string)stateInfos[1];
	initstate = (kstl::string)stateInfos[2];

	FSM_State* newstate = (FSM_State*)KigsCore::GetInstanceOf(statename, statetype);
	newstate->setValue("Description", statedescription.get());
	newstate->setValue("InitState", initstate);
	newstate->Init();

	addStateInstance(newstate);	
}

void FSM::sendEvent(KigsID notifID, void* eventdata)
{
	FSM_Event	toAdd(notifID, eventdata);

	KigsID stateID = 0u;
	if (m_CurrentState)
	{
		// find good parent FSM
		auto* transitions = &(m_CurrentState->getTransitions());
		auto itfound = transitions->find(notifID);
		if (itfound != transitions->end())
		{
			TransitionStruct& currentTransition = (*itfound).second;

			stateID = currentTransition.m_TransitionStateID;
		}
		else // look parent transition
		{
			if (myParentFSM)
			{
				transitions = &(myParentFSM->m_CurrentState->getTransitions());
				itfound = transitions->find(notifID);
				if (itfound != transitions->end())
				{
					TransitionStruct& currentTransition = (*itfound).second;

					stateID = currentTransition.m_TransitionStateID;
				}
			}
		}
	}

	if (stateID != 0u)
	{
		// can set an event on this or on my parent 

		FSM* goodFSM = findFSMForStateID(stateID.toUInt());

		goodFSM->m_EventQueue.push(toAdd);
	}
}


DEFINE_METHOD(FSM, EventReceived)
{
	// retreive event ID
	maUInt* notificationID = ((maUInt*)getParameter(params, "NotificationID"));
	if (!notificationID)
	{
		return false;
	}

	unsigned int notifID = (*notificationID);

	sendEvent(notifID, privateParams);

	return false;
}





void	FSM_Manager::InitModifiable()
{
	if (!IsInit())
	{
		CoreModifiable::InitModifiable();
		CoreBaseApplication* currentApp = KigsCore::GetCoreApplication();
		currentApp->AddAutoUpdate(this);
	}
}

FSM_Manager::~FSM_Manager()
{
}

void	FSM_Manager::registerFSM(FSM* newfsm)
{
	// check that fsm is not already there
	kstl::vector<FSM*>::iterator	itc =	m_FSM_List.begin();
	kstl::vector<FSM*>::iterator	ite =	m_FSM_List.end();
	while (itc != ite)
	{
		if ((*itc) == newfsm)
		{
			// already in the least
			return;
		}
		++itc;
	}

	m_FSM_List.push_back(newfsm);

}
void	FSM_Manager::unregisterFSM(FSM* toremovefsm)
{
	// check that fsm is not already there
	kstl::vector<FSM*>::iterator	itc = m_FSM_List.begin();
	kstl::vector<FSM*>::iterator	ite = m_FSM_List.end();
	while (itc != ite)
	{
		if ((*itc) == toremovefsm)
		{
			m_FSM_List.erase(itc);
			return;
		}
		++itc;
	}
}


void FSM_Manager::Update(const Timer&  timer, void* addParam)
{
	CoreModifiable::Update(timer, addParam);

	kstl::vector<FSM*>::iterator	itc = m_FSM_List.begin();
	kstl::vector<FSM*>::iterator	ite = m_FSM_List.end();
	while (itc != ite)
	{
		(*itc)->updateFSM();
		++itc;
	}
};

