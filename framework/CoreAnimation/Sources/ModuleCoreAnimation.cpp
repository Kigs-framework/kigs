#include "PrecompiledHeaders.h"
#include "Core.h"
#include "ModuleCoreAnimation.h"
#include "MiniInstanceFactory.h"
#include "CoreAction.h"
#include "CoreActionInterpolation.h"
#include "CoreActionDirectInterpolation.h"
#include "CoreActionUtility.h"
#include "CoreActionKeyFrame.h"
#include "CoreActionDirectKeyFrame.h"
#include "JSonFileParser.h"
#include "CoreSequenceLauncher.h"


kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>	ModuleCoreAnimation::myCoreItemOperatorCreateMethodMap;

IMPLEMENT_CLASS_INFO(ModuleCoreAnimation)

ModuleCoreAnimation::ModuleCoreAnimation(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	mySequences.clear();
}

ModuleCoreAnimation::~ModuleCoreAnimation()
{
}    

void ModuleCoreAnimation::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"CoreAnimation",params);
	core->RegisterMainModuleList(this,CoreAnimationModuleCoreIndex);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(),MiniInstanceFactory,MiniInstanceFactory,MiniInstanceFactory)
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreSequenceLauncher, CoreSequenceLauncher, CoreAnimation)
	// register actions
	myPrivateMiniFactory=KigsCore::GetInstanceOf("myPrivateMiniFactory","MiniInstanceFactory");

	// action using "slow" setValue
	MiniFactoryRegister(myPrivateMiniFactory,"Linear1D",CoreActionLinear<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"Linear2D",CoreActionLinear<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"Linear3D",CoreActionLinear<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"Linear4D",CoreActionLinear<Quaternion>);
	MiniFactoryRegister(myPrivateMiniFactory,"Hermite1D",CoreActionHermite<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"Hermite2D",CoreActionHermite<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"Hermite3D",CoreActionHermite<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"Hermite4D",CoreActionHermite<Quaternion>);
	MiniFactoryRegister(myPrivateMiniFactory,"SetValueString", CoreActionSetValue<kstl::string>);
	MiniFactoryRegister(myPrivateMiniFactory,"SetValue1D",CoreActionSetValue<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"SetValue2D",CoreActionSetValue<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"SetValue3D",CoreActionSetValue<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"SetValue4D",CoreActionSetValue<Quaternion>);

	// Keyframe
	MiniFactoryRegister(myPrivateMiniFactory,"KeyFrame1D",CoreActionKeyFrame<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"KeyFrame2D",CoreActionKeyFrame<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"KeyFrame3D",CoreActionKeyFrame<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"KeyFrame4D",CoreActionKeyFrame<Quaternion>);

	// action using "direct" value access
	MiniFactoryRegister(myPrivateMiniFactory,"DirectLinear1D",CoreActionDirectLinear<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectLinear2D",CoreActionDirectLinear<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectLinear3D",CoreActionDirectLinear<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectLinear4D",CoreActionDirectLinear<Quaternion>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectHermite1D",CoreActionDirectHermite<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectHermite2D",CoreActionDirectHermite<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectHermite3D",CoreActionDirectHermite<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectHermite4D",CoreActionDirectHermite<Quaternion>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectSetValueFloat1D",CoreActionDirectSetValue<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectSetValueInt1D",CoreActionDirectSetValue<int>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectSetValueBool1D",CoreActionDirectSetValue<bool>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectSetValue2D",CoreActionDirectSetValue<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectSetValue3D",CoreActionDirectSetValue<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectSetValue4D",CoreActionDirectSetValue<Quaternion>);

	// direct key frame
	MiniFactoryRegister(myPrivateMiniFactory,"DirectKeyFrameFloat1D",CoreActionDirectKeyFrame<kfloat>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectKeyFrameInt1D",CoreActionDirectKeyFrame<int>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectKeyFrameBool1D",CoreActionDirectKeyFrame<bool>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectKeyFrame2D",CoreActionDirectKeyFrame<Point2D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectKeyFrame3D",CoreActionDirectKeyFrame<Point3D>);
	MiniFactoryRegister(myPrivateMiniFactory,"DirectKeyFrame4D",CoreActionDirectKeyFrame<Quaternion>);

	MiniFactoryRegister(myPrivateMiniFactory,"RemoveFromParent",CoreActionRemoveFromParent);
	MiniFactoryRegister(myPrivateMiniFactory,"Notification",CoreActionSendMessage);
	MiniFactoryRegister(myPrivateMiniFactory,"Signal",CoreActionEmitSignal);
	MiniFactoryRegister(myPrivateMiniFactory,"Combo",CoreActionCombo);
	MiniFactoryRegister(myPrivateMiniFactory,"Serie",CoreActionSerie);
	MiniFactoryRegister(myPrivateMiniFactory,"Wait",CoreActionWait);
	MiniFactoryRegister(myPrivateMiniFactory,"ForLoop",CoreActionForLoop);
	MiniFactoryRegister(myPrivateMiniFactory,"DoWhile",CoreActionDoWhile);

	// function

	MiniFactoryRegister(myPrivateMiniFactory, "Function1D", CoreActionFunction1D);
	MiniFactoryRegister(myPrivateMiniFactory, "Function2D", CoreActionFunction2D);
	MiniFactoryRegister(myPrivateMiniFactory, "Function3D", CoreActionFunction3D);
	MiniFactoryRegister(myPrivateMiniFactory, "Function4D", CoreActionFunction4D);

	kstl::vector<CoreItemOperator<kfloat>::SpecificOperator> specificList;
	CoreItemOperator<kfloat>::SpecificOperator toAdd;
	toAdd.myKeyWord = "actionTime";
	toAdd.myCreateMethod = &ActionTimeOperator<kfloat>::create;
	specificList.push_back(toAdd);
	/*toAdd.myKeyWord = "sequenceTime";
	toAdd.myCreateMethod = &SequenceTimeOperator<kfloat>::create;
	specificList.push_back(toAdd);
	*/
	CoreItemOperator<kfloat>::ConstructContextMap(myCoreItemOperatorCreateMethodMap,&specificList);

	
}

kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	ModuleCoreAnimation::GetCoreItemOperatorConstructMap()
{
	return myCoreItemOperatorCreateMethodMap;
}


void ModuleCoreAnimation::Close()
{
    BaseClose();
}    

void ModuleCoreAnimation::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
	updateSequences(timer);
}    

void	ModuleCoreAnimation::addSequence(CoreSequence* sequence)
{
	unsigned int seqID=sequence->getID().toUInt();
	// retain
	sequence->GetRef();

	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator foundid=mySequences.find(seqID);
	
	// id already exists, search in vector
	if(foundid != mySequences.end())
	{
		// check for double add
		kstl::vector<CoreSequence*>& sequenceVector=(*foundid).second;
		kstl::vector<CoreSequence*>::iterator itSeq=sequenceVector.begin();
		kstl::vector<CoreSequence*>::iterator itSeqEnd=sequenceVector.end();
		while(itSeq != itSeqEnd)
		{
			if((*itSeq) == sequence)
			{
				return; // sequence is already in manager
			}
			++itSeq;
		}

		sequenceVector.push_back(sequence);

	}
	else
	{
		kstl::vector<CoreSequence*> toAdd;
		toAdd.push_back(sequence);
		mySequences[seqID]=toAdd;
	}
	
}

void	ModuleCoreAnimation::removeAllSequencesOnTarget(CoreModifiable* target)
{
	kstl::vector<CoreSequence*>	deadSequences;

	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator itMap=mySequences.begin();
	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator itMapEnd=mySequences.end();
	
	while(itMap != itMapEnd)
	{
		// search sequence in vector
		kstl::vector<CoreSequence*>& sequenceVector=(*itMap).second;
		kstl::vector<CoreSequence*>::iterator itSeq=sequenceVector.begin();
		kstl::vector<CoreSequence*>::iterator itSeqEnd=sequenceVector.end();
		while(itSeq != itSeqEnd)
		{
			if((*itSeq)->useModifiable(target))
			{
				deadSequences.push_back((*itSeq));
			}
			++itSeq;
		}
		++itMap;
	}

	// remove dead sequences
	kstl::vector<CoreSequence*>::iterator itSeq=deadSequences.begin();
	kstl::vector<CoreSequence*>::iterator itSeqEnd=deadSequences.end();
	while(itSeq != itSeqEnd)
	{
		removeSequence(*itSeq);
		++itSeq;
	}
}


void	ModuleCoreAnimation::removeSequence(CoreSequence* sequence)
{
	unsigned int seqID=sequence->getID().toUInt();

	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator foundid=mySequences.find(seqID);
	
	if(foundid != mySequences.end())
	{
		// search sequence in vector
		kstl::vector<CoreSequence*>& sequenceVector=(*foundid).second;
		kstl::vector<CoreSequence*>::iterator itSeq=sequenceVector.begin();
		kstl::vector<CoreSequence*>::iterator itSeqEnd=sequenceVector.end();
		while(itSeq != itSeqEnd)
		{
			if((*itSeq) == sequence)
			{
				
				(*itSeq)->Destroy();
				
				sequenceVector.erase(itSeq);
				if(sequenceVector.size() == 0) // remove also map entry
				{
					mySequences.erase(foundid);
				}
				
				return; 
			}
			++itSeq;
		}
	}
}

void	ModuleCoreAnimation::startSequence(CoreSequence* sequence, Timer* t)
{
	if (t) sequence->start(*t);
	else sequence->start();
};

