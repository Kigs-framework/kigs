#include "PrecompiledHeaders.h"
#include "CoreActionUtility.h"
#include "CoreVector.h"
#include "ModuleCoreAnimation.h"
#include "NotificationCenter.h"


void CoreActionRemoveFromParent::init(CoreSequence* sequence,CoreVector* params)
{
	mTarget=sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// kdouble duration, int paramID => 2 params
	if(!(params->size() == 2))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	mDuration=readfloat;

	kstl::string readstring;
	(*params)[1]->getValue(readstring);

	mParentTypeID=CharToID::GetID(readstring);
}

bool	CoreActionRemoveFromParent::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(mStartTime+mDuration))
	{
		const kstl::vector<CoreModifiable*>& parents=mTarget->GetParents();
		if(parents.size())
		{
			// search parent
			if(mParentTypeID != 0xFFFFFFFF)
			{
				kstl::vector<CoreModifiable*>::const_iterator itparent=parents.begin();
				kstl::vector<CoreModifiable*>::const_iterator itparentend=parents.end();
				while(itparent != itparentend)
				{
					if((*itparent)->isSubType(mParentTypeID))
					{
						mTarget->flagAsPostDestroy();
						CMSP todel(mTarget, StealRefTag{});
						(*itparent)->removeItem(todel);
						return true;
					}
					++itparent;
				}
			}
			else // remove from first found parent
			{
				mTarget->flagAsPostDestroy();
				CMSP todel(mTarget, StealRefTag{});
				(*(parents.begin()))->removeItem(todel);
				return true;
			}
		}
	}
	return false;
}

void CoreActionSendMessage::init(CoreSequence* sequence,CoreVector* params)
{
	mTarget=sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// kdouble duration, message => 2 params
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
		mParam = (const usString&)(*params)[2];
	}
}

bool	CoreActionSendMessage::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if((time+TimeEpsilon)>=(mStartTime+mDuration))
	{
		setIsDone();
		if (mParam.length())
		{
			KigsCore::GetNotificationCenter()->postNotificationName(mMessage, mTarget, &mParam);
		}
		else
		{
			KigsCore::GetNotificationCenter()->postNotificationName(mMessage, mTarget);
		}
		return true;
	}
	return false;
}

void CoreActionEmitSignal::init(CoreSequence* sequence, CoreVector* params)
{
	mTarget = sequence->getTarget();

#ifdef _DEBUG // test parameters count
	// kdouble duration, message => 2 params
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

bool	CoreActionEmitSignal::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	// wait the end of the action
	if ((time + TimeEpsilon) >= (mStartTime + mDuration))
	{
		setIsDone();
		mTarget->EmitSignal(mSignal, mTarget, mParam);
		return true;
	}
	return false;
}


void	CoreActionCombo::setStartTime(kdouble t)
{
	CoreAction::setStartTime(t);
	kstl::vector<CoreAction*>::iterator itaction=mList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=mList.end();

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
	kstl::vector<CoreAction*>::iterator itaction=mList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=mList.end();

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
	mTarget=sequence->getTarget();

	ModuleCoreAnimation*	module=(ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);

	mDuration=-1.0;
	bool	onehasnoduration=false;	

	unsigned int i;
	for(i=0;i<params->size();++i)
	{
		// create each action
		CoreItemSP tocreate=(*params)[i];
		CoreItemSP	actiontoaddSP=module->createAction(sequence,tocreate);
		actiontoaddSP->GetRef();
		CoreAction* actiontoadd = (CoreAction * )actiontoaddSP.get();

		mList.push_back(actiontoadd);
		if(actiontoadd->getDuration()>mDuration)
		{
			mDuration=actiontoadd->getDuration();
		}
		if(actiontoadd->getDuration()<0.0)
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
	kstl::vector<CoreAction*>::iterator itaction=mList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=mList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->Destroy();
		++itaction;
	}
}


