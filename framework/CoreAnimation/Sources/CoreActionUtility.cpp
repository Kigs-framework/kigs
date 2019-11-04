#include "PrecompiledHeaders.h"
#include "CoreActionUtility.h"
#include "CoreVector.h"
#include "ModuleCoreAnimation.h"
#include "NotificationCenter.h"


void CoreActionRemoveFromParent::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// kdouble duration, int paramID => 2 params
	if(!(params->size() == 2))
	{
		return;
	}
#endif

	float readfloat;
	((CoreValue<float>&)(*params)[0]).getValue(readfloat);
	myDuration=readfloat;

	kstl::string readstring;
	((CoreValue<kstl::string>&)(*params)[1]).getValue(readstring);

	myParentTypeID=CharToID::GetID(readstring);
}

bool	CoreActionRemoveFromParent::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		const kstl::vector<CoreModifiable*>& parents=myTarget->GetParents();
		if(parents.size())
		{
			// search parent
			if(myParentTypeID != 0xFFFFFFFF)
			{
				kstl::vector<CoreModifiable*>::const_iterator itparent=parents.begin();
				kstl::vector<CoreModifiable*>::const_iterator itparentend=parents.end();
				while(itparent != itparentend)
				{
					if((*itparent)->isSubType(myParentTypeID))
					{
						myTarget->flagAsPostDestroy();
						(*itparent)->removeItem(myTarget);
						return true;
					}
					++itparent;
				}
			}
			else // remove from first found parent
			{
				myTarget->flagAsPostDestroy();
				(*(parents.begin()))->removeItem(myTarget);
				return true;
			}
		}
	}
	return false;
}

void CoreActionSendMessage::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// kdouble duration, message => 2 params
	if(!(params->size() >= 2))
	{
		return;
	}
#endif

	float readfloat;
	((CoreValue<float>&)(*params)[0]).getValue(readfloat);
	myDuration=readfloat;

	((CoreValue<kstl::string>&)(*params)[1]).getValue(myMessage);

	if ((*params).size() > 2)
	{
		myParam = (const usString&)(*params)[2];
	}
}

bool	CoreActionSendMessage::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(myStartTime+myDuration))
	{
		setIsDone();
		if (myParam.length())
		{
			KigsCore::GetNotificationCenter()->postNotificationName(myMessage, myTarget, &myParam);
		}
		else
		{
			KigsCore::GetNotificationCenter()->postNotificationName(myMessage, myTarget);
		}
		return true;
	}
	return false;
}

void CoreActionEmitSignal::init(CoreSequence* sequence, CoreVector* params)
{
	myTarget = sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// kdouble duration, message => 2 params
	if (!(params->size() >= 2))
	{
		return;
	}
#endif

	float readfloat;
	((CoreValue<float>&)(*params)[0]).getValue(readfloat);
	myDuration = readfloat;

	((CoreValue<kstl::string>&)(*params)[1]).getValue(mySignal);

	if ((*params).size() > 2)
	{
		myParam = (const usString&)(*params)[2];
	}
}

bool	CoreActionEmitSignal::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if ((time + TimeEpsilon) >= (myStartTime + myDuration))
	{
		setIsDone();
		myTarget->Emit(mySignal, myTarget, myParam);
		return true;
	}
	return false;
}


void	CoreActionCombo::setStartTime(kdouble t)
{
	CoreAction::setStartTime(t);
	kstl::vector<CoreAction*>::iterator itaction=myList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=myList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->setStartTime(t);
		(*itaction)->reset();				// make sure not in done mode
		++itaction;
	}
}

bool	CoreActionCombo::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool alldone=true;
	kstl::vector<CoreAction*>::iterator itaction=myList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=myList.end();

	while(itaction != itactionEnd)
	{
		
		bool finished=(*itaction)->update(time);
		if(!finished)
		{
			alldone=false;
		}
		
		++itaction;
	}

	return alldone;
	
}

void CoreActionCombo::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	myDuration=-1.0;
	bool	onehasnoduration=false;	

	unsigned int i;
	for(i=0;i<params->size();++i)
	{
		// create each action
		CoreMap<kstl::string>* tocreate=(CoreMap<kstl::string>*)&(*params)[i];
		CoreAction*	actiontoadd=module->createAction(sequence,tocreate);
		myList.push_back(actiontoadd);
		if(actiontoadd->getDuration()>myDuration)
		{
			myDuration=actiontoadd->getDuration();
		}
		if(actiontoadd->getDuration()<0.0)
		{
			onehasnoduration=true;
		}
	}
	if(onehasnoduration)
	{
		myDuration=-1.0f;
	}

}

CoreActionCombo::~CoreActionCombo()
{
	kstl::vector<CoreAction*>::iterator itaction=myList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=myList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->Destroy();
		++itaction;
	}
}


void	CoreActionSerie::setStartTime(kdouble t)
{
	CoreAction::setStartTime(t);
	kstl::vector<CoreAction*>::iterator itaction=myList.begin();

	// start first action
	(*itaction)->setStartTime(t);
	(*itaction)->reset();				// make sure not in done mode
	++itaction;
	kstl::vector<CoreAction*>::iterator itactionEnd=myList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->reset();				// make sure not in done mode
		++itaction;
	}
	myCurrentActionIndex=0;
}

