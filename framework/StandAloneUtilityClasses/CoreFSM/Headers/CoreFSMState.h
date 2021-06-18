#pragma once
#include "CoreModifiable.h"
#include "Upgrador.h"
#include "CoreFSMTransition.h"

class CoreFSM;

// base class for state, multiple inheritance
class CoreFSMStateBase
{
public:

	virtual void	start(CoreFSMStateBase* prevstate=0);
	virtual void	stop(CoreFSMStateBase* nextstate = 0);
	virtual bool Update(CoreModifiable* currentParentClass, u32& specialOrder, KigsID& newstate);

	const KigsID&	getID()
	{
		return dynamic_cast<UpgradorBase*>(this)->getID();
	}

	void	AddTransition(SP<CoreFSMTransition> t)
	{
		mTransitions.push_back(t);
	}

	virtual ~CoreFSMStateBase()
	{
		mTransitions.clear();
	}

	SP<CoreFSMTransition>	getTransition(const KigsID& transitionname)
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

protected:



	bool mIsInit = false; 
	bool mIsPersistent = false; 
	std::vector<SP<CoreFSMTransition>> mTransitions;

};

#define CoreFSMStateClass(baseclassname,statename)  CoreFSMState##baseclassname##statename

#define StringifyClassName(a) #a

#define CoreFSMStateClassName(baseclassname,statename)  StringifyClassName(CoreFSMState##baseclassname##statename)

#define START_DECLARE_COREFSMSTATE(baseclassname,statename) \
class 	CoreFSMState##baseclassname##statename : public Upgrador<baseclassname>,public CoreFSMStateBase \
{ \
protected: \
	START_UPGRADOR(CoreFSMState##baseclassname##statename);\
	

#define ADD_STATE_METHODS(...) UPGRADOR_METHODS(__VA_ARGS__);

#define END_DECLARE_COREFSMSTATE() \
};