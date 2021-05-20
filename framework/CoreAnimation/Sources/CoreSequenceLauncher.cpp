#include "PrecompiledHeaders.h"
#include "CoreSequenceLauncher.h"
#include "ModuleCoreAnimation.h"
#include "NotificationCenter.h"
#include "CoreSequence.h"
#include "Core.h"

/* Sample xml code :

// sequence readed from json file and launched on parent instance when receiving "StartEnterAnim" notification

<Instance Name = "AnimButton" Type = "CoreSequenceLauncher" ID = "51">
	<CoreModifiableAttribute Type = "bool" Name = "StartOnFirstUpdate" Value = "false" Dynamic = "false" / >
	<CoreModifiableAttribute Type = "string" Name = "StartMessage" Value = "StartEnterAnim" Dynamic = "false" / >
	<CoreModifiableAttribute Type = "coreitem" Name = "Sequence" Value = "#AnimButtonStart.json" Dynamic = "false" / >
< / Instance>


// sequence readed directly in the xml and auto launched on parent instance (StartOnFirstUpdate) 


*/

IMPLEMENT_CLASS_INFO(CoreSequenceLauncher)

void	CoreSequenceLauncher::checkDeadSequences()
{
	bool deadFound = true;
	while (deadFound)
	{
		deadFound = false;
		auto itstart = mSequenceMap.begin();
		auto itend = mSequenceMap.end();
		// call destroy on each sequence
		while (itstart != itend)
		{
			if ((*itstart).second.use_count() == 1) // only kept by me
			{
				mSequenceMap.erase(itstart);
				deadFound = true;
				break;
			}
			++itstart;
		}
	}
}

void	CoreSequenceLauncher::addSequencesToParents()
{
	ModuleCoreAnimation* L_CoreAnimation = (ModuleCoreAnimation*)KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex);
	checkDeadSequences();
	for(auto parent : GetParents())
	{
		// check that we did not already create the sequence
		if (mSequenceMap.find(parent) == mSequenceMap.end() && (CoreItem*)mSequence)
		{
			CoreItemSP sequence = ((CoreItem*)mSequence)->SharedFromThis();
			auto L_Sequence = L_CoreAnimation->createSequenceFromCoreMap(parent, sequence);
			L_CoreAnimation->addSequence(L_Sequence.get());
			if (mStartOnFirstUpdate)
				L_Sequence->startAtFirstUpdate();
			mSequenceMap[parent] = L_Sequence;
		}
	}
}

//! try to init window, size and rendering screen must have been set
void	CoreSequenceLauncher::InitModifiable()
{
	if (GetParents().empty()) // start after it was added to parent
	{
		return;
	}
	CoreModifiable::InitModifiable();

	addSequencesToParents();

	if (mStartMessage.const_ref().size())
	{
		KigsCore::GetNotificationCenter()->addObserver(this, "Start", mStartMessage);
	}
}

void CoreSequenceLauncher::Start()
{
	if(mOnce) KigsCore::GetNotificationCenter()->removeObserver(this, mStartMessage);
	addSequencesToParents();
	for (auto& kv : mSequenceMap)
	{
		kv.second->startAtFirstUpdate();
	}
}

bool CoreSequenceLauncher::IsFinished()
{
	checkDeadSequences();
	if (mSequenceMap.size() == 0)
	{
		return true;
	}
	return false;
}

void CoreSequenceLauncher::Stop()
{
	for(auto& kv : mSequenceMap)
	{
		kv.second->stop();
	}
}
