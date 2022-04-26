#pragma once
#include "CoreModifiable.h"
#include "Upgrador.h"
#include "CoreFSMTransition.h"

class CoreFSM;

// ****************************************
// * CoreFSMStateBase class
// * --------------------------------------
/**
 * \class	CoreFSMStateBase
 * \file	CoreFSMState.h
 * \ingroup CoreFSM
 * \brief	FSM state base class.
 *
 */
 // ****************************************

// base class for state, multiple inheritance
class CoreFSMStateBase
{
public:

	virtual void				start(CoreModifiable* currentParentClass, CoreFSMStateBase* prevstate);
	virtual void				stop(CoreModifiable* currentParentClass, CoreFSMStateBase* nextstate);
	// if a transition is valid, return it
	virtual CoreFSMTransition*	update(CoreModifiable* currentParentClass, u32& specialOrder, KigsID& newstate);

	const KigsID&	getID()
	{
		return dynamic_cast<UpgradorBase*>(this)->getID();
	}

	void	addTransition(SP<CoreFSMTransition> t)
	{
		mTransitions.push_back(t);
	}

	virtual ~CoreFSMStateBase()
	{
		mTransitions.clear();
	}

	// get a transition from the list given it's id
	SP<CoreFSMTransition>	getTransition(const KigsID& transitionname) const
	{
		for (auto t : mTransitions)
		{
			if (t->getNameID() == transitionname)
			{
				return t;
			}
		}
		return nullptr;
	}

	std::vector<KigsID>	getTransitionList() const
	{
		std::vector<KigsID> result;
		for (const auto& t : mTransitions)
		{
			result.push_back(t->getNameID());
		}
		return result;
	}

	bool	hasActiveTransition(CoreModifiable* currentParentClass) const
	{
		for (const auto& t : mTransitions)
		{
			if (t->checkTransition(currentParentClass))
				return true;
		}
		return false;
	}

	template<typename T>
	T* as() 
	{
		return static_cast<T*>(this);
	}

protected:

	friend class CoreFSM;
	// transition list for this state
	std::vector<SP<CoreFSMTransition>> mTransitions;

	// a state can activate one of its transition
	bool activateTransition(const KigsID& transitionname);

	// a state can also ask itself to pop 
	void popState();

	CoreFSMTransition*	mActiveTransition = nullptr;
};

#define StringifyClassName(a) #a

// Utility macro to declare states
// a state declaration with no additionnal method will look like 

/*

  START_DECLARE_COREFSMSTATE(Ghost, Appear)  // state for the Ghost class named Appear
  COREFSMSTATE_WITHOUT_METHODS()			 // this state has no specific method
  END_DECLARE_COREFSMSTATE()				 // end of state declaration
  
*/

// a state declaration with an additionnal checkDead method and a variable member will look like 

/*

   START_DECLARE_COREFSMSTATE(Ghost, Hunted) // state for the Ghost class named Hunted
   v2i	mPacmanSeenPos;
   COREFSMSTATE_METHODS(checkDead)
   END_DECLARE_COREFSMSTATE()

*/

// then state Hunted definition will look like

/*
void	CoreFSMStartMethod(Ghost, Hunted)
{
	// start state code here
	// this is an instance of Ghost here
	...
}
void	CoreFSMStopMethod(Ghost, Hunted)
{
	// stop state code here
	// this is an instance of Ghost here
	...
}

// checkDead method
DEFINE_UPGRADOR_METHOD(CoreFSMStateClass(Ghost, Hunted), checkDead)
{
	// this is an instance of Ghost here
	if(mIsDead)
		return true;

	return false;
}

DEFINE_UPGRADOR_UPDATE(CoreFSMStateClass(Ghost, Hunted))
{
	// this is an instance of Ghost here

	// retrieve variable in current state : 
	v2i pacmanLastPos=GetUpgrador()->mPacmanSeenPos;
	...
}

*/

#define CoreFSMStateClass(baseclassname,statename)  CoreFSMState##baseclassname##statename
#define CoreFSMStateClassMethods(baseclassname,statename)  CoreFSMState##baseclassname##statename::UpgradorMethods

#define CoreFSMStartMethod(baseclassname,statename)  CoreFSMState##baseclassname##statename::UpgradorMethods::start(class CoreFSMStateBase*)
#define CoreFSMStopMethod(baseclassname,statename)  CoreFSMState##baseclassname##statename::UpgradorMethods::stop(class CoreFSMStateBase*)