void ModuleCoreAnimation::pauseSequence(CoreSequence* sequence, Timer* t)
{
	if (t) sequence->pause(*t);
	else sequence->pause();
}
void	ModuleCoreAnimation::stopSequence(CoreSequence* sequence, Timer* t)
{
	sequence->stop();
}
void	ModuleCoreAnimation::startSequenceAtFirstUpdate(CoreSequence* sequence)
{
	sequence->startAtFirstUpdate();
}

void ModuleCoreAnimation::updateSequences(const Timer& timer)
{
	kstl::vector<CoreSequence*>	deadSequences;

	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator itMap=mySequences.begin();
	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator itMapEnd=mySequences.end();
	
	while(itMap != itMapEnd)
	{
		// search sequence in vector
		kstl::vector<CoreSequence*>& sequenceVector=(*itMap).second;
		kstl::vector<CoreSequence*>::iterator itSeq=sequenceVector.begin();
		kstl::vector<CoreSequence*>::iterator itSeqEnd=sequenceVector.end();
		while(itSeq != itSeqEnd)
		{
			if((*itSeq)->update(timer))
			{
				deadSequences.push_back((*itSeq));
			}
			++itSeq;
		}
		++itMap;
	}

	// remove dead sequences
	kstl::vector<CoreSequence*>::iterator itSeq=deadSequences.begin();
	kstl::vector<CoreSequence*>::iterator itSeqEnd=deadSequences.end();
	while(itSeq != itSeqEnd)
	{
		removeSequence(*itSeq);
		++itSeq;
	}
}

// a registered animated object was destroy before its animation ended
// so we need to find and remove concerned sequences
DEFINE_METHOD(ModuleCoreAnimation, OnDestroyCallBack)
{
	kstl::vector<CoreSequence*>	deadSequences;

	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator itMap=mySequences.begin();
	kstl::map<unsigned int,kstl::vector<CoreSequence*> >::iterator itMapEnd=mySequences.end();
	
	while(itMap != itMapEnd)
	{
		// search sequence in vector
		kstl::vector<CoreSequence*>& sequenceVector=(*itMap).second;
		kstl::vector<CoreSequence*>::iterator itSeq=sequenceVector.begin();
		kstl::vector<CoreSequence*>::iterator itSeqEnd=sequenceVector.end();
		while(itSeq != itSeqEnd)
		{
			if((*itSeq)->useModifiable(sender))
			{
				deadSequences.push_back((*itSeq));
			}
			++itSeq;
		}
		++itMap;
	}

	// remove dead sequences
	kstl::vector<CoreSequence*>::iterator itSeq=deadSequences.begin();
	kstl::vector<CoreSequence*>::iterator itSeqEnd=deadSequences.end();
	while(itSeq != itSeqEnd)
	{
		(*itSeq)->removeTarget();
		removeSequence(*itSeq);
		++itSeq;
	}
	return true;
}

CoreSequence* ModuleCoreAnimation::createSequenceFromString(CoreModifiable* target, const kstl::string& json, Timer* reftimer)
{
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionaryFromString(json);

	if (!L_Dictionary.isNil())
	{
		CoreSequence* result = createSequenceFromCoreMap(target, L_Dictionary, reftimer);
		return result;
	}
	return 0;
}

CoreSequence*	ModuleCoreAnimation::createSequenceFromJSON(CoreModifiable* target,const kstl::string& file,Timer* reftimer)
{
	
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(file);
	
	if (!L_Dictionary.isNil())
	{
		CoreSequence* result=createSequenceFromCoreMap(target,L_Dictionary,reftimer);
		return result;
	}

	return 0;
}

CoreSequence*	ModuleCoreAnimation::createSequenceFromCoreMap(CoreModifiable* target, CoreItemSP& L_Dictionary,Timer* reftimer)
{
	CoreItemIterator iter=L_Dictionary->begin();

	kstl::string	key;
	iter.getKey(key);

	CoreSequence*	result = new CoreSequence(target, key, reftimer);

	CoreItem& actions= (CoreItem & )(*iter);

	unsigned int actionindex;
	for(actionindex = 0; actionindex<actions.size();actionindex++)
	{
		auto	actiondesc=actions[actionindex];
		CoreItemSP newaction=createAction(result,actiondesc);
		if(!newaction.isNil())
		{
			result->push_back(newaction);
		}
	}

	return result;
}

CoreItemSP		ModuleCoreAnimation::createAction(CoreSequence* sequence, CoreItemSP& actiondesc)
{
	CoreItemIterator firstactiondesc = actiondesc->begin(); // only one

	kstl::string	key;
	firstactiondesc.getKey(key);

	CoreAction* newaction = (CoreAction*)myPrivateMiniFactory->CreateClassInstance(key);
	if(newaction)
	{
		newaction->init(sequence,(CoreVector*)(*firstactiondesc).get());
	}
	return CoreItemSP(newaction, StealRefTag{});
}