void	CoreActionSerie::setStartTime(kdouble t)
{
	CoreAction::setStartTime(t);
	kstl::vector<CoreAction*>::iterator itaction=mList.begin();

	// start first action
	(*itaction)->setStartTime(t);
	(*itaction)->reset();				// make sure not in done mode
	++itaction;
	kstl::vector<CoreAction*>::iterator itactionEnd=mList.end();

	while(itaction != itactionEnd)
	{
		(*itaction)->reset();				// make sure not in done mode
		++itaction;
	}
	mCurrentActionIndex=0;
}

bool	CoreActionSerie::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool done=false;
	CoreAction* current=(CoreAction*)mList[mCurrentActionIndex];
	while(!done)
	{
		if(current->update(time)) // action is finished, start next one
		{
			++mCurrentActionIndex;
			if(mCurrentActionIndex < mList.size())
			{
				kdouble previousend=current->getEndTime();
				current=(CoreAction*)mList[mCurrentActionIndex];
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
		CoreItemSP actiontoaddsp = module->createAction(sequence, tocreate);
		actiontoaddsp->GetRef();
		CoreAction* actiontoadd = (CoreAction * )actiontoaddsp.get();
		mList.push_back(actiontoadd);
		
		if(actiontoadd->getDuration()<0.0)
		{
			onehasnoduration=true;
		}
		mDuration+=actiontoadd->getDuration();	
	}

	if(onehasnoduration)
	{
		mDuration=-1.0f;
	}
}

CoreActionSerie::~CoreActionSerie()
{
	kstl::vector<CoreAction*>::iterator itaction=mList.begin();
	kstl::vector<CoreAction*>::iterator itactionEnd=mList.end();

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
	if(mActionToLoop)
	{
		mActionToLoop->setStartTime(t);
	}
	
}


bool	CoreActionForLoop::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool done=(mActionToLoop==0) || ((mLoopCount>=0)&&(mCurrentLoopIndex>=mLoopCount));
	
	while(!done)
	{
		if(mActionToLoop->update(time)) // action is finished, start it again ?
		{
			++mCurrentLoopIndex;
			kdouble previousend=mActionToLoop->getEndTime();

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
	CoreItemSP actiontoaddsp = module->createAction(sequence, tocreate);
	actiontoaddsp->GetRef();
	mActionToLoop = (CoreAction*)actiontoaddsp.get();

	if(mActionToLoop)
	{
		mDuration=((kdouble)mLoopCount)*mActionToLoop->getDuration();	
		if(mDuration<0.0)
		{
			mDuration=-1.0;
		}
	}
	
}

CoreActionForLoop::~CoreActionForLoop()
{
	if(mActionToLoop)
	{
		mActionToLoop->Destroy();
	}
}

// set son action start time
void	CoreActionDoWhile::setStartTime(kdouble t)
{
	CoreAction::setStartTime(t);
	
	// start first action
	if(mActionToLoop)
	{
		mActionToLoop->setStartTime(t);
	}
	
}


bool	CoreActionDoWhile::protectedUpdate(kdouble time)
{
	CoreAction::protectedUpdate(time);
	bool	done=(mActionToLoop==0);

	bool	whileTest=true;
	mTarget->getValue(mParamID,whileTest);
	
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
			kdouble previousend=mActionToLoop->getEndTime();
			
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

	kstl::string readstring;
	(*params)[0]->getValue(readstring);
	mTarget = checkSubTarget(readstring);

	mParamID=CharToID::GetID(readstring);										// parameter to test

	// create son action
	CoreItemSP tocreate = (*params)[1];
	CoreItemSP actiontoaddsp = module->createAction(sequence, tocreate);
	actiontoaddsp->GetRef();
	mActionToLoop = (CoreAction*)actiontoaddsp.get();

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
	if(mActionToLoop)
	{
		mActionToLoop->Destroy();
	}
}