#define CoreFSMStateClassName(baseclassname,statename)  StringifyClassName(CoreFSMState##baseclassname##statename)


#define START_DECLARE_COREFSMSTATE(baseclassname,statename) \
class 	CoreFSMState##baseclassname##statename : public Upgrador<baseclassname>,public CoreFSMStateBase \
{ \
protected: \
	START_UPGRADOR(CoreFSMState##baseclassname##statename);

#define START_INHERITED_COREFSMSTATE(baseclassname,statename,parentstate) \
class 	CoreFSMState##baseclassname##statename : public CoreFSMState##baseclassname##parentstate \
{ \
protected: \
	START_UPGRADOR(CoreFSMState##baseclassname##statename);

#define DO_COREFSMSTATE_SUBCLASS_DECLARATION() class UpgradorMethods : public currentBaseClass \
{ \
 public:\
	UpgradorType*	GetUpgrador(){return (UpgradorType*)CoreModifiable::GetUpgrador();}\
	bool			UpgradorUpdate(const Timer& timer, void* addParam);\
	void			start(CoreFSMStateBase* prevstate);\
	void			stop(CoreFSMStateBase* nextstate);

#define DO_COREFSMSTATE_BASE_METHOD_DEFINITION() };\
public:\
bool	UpgradorUpdate(CoreModifiable* toUpdate, const Timer& timer, void* addParam) override\
{\
	return ((UpgradorMethods*)toUpdate)->UpgradorUpdate(timer, addParam);\
}\
virtual void	start(CoreModifiable* toStart,CoreFSMStateBase* prevstate) override\
{\
	((UpgradorMethods*)toStart)->start(prevstate);\
	CoreFSMStateBase::start(toStart,prevstate);\
}\
virtual void	stop(CoreModifiable* toStop,CoreFSMStateBase* nextstate) override\
{\
	CoreFSMStateBase::stop(toStop,nextstate);\
	((UpgradorMethods*)toStop)->stop(nextstate);\
}

#define COREFSM_WRAP_METHOD_NO_CTOR(name) inline bool	name##Wrap(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams){\
	kigs_impl::UnpackAndCall(&UpgradorMethods::name, this, sender, params); return false; }

#define COREFSM_WRAP_METHOD_PUSH_BACK(name) table.push_back({ #name, static_cast<CoreModifiable::ModifiableMethod>(&UpgradorMethods::name##Wrap) });


#define COREFSMSTATE_WITHOUT_METHODS() DO_COREFSMSTATE_SUBCLASS_DECLARATION() \
DO_COREFSMSTATE_BASE_METHOD_DEFINITION() \
virtual void GetMethodTable(kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
{\
}

#define COREFSMSTATE_METHODS(...) DO_COREFSMSTATE_SUBCLASS_DECLARATION() \
FOR_EACH(DECLARE_METHOD, __VA_ARGS__)\
DO_COREFSMSTATE_BASE_METHOD_DEFINITION() \
virtual void GetMethodTable(kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
{\
	FOR_EACH(UPGRADOR_METHOD_PUSH_BACK, __VA_ARGS__)\
}

#define END_DECLARE_COREFSMSTATE() \
};

#define COREFSMSTATE_WRAPMETHODS(...) DO_COREFSMSTATE_SUBCLASS_DECLARATION() \
FOR_EACH(COREFSM_WRAP_METHOD_NO_CTOR, __VA_ARGS__)\
DO_COREFSMSTATE_BASE_METHOD_DEFINITION() \
virtual void GetMethodTable(kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
{\
	FOR_EACH(COREFSM_WRAP_METHOD_PUSH_BACK, __VA_ARGS__)\
}

#define STARTCOREFSMSTATE_WRAPMETHODS() DO_COREFSMSTATE_SUBCLASS_DECLARATION() 

#define ENDCOREFSMSTATE_WRAPMETHODS(...) \
FOR_EACH(COREFSM_WRAP_METHOD_NO_CTOR, __VA_ARGS__)\
DO_COREFSMSTATE_BASE_METHOD_DEFINITION() \
virtual void GetMethodTable(kstl::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
{\
	FOR_EACH(COREFSM_WRAP_METHOD_PUSH_BACK, __VA_ARGS__)\
}

