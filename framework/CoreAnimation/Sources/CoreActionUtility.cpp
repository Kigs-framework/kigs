#include "PrecompiledHeaders.h"
#include "CoreActionUtility.h"
#include "CoreVector.h"
#include "ModuleCoreAnimation.h"
#include "NotificationCenter.h"

using namespace Kigs::Action;

void CoreActionRemoveFromParent::init(CoreSequence* sequence,CoreVector* params)
{
	mTarget = sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// double duration, int paramID => 2 params
	if(!(params->size() == 2))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	mDuration=readfloat;

	std::string readstring;
	(*params)[1]->getValue(readstring);

	mParentTypeID=CharToID::GetID(readstring);
}

bool	CoreActionRemoveFromParent::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(mStartTime+mDuration))
	{
		auto ptr = mTarget.lock();
		if (ptr)
		{
			const std::vector<CoreModifiable*>& parents = ptr->GetParents();
			if (parents.size())
			{
				// search parent
				if (mParentTypeID != 0xFFFFFFFF)
				{
					std::vector<CoreModifiable*>::const_iterator itparent = parents.begin();
					std::vector<CoreModifiable*>::const_iterator itparentend = parents.end();
					while (itparent != itparentend)
					{
						if ((*itparent)->isSubType(mParentTypeID))
						{
							ptr->flagAsPostDestroy();
							(*itparent)->removeItem(ptr);
							return true;
						}
						++itparent;
					}
				}
				else // remove from first found parent
				{
					ptr->flagAsPostDestroy();
					(*(parents.begin()))->removeItem(ptr);
					return true;
				}
			}
		}
	}
	return false;
}

void CoreActionSendMessage::init(CoreSequence* sequence,CoreVector* params)
{
	mTarget = sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// double duration, message => 2 params
	if(!(params->size() >= 2))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	mDuration=readfloat;

	(*params)[1]->getValue(mMessage);

	if ((*params).size() > 2)
	{
		mParam = (const usString&)(*params)[2];;
	}
}

bool	CoreActionSendMessage::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(mStartTime+mDuration))
	{
		setIsDone();
		auto ptr = mTarget.lock();
		if (ptr)
		{
			if (mParam.length())
			{
				KigsCore::GetNotificationCenter()->postNotificationName(mMessage, ptr.get(), &mParam);
			}
			else
			{
				KigsCore::GetNotificationCenter()->postNotificationName(mMessage, ptr.get());
			}
		}
		return true;
	}
	return false;
}

void CoreActionEmitSignal::init(CoreSequence* sequence, CoreVector* params)
{
	mTarget = sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// double duration, message => 2 params
	if (!(params->size() >= 2))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	mDuration = readfloat;

	(*params)[1]->getValue(mSignal);

	if ((*params).size() > 2)
	{
		mParam = (const usString&)(*params)[2];
	}
}

bool	CoreActionEmitSignal::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if ((time + TimeEpsilon) >= (mStartTime + mDuration))
	{
		setIsDone();
		auto ptr = mTarget.lock();
		if (ptr)
			ptr->EmitSignal(mSignal, ptr.get(), mParam);
		return true;
	}
	return false;
}


void	CoreActionCombo::setStartTime(double t)
{
	CoreAction::setStartTime(t);
	auto itaction=mList.begin();
	auto itactionEnd=mList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->setStartTime(t);
		(*itaction)->reset();				// make sure not in done mode
		++itaction;
	}
}

bool	CoreActionCombo::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	bool alldone=true;
	auto itaction=mList.begin();
	auto itactionEnd=mList.end();

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
	mTarget = sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	mDuration=-1.0;
	bool	onehasnoduration=false;	

	unsigned int i;
	for(i=0;i<params->size();++i)
	{
		// create each action
		CoreItemSP tocreate=(*params)[i];
		SP<CoreAction> actiontoaddSP = module->createAction(sequence,tocreate);
		mList.push_back(actiontoaddSP);
		
		if(actiontoaddSP->getDuration()>mDuration)
		{
			mDuration= actiontoaddSP->getDuration();
		}
		if(actiontoaddSP->getDuration()<0.0)
		{
			onehasnoduration=true;
		}
	}
	if(onehasnoduration)
	{
		mDuration=-1.0f;
	}

}

CoreActionCombo::~CoreActionCombo()
{
}


void	CoreActionSerie::setStartTime(double t)
{
	CoreAction::setStartTime(t);
	auto itaction=mList.begin();

	// start first action
	(*itaction)->setStartTime(t);
	(*itaction)->reset();				// make sure not in done mode
	++itaction;
	auto itactionEnd=mList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->reset();				// make sure not in done mode
		++itaction;
	}
	mCurrentActionIndex=0;
}