bool	CoreActionSerie::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool done=false;
	CoreAction* current=(CoreAction*)myList[myCurrentActionIndex];
	while(!done)
	{
		if(current->update(time)) // action is finished, start next one
		{
			++myCurrentActionIndex;
			if(myCurrentActionIndex < myList.size())
			{
				kdouble previousend=current->getEndTime();
				current=(CoreAction*)myList[myCurrentActionIndex];
				current->setStartTime(previousend);
			}
			else
			{
				myCurrentActionIndex=0xFFFFFFFF;
				done=true;
			}
		}
		else
		{
			done=true;
		}
	}
	if(myCurrentActionIndex == 0xFFFFFFFF)
	{
		return true;
	}
	return false;
}

void CoreActionSerie::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	myDuration=0.0;
	bool	onehasnoduration=false;	

	unsigned int i;
	for(i=0;i<params->size();++i)
	{
		// create each action
		CoreMap<kstl::string>* tocreate=(CoreMap<kstl::string>*)&(*params)[i];
		CoreAction*	actiontoadd=module->createAction(sequence,tocreate);
		myList.push_back(actiontoadd);
		
		if(actiontoadd->getDuration()<0.0)
		{
			onehasnoduration=true;
		}
		myDuration+=actiontoadd->getDuration();	
	}

	if(onehasnoduration)
	{
		myDuration=-1.0f;
	}
}

CoreActionSerie::~CoreActionSerie()
{
	kstl::vector<CoreAction*>::iterator itaction=myList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=myList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->Destroy();
		++itaction;
	}
}


// Loops


// set son action start time
void	CoreActionForLoop::setStartTime(kdouble t)
{
	CoreAction::setStartTime(t);
	
	// start first action
	if(myActionToLoop)
	{
		myActionToLoop->setStartTime(t);
	}
	
}


bool	CoreActionForLoop::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool done=(myActionToLoop==0) || ((myLoopCount>=0)&&(myCurrentLoopIndex>=myLoopCount));
	
	while(!done)
	{
		if(myActionToLoop->update(time)) // action is finished, start it again ?
		{
			++myCurrentLoopIndex;
			kdouble previousend=myActionToLoop->getEndTime();

			// myLoopCount<0 => infinite loop
			if((myCurrentLoopIndex<myLoopCount) || (myLoopCount<0)) 
			{
				myActionToLoop->reset();
				myActionToLoop->setStartTime(previousend);
			}
			else
			{
				// set duration so that this action ends at same time the last son action loop end
				myDuration=myActionToLoop->getEndTime()-myStartTime;
				myActionToLoop->setIsDone();
				done=true;
				return true;
			}
		}
		else
		{
			done=true;
		}
		
	}
	
	return false;
}

void CoreActionForLoop::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	myDuration=-1.0;

	int readint;
	(*params)[0].getValue(readint); // loop count
	myLoopCount=readint;

	// create son action
	CoreMap<kstl::string>* tocreate=(CoreMap<kstl::string>*)&(*params)[1];
	myActionToLoop=module->createAction(sequence,tocreate);

	if(myActionToLoop)
	{
		myDuration=((kdouble)myLoopCount)*myActionToLoop->getDuration();	
		if(myDuration<0.0)
		{
			myDuration=-1.0;
		}
	}
	
}

CoreActionForLoop::~CoreActionForLoop()
{
	if(myActionToLoop)
	{
		myActionToLoop->Destroy();
	}
}

// set son action start time
void	CoreActionDoWhile::setStartTime(kdouble t)
{
	CoreAction::setStartTime(t);
	
	// start first action
	if(myActionToLoop)
	{
		myActionToLoop->setStartTime(t);
	}
	
}


bool	CoreActionDoWhile::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool	done=(myActionToLoop==0);

	bool	whileTest=true;
	myTarget->getValue(myParamID,whileTest);
	
	if(!whileTest) // end while
	{
		myDuration=time-myStartTime; // reset duration
		myActionToLoop->setIsDone();
		return true;
	}

	while(!done)
	{
		if(myActionToLoop->update(time)) // action is finished, start it again 
		{
			kdouble previousend=myActionToLoop->getEndTime();
			
			myActionToLoop->reset();
			myActionToLoop->setStartTime(previousend);
			
			if(myIsZeroDuration) // do it once per frame if zero duration
			{
				done=true;
			}
		}
		else
		{
			done=true;
		}
		
	}
	
	return false;
}

void CoreActionDoWhile::init(CoreSequence* sequence,CoreVector* params)
{
	myTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	myDuration=-1.0;

	kstl::string readstring;
	(*params)[0].getValue(readstring);
	myTarget = checkSubTarget(readstring);

	myParamID=CharToID::GetID(readstring);										// parameter to test

	// create son action
	CoreMap<kstl::string>* tocreate=(CoreMap<kstl::string>*)&(*params)[1];
	myActionToLoop=module->createAction(sequence,tocreate);

	if(myActionToLoop)
	{
		// special case for 0.0 duration
		if(myActionToLoop->getDuration()<=0.0)
		{
			myIsZeroDuration=true;
		}
	}
	
}

CoreActionDoWhile::~CoreActionDoWhile()
{
	if(myActionToLoop)
	{
		myActionToLoop->Destroy();
	}
}
