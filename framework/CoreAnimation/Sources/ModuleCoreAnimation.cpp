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


kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>	ModuleCoreAnimation::mCoreItemOperatorCreateMethodMap;

IMPLEMENT_CLASS_INFO(ModuleCoreAnimation)

ModuleCoreAnimation::ModuleCoreAnimation(const kstl::string& name,CLASS_NAME_TREE_ARG) : ModuleBase(name,PASS_CLASS_NAME_TREE_ARG)
{
	mSequences.clear();
}

ModuleCoreAnimation::~ModuleCoreAnimation()
{
}    

void ModuleCoreAnimation::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"CoreAnimation",params);
	core->RegisterMainModuleList(this,CoreAnimationModuleCoreIndex);
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), CoreSequenceLauncher, CoreSequenceLauncher, CoreAnimation)
	// register actions
	mPrivateMiniFactory = KigsCore::GetInstanceOf("myPrivateMiniFactory","MiniInstanceFactory");

	// action using "slow" setValue
	MiniFactoryRegister(mPrivateMiniFactory,"Linear1D",CoreActionLinear<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"Linear2D",CoreActionLinear<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"Linear3D",CoreActionLinear<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"Linear4D",CoreActionLinear<Vector4D>);
	MiniFactoryRegister(mPrivateMiniFactory,"Hermite1D",CoreActionHermite<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"Hermite2D",CoreActionHermite<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"Hermite3D",CoreActionHermite<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"Hermite4D",CoreActionHermite<Vector4D>);
	MiniFactoryRegister(mPrivateMiniFactory,"SetValueString", CoreActionSetValue<kstl::string>);
	MiniFactoryRegister(mPrivateMiniFactory,"SetValue1D",CoreActionSetValue<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"SetValue2D",CoreActionSetValue<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"SetValue3D",CoreActionSetValue<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"SetValue4D",CoreActionSetValue<Vector4D>);

	// Keyframe
	MiniFactoryRegister(mPrivateMiniFactory,"KeyFrame1D",CoreActionKeyFrame<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"KeyFrame2D",CoreActionKeyFrame<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"KeyFrame3D",CoreActionKeyFrame<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"KeyFrame4D",CoreActionKeyFrame<Vector4D>);

	// action using "direct" value access
	MiniFactoryRegister(mPrivateMiniFactory,"DirectLinear1D",CoreActionDirectLinear<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectLinear2D",CoreActionDirectLinear<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectLinear3D",CoreActionDirectLinear<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectLinear4D",CoreActionDirectLinear<Vector4D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectHermite1D",CoreActionDirectHermite<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectHermite2D",CoreActionDirectHermite<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectHermite3D",CoreActionDirectHermite<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectHermite4D",CoreActionDirectHermite<Vector4D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectSetValueFloat1D",CoreActionDirectSetValue<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectSetValueInt1D",CoreActionDirectSetValue<int>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectSetValueBool1D",CoreActionDirectSetValue<bool>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectSetValue2D",CoreActionDirectSetValue<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectSetValue3D",CoreActionDirectSetValue<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectSetValue4D",CoreActionDirectSetValue<Vector4D>);

	// direct key frame
	MiniFactoryRegister(mPrivateMiniFactory,"DirectKeyFrameFloat1D",CoreActionDirectKeyFrame<kfloat>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectKeyFrameInt1D",CoreActionDirectKeyFrame<int>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectKeyFrameBool1D",CoreActionDirectKeyFrame<bool>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectKeyFrame2D",CoreActionDirectKeyFrame<Point2D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectKeyFrame3D",CoreActionDirectKeyFrame<Point3D>);
	MiniFactoryRegister(mPrivateMiniFactory,"DirectKeyFrame4D",CoreActionDirectKeyFrame<Vector4D>);

	MiniFactoryRegister(mPrivateMiniFactory,"RemoveFromParent",CoreActionRemoveFromParent);
	MiniFactoryRegister(mPrivateMiniFactory,"Notification",CoreActionSendMessage);
	MiniFactoryRegister(mPrivateMiniFactory,"Signal",CoreActionEmitSignal);
	MiniFactoryRegister(mPrivateMiniFactory,"Combo",CoreActionCombo);
	MiniFactoryRegister(mPrivateMiniFactory,"Serie",CoreActionSerie);
	MiniFactoryRegister(mPrivateMiniFactory,"Wait",CoreActionWait);
	MiniFactoryRegister(mPrivateMiniFactory,"ForLoop",CoreActionForLoop);
	MiniFactoryRegister(mPrivateMiniFactory,"DoWhile",CoreActionDoWhile);

	// function

	MiniFactoryRegister(mPrivateMiniFactory, "Function1D", CoreActionFunction1D);
	MiniFactoryRegister(mPrivateMiniFactory, "Function2D", CoreActionFunction2D);
	MiniFactoryRegister(mPrivateMiniFactory, "Function3D", CoreActionFunction3D);
	MiniFactoryRegister(mPrivateMiniFactory, "Function4D", CoreActionFunction4D);

	kstl::vector<SpecificOperator> specificList;
	SpecificOperator toAdd;
	toAdd.mKeyWord = "actionTime";
	toAdd.mCreateMethod = &ActionTimeOperator<kfloat>::create;
	specificList.push_back(toAdd);
	/*toAdd.mKeyWord = "sequenceTime";
	toAdd.mCreateMethod = &SequenceTimeOperator<kfloat>::create;
	specificList.push_back(toAdd);
	*/
	CoreItemOperator<kfloat>::ConstructContextMap(mCoreItemOperatorCreateMethodMap,&specificList);

	
}

kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	ModuleCoreAnimation::GetCoreItemOperatorConstructMap()
{
	return mCoreItemOperatorCreateMethodMap;
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
	SP<CoreSequence> seq = sequence->SharedFromThis();

	auto foundid=mSequences.find(seqID);
	
	// id already exists, search in vector
	if(foundid != mSequences.end())
	{
		// check for double add
		auto& sequenceVector = (*foundid).second;
		auto itSeq=sequenceVector.begin();
		auto itSeqEnd=sequenceVector.end();
		while(itSeq != itSeqEnd)
		{
			if(itSeq->get() == sequence)
			{
				return; // sequence is already in manager
			}
			++itSeq;
		}
		sequenceVector.push_back(seq);
	}
	else
	{
		mSequences[seqID].push_back(seq);
	}
	
}

void	ModuleCoreAnimation::removeAllSequencesOnTarget(CoreModifiable* target)
{
	kstl::vector<SP<CoreSequence>>	deadSequences;

	auto itMap=mSequences.begin();
	auto itMapEnd=mSequences.end();
	
	while(itMap != itMapEnd)
	{
		// search sequence in vector
		auto& sequenceVector=(*itMap).second;
		auto itSeq = sequenceVector.begin();
		auto itSeqEnd = sequenceVector.end();
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
	for(auto& seq : deadSequences)
	{
		removeSequence(seq.get());
	}
}


void	ModuleCoreAnimation::removeSequence(CoreSequence* sequence)
{
	unsigned int seqID=sequence->getID().toUInt();

	auto foundid=mSequences.find(seqID);
	
	if(foundid != mSequences.end())
	{
		// search sequence in vector
		auto& sequenceVector=(*foundid).second;
		auto itSeq=sequenceVector.begin();
		auto itSeqEnd=sequenceVector.end();
		while(itSeq != itSeqEnd)
		{
			if(itSeq->get() == sequence)
			{
				sequenceVector.erase(itSeq);
				if(sequenceVector.size() == 0) // remove also map mEntry
				{
					mSequences.erase(foundid);
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
	kstl::vector<SP<CoreSequence>>	deadSequences;

	auto itMap=mSequences.begin();
	auto itMapEnd=mSequences.end();
	
	while(itMap != itMapEnd)
	{
		// search sequence in vector
		auto& sequenceVector=(*itMap).second;
		auto itSeq=sequenceVector.begin();
		auto itSeqEnd=sequenceVector.end();
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
	for(auto& seq : deadSequences)
	{
		removeSequence(seq.get());
	}
}

// a registered animated object was destroy before its animation ended
// so we need to find and remove concerned sequences
DEFINE_METHOD(ModuleCoreAnimation, OnDestroyCallBack)
{
	kstl::vector<SP<CoreSequence>>	deadSequences;
	auto itMap = mSequences.begin();
	auto itMapEnd = mSequences.end();
	
	while(itMap != itMapEnd)
	{
		// search sequence in vector
		auto& sequenceVector=(*itMap).second;
		auto itSeq=sequenceVector.begin();
		auto itSeqEnd=sequenceVector.end();
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
	for (auto& seq : deadSequences)
	{
		seq->removeTarget();
		removeSequence(seq.get());
	}
	return true;
}

SP<CoreSequence> ModuleCoreAnimation::createSequenceFromString(CMSP target, const kstl::string& json, Timer* reftimer)
{
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionaryFromString(json);

	if (L_Dictionary)
	{
		SP<CoreSequence> result = createSequenceFromCoreMap(std::move(target), L_Dictionary, reftimer);
		return result;
	}
	return 0;
}

SP<CoreSequence> ModuleCoreAnimation::createSequenceFromJSON(CMSP target,const kstl::string& file,Timer* reftimer)
{
	
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(file);
	
	if (L_Dictionary)
	{
		SP<CoreSequence> result=createSequenceFromCoreMap(std::move(target),L_Dictionary,reftimer);
		return result;
	}

	return 0;
}

SP<CoreSequence> ModuleCoreAnimation::createSequenceFromCoreMap(CMSP target, CoreItemSP& L_Dictionary,Timer* reftimer)
{
	CoreItemIterator iter=L_Dictionary->begin();

	kstl::string	key;
	iter.getKey(key);

	SP<CoreSequence> result = MakeRefCounted<CoreSequence>(std::move(target), key, reftimer);

	CoreItem& actions= (CoreItem & )(*iter);

	unsigned int actionindex;
	for(actionindex = 0; actionindex<actions.size();actionindex++)
	{
		auto	actiondesc=actions[actionindex];
		CoreItemSP newaction=createAction(result.get(), actiondesc);
		if(newaction)
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

	SP<CoreAction> newaction = CoreItemSP((CoreAction*)mPrivateMiniFactory->CreateClassInstance(key));
	if(newaction)
	{
		newaction->init(sequence,(CoreVector*)(*firstactiondesc).get());
	}
	return newaction;
}