bool	CoreActionSerie::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	bool done=false;
	CoreAction* current=(CoreAction*)mList[mCurrentActionIndex].get();
	while(!done)
	{
		if(current->update(time)) // action is finished, start next one
		{
			++mCurrentActionIndex;
			if(mCurrentActionIndex < mList.size())
			{
				double previousend=current->getEndTime();
				current=(CoreAction*)mList[mCurrentActionIndex].get();
				current->setStartTime(previousend);
			}
			else
			{
				mCurrentActionIndex=0xFFFFFFFF;
				done=true;
			}
		}
		else
		{
			done=true;
		}
	}
	if(mCurrentActionIndex == 0xFFFFFFFF)
	{
		return true;
	}
	return false;
}

void CoreActionSerie::init(CoreSequence* sequence,CoreVector* params)
{
	mTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	mDuration=0.0;
	bool	onehasnoduration=false;	

	unsigned int i;
	for(i=0;i<params->size();++i)
	{
		// create each action
		CoreItemSP tocreate=(*params)[i];
		SP<CoreAction> action = module->createAction(sequence, tocreate);
		mList.push_back(action);
		
		if(action->getDuration()<0.0)
		{
			onehasnoduration=true;
		}
		mDuration+= action->getDuration();
	}

	if(onehasnoduration)
	{
		mDuration=-1.0f;
	}
}

CoreActionSerie::~CoreActionSerie()
{
}


// Loops


// set son action start time
void	CoreActionForLoop::setStartTime(double t)
{
	CoreAction::setStartTime(t);
	
	// start first action
	if(mActionToLoop)
	{
		mActionToLoop->setStartTime(t);
	}
	
}


bool	CoreActionForLoop::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	bool done=(mActionToLoop==0) || ((mLoopCount>=0)&&(mCurrentLoopIndex>=mLoopCount));
	
	while(!done)
	{
		if(mActionToLoop->update(time)) // action is finished, start it again ?
		{
			++mCurrentLoopIndex;
			double previousend=mActionToLoop->getEndTime();

			// mLoopCount<0 => infinite loop
			if((mCurrentLoopIndex<mLoopCount) || (mLoopCount<0)) 
			{
				mActionToLoop->reset();
				mActionToLoop->setStartTime(previousend);
			}
			else
			{
				// set duration so that this action ends at same time the last son action loop end
				mDuration=mActionToLoop->getEndTime()-mStartTime;
				mActionToLoop->setIsDone();
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
	mTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	mDuration=-1.0;

	int readint;
	(*params)[0]->getValue(readint); // loop count
	mLoopCount=readint;

	// create son action
	CoreItemSP tocreate = (*params)[1];
	SP<CoreAction> actiontoaddsp = module->createAction(sequence, tocreate);
	mActionToLoop = actiontoaddsp;

	if(mActionToLoop)
	{
		mDuration=((double)mLoopCount)*mActionToLoop->getDuration();	
		if(mDuration<0.0)
		{
			mDuration=-1.0;
		}
	}
	
}

CoreActionForLoop::~CoreActionForLoop()
{
}

// set son action start time
void	CoreActionDoWhile::setStartTime(double t)
{
	CoreAction::setStartTime(t);
	
	// start first action
	if(mActionToLoop)
	{
		mActionToLoop->setStartTime(t);
	}
	
}


bool	CoreActionDoWhile::protectedUpdate(double time)
{
	CoreAction::protectedUpdate(time);
	bool	done=(mActionToLoop==0);

	bool	whileTest=true;

	auto ptr = mTarget.lock();
	if (ptr)
		ptr->getValue(mParamID, whileTest);
	
	if(!whileTest) // end while
	{
		mDuration=time-mStartTime; // reset duration
		mActionToLoop->setIsDone();
		return true;
	}

	while(!done)
	{
		if(mActionToLoop->update(time)) // action is finished, start it again 
		{
			double previousend=mActionToLoop->getEndTime();
			
			mActionToLoop->reset();
			mActionToLoop->setStartTime(previousend);
			
			if(mIsZeroDuration) // do it once per frame if zero duration
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
	mTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	mDuration=-1.0;

	std::string readstring;
	(*params)[0]->getValue(readstring);
	mTarget = checkSubTarget(readstring);

	mParamID=CharToID::GetID(readstring);										// parameter to test

	// create son action
	CoreItemSP tocreate = (*params)[1];
	SP<CoreAction> actiontoaddsp = module->createAction(sequence, tocreate);
	mActionToLoop = actiontoaddsp;
	if(mActionToLoop)
	{
		// special case for 0.0 duration
		if(mActionToLoop->getDuration()<=0.0)
		{
			mIsZeroDuration=true;
		}
	}
	
}

CoreActionDoWhile::~CoreActionDoWhile()
{
}
